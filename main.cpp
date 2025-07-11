#include "cherry/lexer/lexer.hpp"

#include <iostream>

#include "cherry/ir/ir_builder.hpp"
#include "cherry/parser/parser.hpp"
#include "cherry/printer/ast_printer.hpp"
#include "cherry/printer/ir_printer.hpp"

int main() {
    cherry::lexer::Lexer lexer;
    const auto tokens = lexer.lex_file("test.ch");

    for (const auto& token : tokens) {
        std::cout << token.to_str() << std::endl;
    }

    std::cout << "AST ~~~~~~~~~~~~" << std::endl;

    cherry::parser::Parser parser(tokens);
    const auto program = parser.parse_program();
    cherry::ast::printer::print_program(std::cout, program.get());

    // std::cout << "Semantical Analysis ~~~~~~~~~~~~" << std::endl;
    // cherry::semantic::SemanticAnalyzer analyzer;
    // analyzer.analyze(program.get());

    std::cout << "IR ~~~~~~~~~~~~" << std::endl;

    cherry::ir::IRBuilder builder;
    std::unique_ptr<cherry::ir::IRProgram> ir_program = std::move(builder.lower_program(*program));
    cherry::ir::printer::print_program(std::cout, ir_program.get());

    return 0;
}
