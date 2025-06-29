#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <vector>
#include <cherry/lexer/token.hpp>

#include "cherry/ast/program.hpp"
#include "cherry/ast/stmt/assignment.hpp"
#include "cherry/ast/stmt/declaration.hpp"
#include "cherry/ast/stmt/function_call.hpp"
#include "cherry/ast/stmt/function_decl.hpp"
#include "cherry/ast/stmt/if_statement.hpp"
#include "cherry/ast/stmt/return_statement.hpp"
#include "cherry/ast/stmt/while_loop.hpp"
#include "cherry/ast/stmt/scope_blocks.hpp"

namespace cherry::parser {

    class Parser {
        std::vector<lexer::Token> tokens;
        size_t index = 0;

        const lexer::Token& peek(size_t pos = 0);
        const lexer::Token& consume();
        [[nodiscard]] bool is_at_end() const;
        void advance(size_t pos = 1);
        [[nodiscard]] bool match(lexer::TokenType type);
        const lexer::Token& expect(lexer::TokenType type, const std::string& err_msg);

        std::vector<std::unique_ptr<ast::Expr>> parse_expr_list();
        std::unique_ptr<ast::IdentifierExpr> parse_identifier();
        std::vector<std::unique_ptr<ast::Statement>> parse_body_block();

        std::unique_ptr<ast::Expr> parse_function_expr();
        std::unique_ptr<ast::Expr> parse_logical_or();
        std::unique_ptr<ast::Expr> parse_logical_and();
        std::unique_ptr<ast::Expr> parse_equality();
        std::unique_ptr<ast::Expr> parse_comparison();
        std::unique_ptr<ast::Expr> parse_term();
        std::unique_ptr<ast::Expr> parse_factor();
        std::unique_ptr<ast::Expr> parse_unary();
        std::unique_ptr<ast::Expr> parse_primary();

        std::unique_ptr<ast::Declaration> parse_declaration();
        std::unique_ptr<ast::Assignment> parse_assignment();
        std::unique_ptr<ast::FunctionDecl> parse_function_decl();
        std::unique_ptr<ast::IfStatement> parse_if_statement();
        std::unique_ptr<ast::WhileStatement> parse_while_statement();
        std::unique_ptr<ast::ReturnStatement> parse_return_statement();
        std::unique_ptr<ast::FunctionCallStatement> parse_function_call_statement();
        std::unique_ptr<ast::PublicBlock> parse_public_scope_statement();
        std::unique_ptr<ast::PrivateBlock> parse_private_scope_statement();

        std::unique_ptr<ast::Statement> parse_stmt();

    public:
        explicit Parser(std::vector<lexer::Token> tokens);

        std::unique_ptr<ast::Program> parse_program();
    };

}

#endif //PARSER_HPP
