#include "cherry/llvm/llvm_code_gen.hpp"

#include <llvm/IR/Constants.h>

#include "cherry/ir/ir_variable_assignment.hpp"
#include "cherry/ir/ir_variable_decl.hpp"

namespace cherry::llvm {

    using namespace ::llvm;

    LLVMCodeGen::LLVMCodeGen(const std::string& module_name)
        : module(std::make_unique<Module>(module_name, context)),
          builder(std::make_unique<IRBuilder<>>(context)) {
        module->setTargetTriple("arm64-apple-macosx14.0.0");
    }

    void LLVMCodeGen::lower_program(const ir::IRProgram* program) {
        for (const auto& node : program->nodes) {
            if (auto* func = dynamic_cast<const ir::IRFunctionDecl*>(node)) {
                lower_function(func);
            }
        }
    }

    Type* LLVMCodeGen::to_llvm_type(const ast::Type& type) {
        switch (type) {
            case ast::Type::INT: return Type::getInt32Ty(context);
            case ast::Type::FLOAT: return Type::getDoubleTy(context);
            case ast::Type::BOOL: return Type::getInt1Ty(context);
            case ast::Type::VOID: return Type::getVoidTy(context);

            default:
                return nullptr;
        }
    }

    Type* LLVMCodeGen::to_llvm_type(const ir::IRNode* node) {
        if (auto* val = dynamic_cast<const ir::IRValue*>(node)) {
            return to_llvm_type(val->type);
        }

        return nullptr;
    }

    Value* LLVMCodeGen::lower_constant(const ir::IRConstant* constant) {
        switch (constant->type) {
            case ast::Type::INT:
                return ConstantInt::get(Type::getInt32Ty(context), std::stoi(constant->value));
            case ast::Type::FLOAT:
                return ConstantFP::get(Type::getDoubleTy(context), std::stod(constant->value));
            case ast::Type::BOOL:
                return ConstantInt::get(Type::getInt1Ty(context), constant->value == "true");

            default:
                return nullptr;
        }
    }

    Function* LLVMCodeGen::lower_function(const ir::IRFunctionDecl* func) {
        Type* return_type = to_llvm_type(func->return_type);
        auto* fn_type = FunctionType::get(return_type, {}, false);
        auto* function = Function::Create(fn_type, Function::ExternalLinkage, func->name, module.get());

        BasicBlock* entry = BasicBlock::Create(context, "entry", function);
        builder->SetInsertPoint(entry);

        named_values.clear();

        for (const auto& stmt : func->body) {
            lower_instruction(stmt);
        }

        if (return_type->isVoidTy()) {
            builder->CreateRetVoid();
        }

        verifyFunction(*function);
        return function;
    }

    void LLVMCodeGen::lower_instruction(const ir::IRNode* node) {
        if (auto* decl = dynamic_cast<const ir::IRVariableDecl*>(node)) {
            auto* ty = to_llvm_type(decl->type);
            Value* alloca_val = builder->CreateAlloca(ty, nullptr, decl->name);
            named_values[decl->name] = alloca_val;
        } else if (auto* assign = dynamic_cast<const ir::IRVariableAssignment*>(node)) {
            auto* ptr = named_values[assign->name];
            auto* value = lower_constant(dynamic_cast<const ir::IRConstant*>(assign->value));
            builder->CreateStore(value, ptr);
        }
    }

    const Module& LLVMCodeGen::get_module() {
        return *module;
    }

}