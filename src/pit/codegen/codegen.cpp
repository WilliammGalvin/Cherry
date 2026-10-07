#include "pit/codegen/codegen.hpp"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>

#include <llvm/Config/llvm-config.h>
#include <llvm/IR/LegacyPassManager.h>

#include <cassert>
#include <charconv>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace pit::codegen {

std::string codegen_error::to_str() const {
  std::ostringstream out;
  out << line << ':' << column << ": error: " << message;
  return out.str();
}

namespace {

struct error_signal {
  codegen_error error;
};

class generator {
public:
  generator(const ast::tree &tree, std::span<const lexer::token> tokens,
            const sema::analyzer &sema, std::string_view module_name)
      : tree_(tree), tokens_(tokens), sema_(sema),
        context_(std::make_unique<llvm::LLVMContext>()),
        module_(std::make_unique<llvm::Module>(module_name, *context_)),
        builder_(*context_) {}

  void run() {
    // Sized by symbol count, not by name. Both are indexed by symbol_id.
    const auto symbol_count = sema_.symbol_count();
    functions_.assign(symbol_count, nullptr);
    slots_.assign(symbol_count, nullptr);

    for (const ast::node_id root : tree_.roots())
      declare(root);

    for (const ast::node_id root : tree_.roots()) {
      if (tree_[root].kind == ast::node_kind::fn_decl)
        define(root);
    }

    std::string errors;
    llvm::raw_string_ostream stream{errors};
    if (llvm::verifyModule(*module_, &stream))
      fail(ast::node_id::none, "generated invalid IR: " + errors);
  }

  llvm::Module &module() noexcept { return *module_; }

private:
  // --- internal errors ----------------------------------------------------
  //
  // Reached only if sema and codegen disagree. Kept as a throw rather than an
  // abort so a bug surfaces as a message instead of a crash in release.

  [[noreturn]] void fail(ast::node_id at, std::string message) const {
    std::uint32_t line = 1;
    std::uint32_t column = 1;
    if (ast::present(at)) {
      const auto token = static_cast<std::uint32_t>(tree_[at].main_token);
      if (token < tokens_.size()) {
        line = tokens_[token].line;
        column = tokens_[token].column;
      }
    }
    throw error_signal{
        codegen_error{"internal error: " + std::move(message), line, column}};
  }

  std::string_view text_of(ast::node_id id) const {
    const auto token = static_cast<std::uint32_t>(tree_[id].main_token);
    return token < tokens_.size() ? tokens_[token].text : std::string_view{};
  }

  // --- types --------------------------------------------------------------
  //
  // Driven by sema::type_id, never by re-reading a type_name's spelling.
  // Unknown-type diagnosis already happened in sema::resolve_type_name.

  llvm::Type *llvm_type(sema::type_id type) {
    switch (type) {
    case sema::type_id::void_:
      return llvm::Type::getVoidTy(*context_);
    case sema::type_id::bool_:
      return llvm::Type::getInt1Ty(*context_);
    case sema::type_id::i32:
      return llvm::Type::getInt32Ty(*context_);
    case sema::type_id::i64:
      return llvm::Type::getInt64Ty(*context_);
    case sema::type_id::f32:
      return llvm::Type::getFloatTy(*context_);
    case sema::type_id::f64:
      return llvm::Type::getDoubleTy(*context_);
    case sema::type_id::str:
      return llvm::PointerType::get(*context_, 0);
    case sema::type_id::invalid:
    case sema::type_id::error:
      break;
    }
    throw error_signal{codegen_error{
        "internal error: unannotated node reached codegen", 1, 1}};
  }

  llvm::Type *type_of(ast::node_id id) { return llvm_type(sema_.type_of(id)); }

  // --- helpers ------------------------------------------------------------

  bool terminated() const {
    return builder_.GetInsertBlock()->getTerminator() != nullptr;
  }

  llvm::Function *current_function() const {
    return builder_.GetInsertBlock()->getParent();
  }

  llvm::AllocaInst *&slot_for(ast::node_id id) {
    const auto sym = sema_.resolved(id);
    if (!sema::present(sym))
      fail(id, "identifier has no resolved symbol");
    return slots_[static_cast<std::uint32_t>(sym)];
  }

  // Allocas go in the entry block so mem2reg can promote them. Emitting them
  // inside a loop body would leak stack on every iteration.
  llvm::AllocaInst *entry_alloca(llvm::Type *type, const llvm::Twine &name) {
    llvm::BasicBlock &entry = current_function()->getEntryBlock();
    llvm::IRBuilder<> tmp{&entry, entry.getFirstInsertionPt()};
    return tmp.CreateAlloca(type, nullptr, name);
  }

  // --- declarations -------------------------------------------------------

  void declare(ast::node_id id) {
    const ast::node &n = tree_[id];
    if (n.kind != ast::node_kind::fn_decl &&
        n.kind != ast::node_kind::extern_fn)
      fail(id, "non-function at top level survived sema");

    const auto sym = sema_.resolved(id);
    if (!sema::present(sym))
      fail(id, "function declaration has no resolved symbol");

    std::vector<llvm::Type *> params;
    for (const sema::type_id p : sema_.params_of(sym))
      params.push_back(llvm_type(p));

    auto *signature = llvm::FunctionType::get(llvm_type(sema_[sym].type),
                                              params, /*isVarArg=*/false);

    llvm::Function *fn =
        llvm::Function::Create(signature, llvm::Function::ExternalLinkage,
                               std::string{text_of(id)}, module_.get());

    functions_[static_cast<std::uint32_t>(sym)] = fn;
  }

  void define(ast::node_id id) {
    const ast::node &n = tree_[id];
    const auto extra =
        tree_.extra<ast::fn_extra>(static_cast<ast::extra_id>(n.lhs));

    const auto sym = sema_.resolved(id);
    llvm::Function *function = functions_[static_cast<std::uint32_t>(sym)];

    auto *entry = llvm::BasicBlock::Create(*context_, "entry", function);
    builder_.SetInsertPoint(entry);

    current_return_ = function->getReturnType();

    const auto params = tree_.range(extra.params_start, extra.params_end);
    std::uint32_t i = 0;
    for (auto &argument : function->args()) {
      const ast::node_id param = params[i++];
      const std::string name{text_of(param)};
      argument.setName(name);

      llvm::AllocaInst *store =
          entry_alloca(argument.getType(), name + ".addr");
      builder_.CreateStore(&argument, store);
      slot_for(param) = store;
    }

    emit_block(n.rhs);

    // Sema's always_returns guarantees a non-void function terminates on every
    // path, so this only fires for void functions falling off the end and for
    // dead blocks left behind by if/else where both arms returned.
    if (!terminated()) {
      if (current_return_->isVoidTy())
        builder_.CreateRetVoid();
      else
        builder_.CreateRet(llvm::Constant::getNullValue(current_return_));
    }
  }

  // --- statements ---------------------------------------------------------

  void emit_block(ast::node_id id) {
    const ast::node &n = tree_[id];
    const auto stmts = tree_.range(static_cast<std::uint32_t>(n.lhs),
                                   static_cast<std::uint32_t>(n.rhs));
    for (const ast::node_id stmt : stmts) {
      if (terminated())
        break; // everything after a return/break/continue is unreachable
      emit_stmt(stmt);
    }
  }

  void emit_stmt(ast::node_id id) {
    const ast::node &n = tree_[id];

    switch (n.kind) {
    case ast::node_kind::block:
      emit_block(id);
      return;

    case ast::node_kind::return_stmt:
      if (!ast::present(n.lhs))
        builder_.CreateRetVoid();
      else
        builder_.CreateRet(emit_expr(n.lhs));
      return;

    case ast::node_kind::const_decl:
    case ast::node_kind::var_decl: {
      const auto sym = sema_.resolved(id);
      if (!sema::present(sym))
        fail(id, "declaration has no resolved symbol");

      llvm::AllocaInst *store =
          entry_alloca(llvm_type(sema_[sym].type), text_of(id));

      // Initializer is emitted at the point of declaration, not in the entry
      // block -- only the alloca is hoisted.
      if (ast::present(n.rhs))
        builder_.CreateStore(emit_expr(n.rhs), store);

      slots_[static_cast<std::uint32_t>(sym)] = store;
      return;
    }

    case ast::node_kind::assign:
      builder_.CreateStore(emit_expr(n.rhs), slot_for(n.lhs));
      return;

    case ast::node_kind::if_stmt:
      emit_if(id, /*has_else=*/false);
      return;

    case ast::node_kind::if_else:
      emit_if(id, /*has_else=*/true);
      return;

    case ast::node_kind::while_stmt:
      emit_while(id);
      return;

    case ast::node_kind::break_stmt:
      if (loops_.empty())
        fail(id, "'break' outside a loop survived sema");
      builder_.CreateBr(loops_.back().after);
      return;

    case ast::node_kind::continue_stmt:
      if (loops_.empty())
        fail(id, "'continue' outside a loop survived sema");
      builder_.CreateBr(loops_.back().condition);
      return;

    case ast::node_kind::expr_stmt:
      if (ast::present(n.lhs))
        emit_expr(n.lhs);
      return;

    default:
      fail(id, "unsupported statement: " +
                   std::string{ast::node_kind_to_str(n.kind)});
    }
  }

  void emit_if(ast::node_id id, bool has_else) {
    const ast::node &n = tree_[id];

    ast::node_id then_node = n.rhs;
    ast::node_id else_node = ast::node_id::none;
    if (has_else) {
      const auto branches = tree_.extra_of<ast::if_extra>(id);
      then_node = branches.then_block;
      else_node = branches.else_block;
    }

    llvm::Value *condition = emit_expr(n.lhs);
    llvm::Function *function = current_function();

    auto *then_bb = llvm::BasicBlock::Create(*context_, "if.then", function);
    auto *else_bb =
        has_else ? llvm::BasicBlock::Create(*context_, "if.else", function)
                 : nullptr;
    auto *end_bb = llvm::BasicBlock::Create(*context_, "if.end", function);

    builder_.CreateCondBr(condition, then_bb, has_else ? else_bb : end_bb);

    builder_.SetInsertPoint(then_bb);
    emit_stmt(then_node);
    if (!terminated())
      builder_.CreateBr(end_bb);

    if (has_else) {
      builder_.SetInsertPoint(else_bb);
      emit_stmt(else_node);
      if (!terminated())
        builder_.CreateBr(end_bb);
    }

    // end_bb may end up with no predecessors (both arms returned). It is left
    // in place and picks up a terminator from define()'s fallback; the
    // verifier accepts unreachable blocks and simplifycfg deletes them.
    builder_.SetInsertPoint(end_bb);
  }

  void emit_while(ast::node_id id) {
    const ast::node &n = tree_[id];
    llvm::Function *function = current_function();

    auto *cond_bb = llvm::BasicBlock::Create(*context_, "while.cond", function);
    auto *body_bb = llvm::BasicBlock::Create(*context_, "while.body", function);
    auto *end_bb = llvm::BasicBlock::Create(*context_, "while.end", function);

    builder_.CreateBr(cond_bb);

    builder_.SetInsertPoint(cond_bb);
    llvm::Value *condition = emit_expr(n.lhs);
    builder_.CreateCondBr(condition, body_bb, end_bb);

    loops_.push_back(loop_target{cond_bb, end_bb});
    builder_.SetInsertPoint(body_bb);
    emit_stmt(n.rhs);
    if (!terminated())
      builder_.CreateBr(cond_bb);
    loops_.pop_back();

    builder_.SetInsertPoint(end_bb);
  }

  // --- expressions --------------------------------------------------------

  llvm::Value *emit_expr(ast::node_id id) {
    const ast::node &n = tree_[id];

    switch (n.kind) {
    case ast::node_kind::int_literal:
      return emit_int_literal(id);

    case ast::node_kind::bool_literal:
      return llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_),
                                    text_of(id) == "true" ? 1 : 0);

    case ast::node_kind::float_literal:
      return emit_float_literal(id);

    case ast::node_kind::identifier: {
      llvm::AllocaInst *store = slot_for(id);
      return builder_.CreateLoad(store->getAllocatedType(), store, text_of(id));
    }

    case ast::node_kind::call:
      return emit_call(id);

    case ast::node_kind::neg:
      return sema::is_float(sema_.type_of(n.lhs))
                 ? builder_.CreateFNeg(emit_expr(n.lhs), "neg")
                 : builder_.CreateNeg(emit_expr(n.lhs), "neg");

    case ast::node_kind::logical_not:
      return builder_.CreateNot(emit_expr(n.lhs), "not");

    case ast::node_kind::logical_and:
    case ast::node_kind::logical_or:
      return emit_short_circuit(id);

    default:
      break;
    }

    if (ast::is_binary(n.kind))
      return emit_binary(id);

    fail(id, "unsupported expression: " +
                 std::string{ast::node_kind_to_str(n.kind)});
  }

  // std::stoll throws std::out_of_range, which is not error_signal and would
  // escape emit_ir entirely. from_chars reports instead.
  llvm::Value *emit_int_literal(ast::node_id id) {
    const std::string_view text = text_of(id);

    std::uint64_t value = 0;
    const auto *begin = text.data();
    const auto *end = text.data() + text.size();
    const auto result = std::from_chars(begin, end, value);

    if (result.ec != std::errc{} || result.ptr != end)
      fail(id, "integer literal '" + std::string{text} + "' is out of range");

    // Width comes from sema, not a hardcoded i32, so i64 keeps working.
    return llvm::ConstantInt::get(type_of(id), value, /*IsSigned=*/true);
  }

  llvm::Value *emit_float_literal(ast::node_id id) {
    std::string text{text_of(id)};
    if (!text.empty() && (text.back() == 'f' || text.back() == 'F'))
      text.pop_back();

    return llvm::ConstantFP::get(type_of(id), llvm::StringRef{text});
  }

  llvm::Value *emit_binary(ast::node_id id) {
    const ast::node &n = tree_[id];

    // The operand type, not the result type: comparisons yield bool but must
    // be emitted as fcmp when their operands are floats.
    const bool fp = sema::is_float(sema_.type_of(n.lhs));

    llvm::Value *lhs = emit_expr(n.lhs);
    llvm::Value *rhs = emit_expr(n.rhs);

    switch (n.kind) {
    case ast::node_kind::add:
      return fp ? builder_.CreateFAdd(lhs, rhs, "add")
                : builder_.CreateAdd(lhs, rhs, "add");
    case ast::node_kind::sub:
      return fp ? builder_.CreateFSub(lhs, rhs, "sub")
                : builder_.CreateSub(lhs, rhs, "sub");
    case ast::node_kind::mul:
      return fp ? builder_.CreateFMul(lhs, rhs, "mul")
                : builder_.CreateMul(lhs, rhs, "mul");
    case ast::node_kind::div:
      return fp ? builder_.CreateFDiv(lhs, rhs, "div")
                : builder_.CreateSDiv(lhs, rhs, "div");
    case ast::node_kind::rem:
      return builder_.CreateSRem(lhs, rhs, "rem"); // sema restricts to integers

    case ast::node_kind::eq:
      return fp ? builder_.CreateFCmpOEQ(lhs, rhs, "eq")
                : builder_.CreateICmpEQ(lhs, rhs, "eq");
    case ast::node_kind::ne:
      return fp ? builder_.CreateFCmpONE(lhs, rhs, "ne")
                : builder_.CreateICmpNE(lhs, rhs, "ne");
    case ast::node_kind::lt:
      return fp ? builder_.CreateFCmpOLT(lhs, rhs, "lt")
                : builder_.CreateICmpSLT(lhs, rhs, "lt");
    case ast::node_kind::le:
      return fp ? builder_.CreateFCmpOLE(lhs, rhs, "le")
                : builder_.CreateICmpSLE(lhs, rhs, "le");
    case ast::node_kind::gt:
      return fp ? builder_.CreateFCmpOGT(lhs, rhs, "gt")
                : builder_.CreateICmpSGT(lhs, rhs, "gt");
    case ast::node_kind::ge:
      return fp ? builder_.CreateFCmpOGE(lhs, rhs, "ge")
                : builder_.CreateICmpSGE(lhs, rhs, "ge");

    default:
      fail(id, "not a binary operator");
    }
  }

  // && and || must not evaluate the right operand unconditionally.
  llvm::Value *emit_short_circuit(ast::node_id id) {
    const ast::node &n = tree_[id];
    const bool is_and = n.kind == ast::node_kind::logical_and;

    llvm::Value *lhs = emit_expr(n.lhs);
    llvm::BasicBlock *lhs_end = builder_.GetInsertBlock();
    llvm::Function *function = lhs_end->getParent();

    auto *rhs_bb = llvm::BasicBlock::Create(*context_, "lop.rhs", function);
    auto *end_bb = llvm::BasicBlock::Create(*context_, "lop.end", function);

    if (is_and)
      builder_.CreateCondBr(lhs, rhs_bb, end_bb);
    else
      builder_.CreateCondBr(lhs, end_bb, rhs_bb);

    builder_.SetInsertPoint(rhs_bb);
    llvm::Value *rhs = emit_expr(n.rhs);
    llvm::BasicBlock *rhs_end =
        builder_.GetInsertBlock(); // rhs may have branched
    builder_.CreateBr(end_bb);

    builder_.SetInsertPoint(end_bb);
    auto *boolean = llvm::Type::getInt1Ty(*context_);
    llvm::PHINode *phi = builder_.CreatePHI(boolean, 2, is_and ? "and" : "or");
    phi->addIncoming(llvm::ConstantInt::get(boolean, is_and ? 0 : 1), lhs_end);
    phi->addIncoming(rhs, rhs_end);
    return phi;
  }

  llvm::Value *emit_call(ast::node_id id) {
    const ast::node &n = tree_[id];

    const auto sym = sema_.resolved(n.lhs);
    if (!sema::present(sym))
      fail(id, "call has no resolved callee");

    llvm::Function *function = functions_[static_cast<std::uint32_t>(sym)];
    if (function == nullptr)
      fail(id, "callee was never declared in the module");

    const auto extra = tree_.extra_of<ast::call_extra>(id);
    const auto arg_nodes = tree_.range(extra.args_start, extra.args_end);

    // Arity and argument types were checked by sema.
    std::vector<llvm::Value *> args;
    args.reserve(arg_nodes.size());
    for (const ast::node_id arg : arg_nodes)
      args.push_back(emit_expr(arg));

    const bool returns_void = function->getReturnType()->isVoidTy();
    return builder_.CreateCall(function, args, returns_void ? "" : "call");
  }

  struct loop_target {
    llvm::BasicBlock *condition;
    llvm::BasicBlock *after;
  };

  const ast::tree &tree_;
  std::span<const lexer::token> tokens_;
  const sema::analyzer &sema_;

  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::Module> module_;
  llvm::IRBuilder<> builder_;

  // Both indexed by symbol_id -- no string hashing, and shadowing is correct
  // by construction because sema already gave inner declarations distinct ids.
  std::vector<llvm::Function *> functions_;
  std::vector<llvm::AllocaInst *> slots_;

  std::vector<loop_target> loops_;
  llvm::Type *current_return_{nullptr};
};

} // namespace

std::expected<std::string, codegen_error>
emit_ir(const ast::tree &tree, std::span<const lexer::token> tokens,
        const sema::analyzer &sema, std::string_view module_name) {
  try {
    generator gen{tree, tokens, sema, module_name};
    gen.run();

    std::string out;
    llvm::raw_string_ostream stream{out};
    gen.module().print(stream, nullptr);
    return out;
  } catch (const error_signal &signal) {
    return std::unexpected{signal.error};
  }
}

std::expected<void, codegen_error>
emit_object(const ast::tree &tree, std::span<const lexer::token> tokens,
            const sema::analyzer &sema, std::string_view module_name,
            std::string_view path) {
  try {
    generator gen{tree, tokens, sema, module_name};
    gen.run();

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    const std::string triple_text = llvm::sys::getDefaultTargetTriple();
    const llvm::Triple triple{triple_text};

    std::string error;
    gen.module().setTargetTriple(triple);
    const llvm::Target *target =
        llvm::TargetRegistry::lookupTarget(triple, error);

    if (target == nullptr)
      return std::unexpected{
          codegen_error{"no target for " + triple_text + ": " + error}};

    std::unique_ptr<llvm::TargetMachine> machine{target->createTargetMachine(
        triple, "generic", "", llvm::TargetOptions{}, llvm::Reloc::PIC_)};
    gen.module().setDataLayout(machine->createDataLayout());

    std::error_code ec;
    llvm::raw_fd_ostream out{std::string{path}, ec, llvm::sys::fs::OF_None};
    if (ec)
      return std::unexpected{codegen_error{
          "could not open " + std::string{path} + ": " + ec.message()}};

    llvm::legacy::PassManager passes;
    if (machine->addPassesToEmitFile(passes, out, nullptr,
                                     llvm::CodeGenFileType::ObjectFile))
      return std::unexpected{codegen_error{"target cannot emit object files"}};

    passes.run(gen.module());
    out.flush();
    return {};
  } catch (const error_signal &signal) {
    return std::unexpected{signal.error};
  }
}

} // namespace pit::codegen
