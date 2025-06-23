#include "cherry/lexer/lexer.hpp"

#include <iostream>
#include <cherry/semantic/semantic_analyzer.hpp>

#include "cherry/ir/ir_builder.hpp"
#include "cherry/llvm/llvm_code_gen.hpp"
#include "cherry/llvm/llvm_runner.hpp"
#include "cherry/parser/parser.hpp"
#include "cherry/printer/ast_printer.hpp"
#include "cherry/printer/ir_printer.hpp"
#include "cherry/printer/llvm_printer.hpp"

int main() {
    cherry::lexer::Lexer lexer;
    const auto tokens = lexer.lex_file("test.ch");

    for (const auto& token : tokens) {
        std::cout << token.to_str() << std::endl;
    }

    std::cout << "~~~~~~~~~~~~" << std::endl;

    cherry::parser::Parser parser(tokens);
    const auto program = parser.parse_program();
    cherry::ast::printer::print_program(std::cout, program.get());

    std::cout << "~~~~~~~~~~~~" << std::endl;

    // std::cout << "Semantically Analyzing..." << std::endl;
    // cherry::semantic::SemanticAnalyzer analyzer;
    // analyzer.analyze(program.get());
    //
    // std::cout << "~~~~~~~~~~~~" << std::endl;

    cherry::ir::IRBuilder builder;
    cherry::ir::IRProgram* ir_program = builder.lower_program(program.get());
    cherry::ir::printer::print_program(std::cout, ir_program);

    // std::cout << "~~~~~~~~~~~~" << std::endl;
    //
    // cherry::llvm::LLVMCodeGen gen("cherry_module");
    // gen.lower_program(ir_program);
    // cherry::llvm_printer::print_module(std::cout, gen.get_module());

    // std::cout << "~~~~~~~~~~~~" << std::endl;
    //
    // std::cout << "Compiling and executing..." << std::endl;
    // cherry::ir::LLVMRunner::run(gen.get_module());

    return 0;
}
