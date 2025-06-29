#ifndef LLVM_CODEGEN_HPP
#define LLVM_CODEGEN_HPP

#include <memory>
#include <unordered_map>
#include <string>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "cherry/ir/ir_program.hpp"
#include "cherry/ir/ir_function.hpp"
#include "cherry/ir/instructions/ir_instruction.hpp"
#include "cherry/ir/values/ir_value.hpp"

namespace cherry::codegen {

    class LLVMGenerator {
        void generate_function(ir::IRFunction* func);
        llvm::Value* generate_value(ir::IRValue* value);
        void generate_statement(ir::IRInstruction* stmt);

    public:
        llvm::LLVMContext context;
        llvm::IRBuilder<> builder;
        std::unique_ptr<llvm::Module> module;

        std::unordered_map<std::string, llvm::Value*> namedValues;

        LLVMGenerator()
            : builder(context),
              module(std::make_unique<llvm::Module>("cherry_module", context)) {}

        llvm::Module* generate(ir::IRProgram* program);
    };

} // namespace cherry::codegen

#endif // LLVM_CODEGEN_HPP