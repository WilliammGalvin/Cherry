#include "cherry/parser/parser.hpp"

#include <unordered_map>

#include "cherry/ast/expr/binary_op.hpp"
#include "cherry/ast/expr/function_call.hpp"
#include "cherry/ast/expr/literal.hpp"
#include "cherry/ast/expr/unary_op.hpp"
#include "cherry/parser/parse_error.hpp"

using namespace cherry::parser;

static const std::unordered_map<std::string, cherry::lexer::TokenType> builtin_functions = {
    { "print", cherry::lexer::BUILTIN_PRINT },
    { "println", cherry::lexer::BUILTIN_PRINTLN },
};

static cherry::ast::Type check_type_keyword(const cherry::lexer::TokenType type) {
    switch (type) {
        case cherry::lexer::KEYWORD_INT: return cherry::ast::INT;
        case cherry::lexer::KEYWORD_FLOAT: return cherry::ast::FLOAT;
        case cherry::lexer::KEYWORD_STRING: return cherry::ast::STRING;
        case cherry::lexer::KEYWORD_BOOL: return cherry::ast::BOOL;
        case cherry::lexer::KEYWORD_VOID: return cherry::ast::VOID;
        default: return cherry::ast::NONE;
    }
}

const cherry::lexer::Token& Parser::peek(const size_t pos) {
    if (index + pos > tokens.size()) {
        throw ParseError("Trying to peek out of bounds.");
    }

    return tokens[index + pos];
}

const cherry::lexer::Token& Parser::consume() {
    if (index >= tokens.size()) {
        throw ParseError("Trying to consume out of bounds.");
    }

    return tokens[index++];
}

void Parser::advance(const size_t pos) {
    index += pos;
}

bool Parser::is_at_end() const {
    return index >= tokens.size();
}

bool Parser::match(const lexer::TokenType type) {
    if (is_at_end()) return false;
    if (peek().type != type) return false;

    consume();
    return true;
}

const cherry::lexer::Token& Parser::expect(const lexer::TokenType type, const std::string& err_msg) {
    if (is_at_end() || peek().type != type) {
        throw ParseError(err_msg);
    }

    return consume();
}

std::vector<std::unique_ptr<cherry::ast::Statement>> Parser::parse_body_block() {
    std::vector<std::unique_ptr<ast::Statement>> body;
    while (!is_at_end() && peek().type != lexer::RIGHT_BRACE) {
        if (peek().type == lexer::LINE_END) {
            consume();
            continue;
        }

        body.push_back(std::move(parse_stmt()));
    }

    expect(lexer::RIGHT_BRACE, "Expected '}' to close body block.");
    return body;
}

std::vector<std::unique_ptr<cherry::ast::Expr> > Parser::parse_expr_list() {
    std::vector<std::unique_ptr<ast::Expr>> exprs;

    while (!is_at_end() && peek().type != lexer::RIGHT_PAREN) {
        exprs.push_back(std::move(parse_logical_or()));

        if (peek().type == lexer::RIGHT_PAREN) {
            break;
        }

        if (match(lexer::COMMA)) {
            continue;
        }

        throw ParseError("Expected ',' or ')' in expression list.");
    }

    if (is_at_end()) {
        throw ParseError("Argument list not terminated with ')'.");
    }

    return exprs;
}

std::unique_ptr<cherry::ast::IdentifierExpr> Parser::parse_identifier() {
    auto identifier = expect(lexer::IDENTIFIER, "Expected identifier.");
    return std::make_unique<ast::IdentifierExpr>(identifier.value);
}

std::unique_ptr<cherry::ast::Expr> Parser::parse_function_expr() {
    std::string callee;

    if (peek().type == lexer::IDENTIFIER) {
        callee = consume().value;
    } else {
        for (const auto& [key, value] : builtin_functions) {
            if (match(value)) {
                callee = key;
                break;
            }
        }
    }

    if (callee.empty()) {
        throw ParseError("Expected a function name or a built-in function.");
    }

    expect(lexer::LEFT_PAREN, "Expected '(' after function name.");
    auto args = parse_expr_list();
    expect(lexer::RIGHT_PAREN, "Expected ')' after function arguments.");
    return std::make_unique<ast::FunctionCallExpr>(callee, std::move(args));
}

std::unique_ptr<cherry::ast::Expr> Parser::parse_logical_or() {
    auto lhs = parse_logical_and();

    while (match(lexer::OR)) {
        auto rhs = parse_logical_or();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::LOGICAL_OR, std::move(lhs), std::move(rhs)
        ));
    }

    return lhs;
}

std::unique_ptr<cherry::ast::Expr> Parser::parse_logical_and() {
    auto lhs = parse_equality();

    while (match(lexer::AND)) {
        auto rhs = parse_equality();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::LOGICAL_AND, std::move(lhs), std::move(rhs)
        ));
    }

    return lhs;
}

std::unique_ptr<cherry::ast::Expr> Parser::parse_equality() {
    auto lhs = parse_comparison();

    while (match(lexer::DOUBLE_EQUAL)) {
        auto rhs = parse_comparison();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::EQUAL, std::move(lhs), std::move(rhs)
        ));
    }

    while (match(lexer::BANG_EQUAL)) {
        auto rhs = parse_comparison();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::NOT_EQUAL, std::move(lhs), std::move(rhs)
        ));
    }

    return lhs;
}

std::unique_ptr<cherry::ast::Expr> Parser::parse_comparison() {
    auto lhs = parse_term();

    while (match(lexer::GREATER_THAN)) {
        auto rhs = parse_term();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::GREATER_THAN, std::move(lhs), std::move(rhs)
        ));
    }

    while (match(lexer::GREATER_THAN_OR_EQUAL)) {
        auto rhs = parse_term();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::GREATER_EQUAL, std::move(lhs), std::move(rhs)
        ));
    }

    while (match(lexer::LESSER_THAN)) {
        auto rhs = parse_term();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::LESS_THAN, std::move(lhs), std::move(rhs)
        ));
    }

    while (match(lexer::LESSER_THAN_OR_EQUAL)) {
        auto rhs = parse_term();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::LESS_EQUAL, std::move(lhs), std::move(rhs)
        ));
    }

    return lhs;
}

std::unique_ptr<cherry::ast::Expr> Parser::parse_term() {
    auto lhs = parse_factor();

    while (match(lexer::PLUS)) {
        auto rhs = parse_factor();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::ADD, std::move(lhs), std::move(rhs)
        ));
    }

    while (match(lexer::MINUS)) {
        auto rhs = parse_factor();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::SUBTRACT, std::move(lhs), std::move(rhs)
        ));
    }

    return lhs;
}

std::unique_ptr<cherry::ast::Expr> Parser::parse_factor() {
    auto lhs = parse_unary();

    while (match(lexer::STAR)) {
        auto rhs = parse_unary();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::MULTIPLY, std::move(lhs), std::move(rhs)
        ));
    }

    while (match(lexer::SLASH)) {
        auto rhs = parse_unary();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::DIVIDE, std::move(lhs), std::move(rhs)
        ));
    }


    while (match(lexer::PERCENT)) {
        auto rhs = parse_unary();
        lhs = std::make_unique<ast::BinaryExpr>(ast::BinaryExpr(
            ast::BinaryOp::MODULO, std::move(lhs), std::move(rhs)
        ));
    }


    return lhs;
}

std::unique_ptr<cherry::ast::Expr> Parser::parse_unary() {
    if (match(lexer::BANG)) {
        auto unary = parse_unary();
        return std::make_unique<ast::UnaryExpr>(ast::UnaryExpr(
            ast::UnaryOp::LOGICAL_NOT, std::move(unary)
        ));
    }

    if (match(lexer::MINUS)) {
        auto unary = parse_unary();
        return std::make_unique<ast::UnaryExpr>(ast::UnaryExpr(
            ast::UnaryOp::NEGATE, std::move(unary)
        ));
    }

    return parse_primary();
}

std::unique_ptr<cherry::ast::Expr> Parser::parse_primary() {
    if (match(lexer::BOOLEAN_LITERAL_TRUE)) {
        return std::make_unique<ast::BooleanLiteral>(ast::BooleanLiteral(true));
    }

    if (match(lexer::BOOLEAN_LITERAL_FALSE)) {
        return std::make_unique<ast::BooleanLiteral>(ast::BooleanLiteral(false));
    }

    if (peek().type == lexer::INTEGER_LITERAL) {
        return std::make_unique<ast::IntegerLiteral>(ast::IntegerLiteral(
            std::stoll(consume().value)
        ));
    }

    if (peek().type == lexer::FLOAT_LITERAL) {
        return std::make_unique<ast::FloatLiteral>(ast::FloatLiteral(
            std::stod(consume().value)
        ));
    }

    if (peek().type == lexer::STRING_LITERAL) {
        return std::make_unique<ast::StringLiteral>(ast::StringLiteral(
            consume().value
        ));
    }

    if (match(lexer::LEFT_PAREN)) {
        auto expr = parse_logical_or();
        expect(lexer::RIGHT_PAREN, "Expected ')' after expression.");
        return expr;
    }

    if (
        index + 1 < tokens.size() &&
        peek(1).type == lexer::LEFT_PAREN
    ) {
        return parse_function_expr();
    }

    return parse_identifier();
}

std::unique_ptr<cherry::ast::Declaration> Parser::parse_declaration() {
    const bool is_const = match(lexer::KEYWORD_CONST);
    const ast::Type type = check_type_keyword(peek().type);

    if (type == ast::NONE) {
        throw ParseError("Expected a type keyword (var, int, float, string, bool) for declaration.");
    }

    advance();
    auto identifier_tok = expect(lexer::IDENTIFIER, "Expected identifier after type keyword.");
    expect(lexer::EQUAL, "Expected '=' after declaration type.");
    auto value = parse_logical_or();
    expect(lexer::SEMI_COLON, "Expected ';' after declaration type.");

    return std::make_unique<ast::Declaration>(
        is_const, identifier_tok.value, std::move(value), type
    );
}

std::unique_ptr<cherry::ast::Assignment> Parser::parse_assignment() {
    auto identifier = parse_identifier();
    expect(lexer::EQUAL, "Expected '=' for assignment.");
    auto value = parse_logical_or();
    expect(lexer::SEMI_COLON, "Expected ';' after assignment.");

    return std::make_unique<ast::Assignment>(
        std::move(identifier), std::move(value)
    );
}

std::unique_ptr<cherry::ast::FunctionDecl> Parser::parse_function_decl() {
    expect(lexer::KEYWORD_FUNC, "Expected 'func' keyword for function declaration.");
    auto identifier = parse_identifier();
    expect(lexer::LEFT_PAREN, "Expected '(' after function name.");

    std::vector<ast::Param> params;
    while (!is_at_end() && !match(lexer::RIGHT_PAREN)) {
        const ast::Type type = check_type_keyword(peek().type);

        if (type == ast::NONE) {
            throw ParseError("Expected a type keyword (int, float, string, bool) for parameter.");
        }

        advance();
        const auto name_tok = expect(lexer::IDENTIFIER, "Expected parameter name after type keyword.");
        params.emplace_back(type, name_tok.value);

        if (peek().type != lexer::RIGHT_PAREN) {
            expect(lexer::COMMA, "Expected ',' after parameter name.");
        }
    }

    expect(lexer::COLON, "Expected a return type for function declaration.");
    ast::Type return_type = check_type_keyword(peek().type);

    if (return_type == ast::NONE && match(lexer::KEYWORD_VOID)) {
        return_type = ast::VOID;
    }

    if (return_type == ast::NONE) {
        throw ParseError("Expected a return type for function declaration.");
    }

    advance();
    expect(lexer::LEFT_BRACE, "Expected '{' to start function body.");
    auto body = parse_body_block();

    return std::make_unique<ast::FunctionDecl>(
        std::move(identifier), std::move(params), return_type, std::move(body)
    );
}

std::unique_ptr<cherry::ast::IfStatement> Parser::parse_if_statement() {
    expect(lexer::KEYWORD_IF, "Expected 'if' keyword for condition.");
    expect(lexer::LEFT_PAREN, "Expected '(' after 'if' keyword.");
    auto condition = parse_logical_or();
    expect(lexer::RIGHT_PAREN, "Expected ')' after condition expression.");
    expect(lexer::LEFT_BRACE, "Expected '{' to start if body.");
    auto then = parse_body_block();

    std::vector<std::unique_ptr<ast::Statement>> else_body;
    if (match(lexer::KEYWORD_ELSE)) {
        expect(lexer::LEFT_BRACE, "Expected '{' to start else body.");
        else_body = parse_body_block();
    }

    return std::make_unique<ast::IfStatement>(std::move(condition), std::move(then), std::move(else_body));
}

std::unique_ptr<cherry::ast::WhileStatement> Parser::parse_while_statement() {
    expect(lexer::KEYWORD_WHILE, "Expected 'while' keyword for condition.");
    expect(lexer::LEFT_PAREN, "Expected '(' after 'while' keyword.");
    auto condition = parse_logical_or();
    expect(lexer::RIGHT_PAREN, "Expected ')' after condition expression.");
    expect(lexer::LEFT_BRACE, "Expected '{' to start while body.");
    auto body = parse_body_block();

    return std::make_unique<ast::WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<cherry::ast::ReturnStatement> Parser::parse_return_statement() {
    expect(lexer::KEYWORD_RETURN, "Expected 'return' keyword for condition.");
    auto value = parse_logical_or();
    expect(lexer::SEMI_COLON, "Expected ';' after return value.");

    return std::make_unique<ast::ReturnStatement>(std::move(value));
}

std::unique_ptr<cherry::ast::FunctionCallStatement> Parser::parse_function_call_statement() {
    std::string callee;

    if (peek().type == lexer::IDENTIFIER) {
        callee = consume().value;
    }

    for (const auto& [key, value] : builtin_functions) {
        if (match(value)) {
            callee = key;
            break;
        }
    }

    if (callee.empty()) {
        throw ParseError("Expected a function name or a built-in function.");
    }

    expect(lexer::LEFT_PAREN, "Expected '(' after function name.");
    auto args = parse_expr_list();
    expect(lexer::RIGHT_PAREN, "Expected ')' after function arguments.");

    return std::make_unique<ast::FunctionCallStatement>(
        callee, std::move(args)
    );
}

std::unique_ptr<cherry::ast::PublicBlock> Parser::parse_public_scope_statement() {
    expect(lexer::KEYWORD_PUBLIC, "Expected 'public' keyword.");
    expect(lexer::LEFT_BRACE, "Expected '{' to start public scope body.");
    auto body = parse_body_block();
    return std::make_unique<ast::PublicBlock>(std::move(body));
}

std::unique_ptr<cherry::ast::PrivateBlock> Parser::parse_private_scope_statement() {
    expect(lexer::KEYWORD_PRIVATE, "Expected 'private' keyword.");
    expect(lexer::LEFT_BRACE, "Expected '{' to start private scope body.");
    auto body = parse_body_block();
    return std::make_unique<ast::PrivateBlock>(std::move(body));
}

std::unique_ptr<cherry::ast::Statement> Parser::parse_stmt() {
    if (
        peek().type == lexer::KEYWORD_CONST ||
        peek().type == lexer::KEYWORD_INT ||
        peek().type == lexer::KEYWORD_FLOAT ||
        peek().type == lexer::KEYWORD_STRING ||
        peek().type == lexer::KEYWORD_BOOL
    ) {
        return parse_declaration();
    }

    if (peek().type == lexer::KEYWORD_FUNC) {
        return parse_function_decl();
    }

    if (peek().type == lexer::KEYWORD_IF) {
        return parse_if_statement();
    }

    if (peek().type == lexer::KEYWORD_WHILE) {
        return parse_while_statement();
    }

    if (peek().type == lexer::KEYWORD_RETURN) {
        return parse_return_statement();
    }

    if (
        peek().type == lexer::IDENTIFIER &&
        peek(1).type == lexer::EQUAL
    ) {
        return parse_assignment();
    }

    if (
        peek().type == lexer::IDENTIFIER &&
        peek(1).type == lexer::LEFT_PAREN
    ) {
        return parse_function_call_statement();
    }

    if (
        peek().type == lexer::KEYWORD_PUBLIC &&
        peek(1).type == lexer::LEFT_BRACE
    ) {
        return parse_public_scope_statement();
    }

    if (
        peek().type == lexer::KEYWORD_PRIVATE &&
        peek(1).type == lexer::LEFT_BRACE
    ) {
        return parse_private_scope_statement();
    }

    throw ParseError("Failed to parse next statement.");
}

Parser::Parser(std::vector<lexer::Token> tokens)
    : tokens(std::move(tokens)) {}

std::unique_ptr<cherry::ast::Program> Parser::parse_program() {
    std::vector<std::unique_ptr<ast::Statement>> stmts;

    while (!is_at_end() && peek().type != lexer::END_OF_FILE) {
        if (peek().type == lexer::LINE_END) {
            consume();
            continue;
        }

        auto stmt = parse_stmt();
        stmts.push_back(std::move(stmt));
    }

    return std::make_unique<ast::Program>(std::move(stmts));
}
