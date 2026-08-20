#include "common/source.hpp"
#include "pit/lexer/lexer.hpp"

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

  for (const auto &tok : *tokens)
    std::cout << tok.line << ':' << tok.column << '\t' << tok.to_str() << '\n';

  return 0;
}
