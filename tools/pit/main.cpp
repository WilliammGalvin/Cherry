#include "common/source.hpp"
#include "pit/ast/printer.hpp"
#include "pit/lexer/lexer.hpp"
#include "pit/parser/parser.hpp"

#include <iostream>

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: pit <file.pit>\n";
    return 2;
  }

  auto source = cherry::common::source_file::load(argv[1]);
  if (!source) {
    std::cerr << "error: " << source.error() << '\n';
    return 1;
  }

  pit::lexer::lexer lex{source->text()};
  const auto tokens = lex.lex();
  if (!tokens) {
    std::cerr << source->path() << ':' << tokens.error().to_str() << '\n';
    return 1;
  }

  pit::parser::parser parse{*tokens};
  const auto tree = parse.parse();
  if (!tree) {
    std::cerr << source->path() << ':' << tree.error().to_str() << '\n';
    return 1;
  }

  std::cout << pit::ast::dump(*tree, *tokens);
  return 0;
}
