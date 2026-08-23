#include "pit/parser/parser.hpp"

#include <optional>
#include <sstream>
#include <utility>

namespace pit::parser {

using lexer::token_type;

namespace {

struct binary_op {
  ast::node_kind kind;
  int precedence;
};

constexpr std::optional<binary_op> binary_op_for(token_type type) noexcept {
  switch (type) {
  case token_type::logical_or:
    return binary_op{ast::node_kind::logical_or, 1};
  case token_type::logical_and:
    return binary_op{ast::node_kind::logical_and, 2};
  case token_type::double_equal:
    return binary_op{ast::node_kind::eq, 3};
  case token_type::bang_equal:
    return binary_op{ast::node_kind::ne, 3};
  case token_type::less:
    return binary_op{ast::node_kind::lt, 4};
  case token_type::less_equal:
    return binary_op{ast::node_kind::le, 4};
  case token_type::greater:
    return binary_op{ast::node_kind::gt, 4};
  case token_type::greater_equal:
    return binary_op{ast::node_kind::ge, 4};
  case token_type::plus:
    return binary_op{ast::node_kind::add, 5};
  case token_type::minus:
    return binary_op{ast::node_kind::sub, 5};
  case token_type::star:
    return binary_op{ast::node_kind::mul, 6};
  case token_type::slash:
    return binary_op{ast::node_kind::div, 6};
  case token_type::percent:
    return binary_op{ast::node_kind::rem, 6};
  default:
    return std::nullopt;
  }
}

constexpr std::optional<ast::node_kind>
compound_assign_op(token_type type) noexcept {
  switch (type) {
  case token_type::plus_equal:
    return ast::node_kind::add;
  case token_type::minus_equal:
    return ast::node_kind::sub;
  case token_type::star_equal:
    return ast::node_kind::mul;
  case token_type::slash_equal:
    return ast::node_kind::div;
  case token_type::percent_equal:
    return ast::node_kind::rem;
  default:
    return std::nullopt;
  }
}

constexpr bool is_assignment(token_type type) noexcept {
  return type == token_type::equal || compound_assign_op(type).has_value();
}

} // namespace

std::string parse_error::to_str() const {
  std::ostringstream out;
  out << line << ':' << column << ": error: " << message;
  return out.str();
}

// --- token access -----------------------------------------------------------

const lexer::token &parser::peek(std::uint32_t ahead) const noexcept {
  const std::uint32_t at = index_ + ahead;
  return tokens_[at < tokens_.size() ? at : tokens_.size() - 1];
}

token_type parser::peek_type(std::uint32_t ahead) const noexcept {
  return peek(ahead).type;
}

ast::token_id parser::here() const noexcept {
  return static_cast<ast::token_id>(index_);
}

bool parser::at_end() const noexcept { return peek_type() == token_type::eof; }

bool parser::check(token_type type) const noexcept {
  return peek_type() == type;
}

ast::token_id parser::advance() noexcept {
  const auto id = here();
  if (!at_end())
    ++index_;

  return id;
}

bool parser::match(token_type type) noexcept {
  if (!check(type))
    return false;

  advance();
  return true;
}

ast::token_id parser::expect(token_type type, std::string_view what) {
  if (!check(type)) {
    std::ostringstream out;
    out << "expected " << what << ", found "
        << lexer::token_type_to_str(peek_type());
    fail(out.str());
  }

  return advance();
}

void parser::fail(std::string message) const {
  const lexer::token &tok = peek();
  throw error_signal{
      parse_error{std::move(message), tok.offset, tok.line, tok.column}};
}

// --- entry point ------------------------------------------------------------

parse_result parser::parse() {
  try {
    while (!at_end())
      tree_.add_root(parse_decl());
    return std::move(tree_);
  } catch (const error_signal &signal) {
    return std::unexpected{signal.error};
  }
}

// --- declarations -----------------------------------------------------------

ast::node_id parser::parse_decl() {
  if (check(token_type::kw_func))
    return parse_fn_decl();

  fail("expected a top-level declaration");
}

ast::node_id parser::parse_fn_decl() {
  expect(token_type::kw_func, "'fn'");
  const auto name = expect(token_type::identifier, "function name");
  expect(token_type::left_paren, "'(' after function name");

  const auto mark = scratch_.size();
  while (!check(token_type::right_paren) && !at_end()) {
    scratch_.push_back(parse_param());
    if (!check(token_type::right_paren))
      expect(token_type::comma, "',' or ')' after parameter");
  }

  expect(token_type::right_paren, "')' after parameters");

  const auto [params_start, params_end] =
      tree_.add_range(std::span{scratch_}.subspan(mark));
  scratch_.resize(mark);

  expect(token_type::colon, "':' before return type");
  const auto return_type = parse_type();
  const auto body = parse_block();

  const auto extra =
      tree_.add_extra(ast::fn_extra{params_start, params_end, return_type});
  return tree_.add(ast::node_kind::fn_decl, name,
                   static_cast<ast::node_id>(extra), body);
}

ast::node_id parser::parse_param() {
  const auto type = parse_type();
  const auto name = expect(token_type::identifier, "parameter name");
  return tree_.add(ast::node_kind::param, name, type);
}

ast::node_id parser::parse_type() {
  const auto name = expect(token_type::identifier, "a type name");
  return tree_.add(ast::node_kind::type_name, name);
}

// --- statements -------------------------------------------------------------

ast::node_id parser::parse_block() {
  const auto brace = expect(token_type::left_brace, "'{'");

  const auto mark = scratch_.size();
  while (!check(token_type::right_brace) && !at_end())
    scratch_.push_back(parse_stmt());
  expect(token_type::right_brace, "'}' to close block");

  const auto [start, end] = tree_.add_range(std::span{scratch_}.subspan(mark));
  scratch_.resize(mark);

  return tree_.add(ast::node_kind::block, brace,
                   static_cast<ast::node_id>(start),
                   static_cast<ast::node_id>(end));
}

bool parser::starts_declaration() const noexcept {
  if (check(token_type::kw_const))
    return true;

  return check(token_type::identifier) &&
         peek_type(1) == token_type::identifier;
}

ast::node_id parser::parse_stmt() {
  if (starts_declaration()) {
    const auto decl = parse_local_decl();
    expect(token_type::semi_colon, "';' after declaration");
    return decl;
  }

  switch (peek_type()) {
  case token_type::kw_if:
    return parse_if();
  case token_type::kw_while:
    return parse_while();
  case token_type::left_brace:
    return parse_block();
  case token_type::kw_return: {
    const auto ret = parse_return();
    expect(token_type::semi_colon, "';' after return");
    return ret;
  }
  case token_type::kw_break: {
    const auto tok = advance();
    expect(token_type::semi_colon, "';' after break");
    return tree_.add(ast::node_kind::break_stmt, tok);
  }
  case token_type::kw_continue: {
    const auto tok = advance();
    expect(token_type::semi_colon, "';' after continue");
    return tree_.add(ast::node_kind::continue_stmt, tok);
  }
  default:
    break;
  }

  const auto stmt = parse_assign_or_expr_stmt();
  expect(token_type::semi_colon, "';' after statement");
  return stmt;
}

ast::node_id parser::parse_local_decl() {
  const bool is_const = match(token_type::kw_const);
  const auto type = parse_type();
  const auto name = expect(token_type::identifier, "variable name");

  ast::node_id init = ast::node_id::none;
  if (match(token_type::equal))
    init = parse_expr();

  return tree_.add(is_const ? ast::node_kind::const_decl
                            : ast::node_kind::var_decl,
                   name, type, init);
}

ast::node_id parser::parse_assign_or_expr_stmt() {
  const auto target = parse_expr();

  if (!is_assignment(peek_type()))
    return tree_.add(ast::node_kind::expr_stmt, here(), target);

  if (tree_[target].kind != ast::node_kind::identifier)
    fail("left-hand side of assignment is not assignable");

  const auto target_token = tree_[target].main_token;
  const auto compound = compound_assign_op(peek_type());
  const auto op = advance();
  const auto value = parse_expr();

  if (!compound)
    return tree_.add(ast::node_kind::assign, op, target, value);

  const auto reread = tree_.add(ast::node_kind::identifier, target_token);
  const auto combined = tree_.add(*compound, op, reread, value);
  return tree_.add(ast::node_kind::assign, op, target, combined);
}

ast::node_id parser::parse_if() {
  const auto tok = expect(token_type::kw_if, "'if'");
  const auto condition = parse_expr();
  const auto then_block = parse_block();

  if (!match(token_type::kw_else))
    return tree_.add(ast::node_kind::if_stmt, tok, condition, then_block);

  const auto else_branch =
      check(token_type::kw_if) ? parse_if() : parse_block();

  const auto extra = tree_.add_extra(ast::if_extra{then_block, else_branch});
  return tree_.add(ast::node_kind::if_else, tok, condition,
                   static_cast<ast::node_id>(extra));
}

ast::node_id parser::parse_while() {
  const auto tok = expect(token_type::kw_while, "'while'");
  const auto condition = parse_expr();
  const auto body = parse_block();
  return tree_.add(ast::node_kind::while_stmt, tok, condition, body);
}

ast::node_id parser::parse_return() {
  const auto tok = expect(token_type::kw_return, "'return'");

  ast::node_id value = ast::node_id::none;
  if (!check(token_type::semi_colon))
    value = parse_expr();

  return tree_.add(ast::node_kind::return_stmt, tok, value);
}

// --- expressions ------------------------------------------------------------

ast::node_id parser::parse_expr() { return parse_binary(1); }

ast::node_id parser::parse_binary(int min_precedence) {
  ast::node_id lhs = parse_unary();

  while (true) {
    const auto op = binary_op_for(peek_type());
    if (!op || op->precedence < min_precedence)
      return lhs;

    const auto op_token = advance();
    const ast::node_id rhs = parse_binary(op->precedence + 1);
    lhs = tree_.add(op->kind, op_token, lhs, rhs);
  }
}

ast::node_id parser::parse_unary() {
  if (check(token_type::bang)) {
    const auto tok = advance();
    return tree_.add(ast::node_kind::logical_not, tok, parse_unary());
  }

  if (check(token_type::minus)) {
    const auto tok = advance();
    return tree_.add(ast::node_kind::neg, tok, parse_unary());
  }

  return parse_postfix();
}

ast::node_id parser::parse_postfix() {
  ast::node_id expr = parse_primary();
  while (check(token_type::left_paren))
    expr = parse_call(expr);

  return expr;
}

ast::node_id parser::parse_call(ast::node_id callee) {
  const auto paren = expect(token_type::left_paren, "'('");

  const auto mark = scratch_.size();
  while (!check(token_type::right_paren) && !at_end()) {
    scratch_.push_back(parse_expr());
    if (!check(token_type::right_paren))
      expect(token_type::comma, "',' or ')' in argument list");
  }
  expect(token_type::right_paren, "')' after arguments");

  const auto [start, end] = tree_.add_range(std::span{scratch_}.subspan(mark));
  scratch_.resize(mark);

  const auto extra = tree_.add_extra(ast::call_extra{start, end});
  return tree_.add(ast::node_kind::call, paren, callee,
                   static_cast<ast::node_id>(extra));
}

ast::node_id parser::parse_primary() {
  switch (peek_type()) {
  case token_type::integer_literal:
    return tree_.add(ast::node_kind::int_literal, advance());
  case token_type::float_literal:
    return tree_.add(ast::node_kind::float_literal, advance());
  case token_type::string_literal:
    return tree_.add(ast::node_kind::string_literal, advance());
  case token_type::boolean_literal_true:
  case token_type::boolean_literal_false:
    return tree_.add(ast::node_kind::bool_literal, advance());
  case token_type::identifier:
    return tree_.add(ast::node_kind::identifier, advance());
  case token_type::left_paren: {
    advance();
    const auto inner = parse_expr();
    expect(token_type::right_paren, "')' after expression");
    return inner;
  }
  default:
    fail("expected an expression");
  }
}

} // namespace pit::parser
