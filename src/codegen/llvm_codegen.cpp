#include "cherry/codegen/llvm_codegen.hpp"

#include <llvm/IR/Verifier.h>
#include <llvm/IR/Constants.h>
#include <stdexcept>
#include <cherry/ast/expr/unary_op.hpp>

#include "cherry/ir/instructions/ir_assignment.hpp"
#include "cherry/ir/instructions/ir_return.hpp"
#include "cherry/ir/instructions/ir_variable_decl.hpp"
#include "cherry/ir/values/ir_binary.hpp"
#include "cherry/ir/values/ir_constant.hpp"
#include "cherry/ir/values/ir_function_call.hpp"
#include "cherry/ir/values/ir_identifier.hpp"
#include "cherry/ir/values/ir_unary.hpp"

using namespace cherry;
using namespace cherry::codegen;

llvm::Module* LLVMGenerator::generate(ir::IRProgram* program) {
    for (const auto& func : program->functions) {
        generate_function(func.get());
    }
    return module.get();
}

void LLVMGenerator::generate_function(ir::IRFunction* func) {
    std::vector<llvm::Type*> paramTypes;
    for (const auto& param : func->params) {
        // Placeholder: support only INT for now
        paramTypes.push_back(llvm::Type::getInt32Ty(context));
    }

    llvm::FunctionType* funcType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context), paramTypes, false);

    llvm::Function* llvmFunc = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, func->name, module.get());

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", llvmFunc);
    builder.SetInsertPoint(entry);

    size_t idx = 0;
    for (auto& arg : llvmFunc->args()) {
        arg.setName(func->params[idx].name);
        llvm::AllocaInst* alloca = builder.CreateAlloca(llvm::Type::getInt32Ty(context), nullptr, arg.getName());
        builder.CreateStore(&arg, alloca);
        namedValues[arg.getName().str()] = alloca;
        ++idx;
    }

    for (const auto& stmt : func->body) {
        generate_statement(stmt.get());
    }

    if (!entry->getTerminator()) {
        builder.CreateRet(llvm::ConstantInt::get(context, llvm::APInt(32, 0)));
    }

    llvm::verifyFunction(*llvmFunc);
}

void LLVMGenerator::generate_statement(ir::IRInstruction* stmt) {
    using namespace ir;

    if (auto* decl = dynamic_cast<IRVariableDecl*>(stmt)) {
        llvm::AllocaInst* alloca = builder.CreateAlloca(
            llvm::Type::getInt32Ty(context), nullptr, decl->name);

        if (decl->initializer) {
            llvm::Value* initVal = generate_value(decl->initializer.get());
            builder.CreateStore(initVal, alloca);
        }

        namedValues[decl->name] = alloca;
    }

    else if (auto* assign = dynamic_cast<IRAssignment*>(stmt)) {
        llvm::Value* val = generate_value(assign->value.get());
        llvm::Value* ptr = namedValues.at(assign->target);
        builder.CreateStore(val, ptr);
    }

    else if (auto* ret = dynamic_cast<IRReturn*>(stmt)) {
        llvm::Value* retval = generate_value(ret->value.get());
        builder.CreateRet(retval);
    }

    else {
        throw std::runtime_error("Unsupported IRInstruction in generate_statement");
    }
}

llvm::Value* LLVMGenerator::generate_value(ir::IRValue* value) {
    using namespace ir;

    if (auto* c = dynamic_cast<IRConstant*>(value)) {
        return llvm::ConstantInt::get(context, llvm::APInt(32, std::get<long long>(c->value)));
    }

    if (auto* id = dynamic_cast<IRIdentifier*>(value)) {
        llvm::Value* ptr = namedValues.at(id->name);
        return builder.CreateLoad(llvm::Type::getInt32Ty(context), ptr, id->name + ".val");
    }

    if (auto* bin = dynamic_cast<IRBinary*>(value)) {
        llvm::Value* lhs = generate_value(bin->left.get());
        llvm::Value* rhs = generate_value(bin->right.get());

        switch (bin->op) {
            case BinaryOperation::ADD: return builder.CreateAdd(lhs, rhs);
            case BinaryOperation::SUBTRACT: return builder.CreateSub(lhs, rhs);
            case BinaryOperation::MULTIPLY: return builder.CreateMul(lhs, rhs);
            case BinaryOperation::DIVIDE: return builder.CreateSDiv(lhs, rhs);
            case BinaryOperation::MODULO: return builder.CreateSRem(lhs, rhs);
            default: throw std::runtime_error("Unsupported binary op");
        }
    }

    if (auto* unary = dynamic_cast<IRUnary*>(value)) {
        llvm::Value* operand = generate_value(unary->operand.get());
        switch (unary->op) {
            case NEGATE: return builder.CreateNeg(operand);
            case LOGICAL_NOT: return builder.CreateNot(operand);

            default:
                throw std::runtime_error("Unsupported unary op");
        }
    }

    if (auto* call = dynamic_cast<IRFunctionCall*>(value)) {
        std::vector<llvm::Value*> args;
        for (const auto& arg : call->arguments) {
            args.push_back(generate_value(arg.get()));
        }
        llvm::Function* callee = module->getFunction(call->callee);
        if (!callee) throw std::runtime_error("Unknown function: " + call->callee);
        return builder.CreateCall(callee, args);
    }

    throw std::runtime_error("Unknown IRValue type");
}