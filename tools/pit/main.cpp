#include "launch_args.hpp"
#include "pit/lexer/lexer.hpp"
#include "pit/lexer/token.hpp"
#include "pit/parser/parse_error.hpp"
#include "pit/parser/parser.hpp"
#include "source_reader.hpp"

#include <cstdlib>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#ifndef CHERRY_VERSION
#define CHERRY_VERSION "unknown"
#endif

using error_message = const std::string &;
using errors_list = std::vector<std::string>;
using compile_errors = std::optional<errors_list>;

void print_compile_success() {
  //
}

void print_error(error_message msg) {
  std::cerr << "\033[1;31merror:\033[0m " << msg << "\n";
}

void print_errors(const errors_list &errors) {
  for (const auto &e : errors)
    print_error(e);
}

compile_errors compile(const std::string &file_name) {
  errors_list errors{};

  const auto file_src = read_source_file(file_name);
  if (!file_src) {
    errors.push_back(file_src.error());
    return errors;
  }

  const auto lex_res = pit::lexer::lex(*file_src);
  if (!lex_res) {
    errors.push_back(pit::lexer::to_str(lex_res.error()));
    return errors;
  }

  const pit::parser::token_input tokens = *lex_res;
  const auto parse_res = pit::parser::parse(tokens);
  if (!parse_res) {
    errors.push_back(pit::parser::to_str(parse_res.error()));
    return errors;
  }

  // WIP

  return std::nullopt;
}

int main(int argc, char **argv) {
  const launch_args args{argc, argv};

  if (!args.errors().empty()) {
    print_errors(args.errors());
    return 2;
  }

  if (args.args().size() != 1) {
    print_error(std::format("usage: {} <file-name>", args.launch_name()));
    return 2;
  }

  compile_errors compiler_errors = compile(args.args()[0]);
  if (compiler_errors) {
    print_errors(*compiler_errors);
    return EXIT_FAILURE;
  }

  print_compile_success();
  return EXIT_SUCCESS;
}
