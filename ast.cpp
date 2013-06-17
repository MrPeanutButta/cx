#include <vector>
#include <string>
#include <map>
#include "ast.h"
#include "parser.h"

//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//

/// GetTokPrecedence - Get the precedence of the pending binary operator token.

int TParser::GetTokPrecedence(void) {
    //if (!isascii(CurTok))
    //return -1;

    // Make sure it's a declared binop.
    int TokPrec = BinopPrecedence[token];
    if (TokPrec <= 0) return -1;
    return TokPrec;
}

/// Error* - These are little helper functions for error handling.

ExprAST *Error(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return 0;
}

PrototypeAST *ErrorP(const char *Str) {
    Error(Str);
    return 0;
}

FunctionAST *ErrorF(const char *Str) {
    Error(Str);
    return 0;
}

//ExprAST *ParseExpression();

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'

ExprAST *TParser::ParseIdentifierExpr(void) {
    string IdName = pToken->String();

    this->GetToken(); // eat identifier.

    if (token != tcLParen) // Simple variable ref.
        return new VariableExprAST(IdName);

    // Call.
    this->GetToken(); // eat (
    vector<ExprAST*> Args;
    if (token != tcLParen) {
        while (1) {
            ExprAST *Arg = ParseExpression();
            if (!Arg) return 0;
            Args.push_back(Arg);

            if (token == tcRParen) break;

            if (token != tcComma)
                return Error("Expected ')' or ',' in argument list");
            this->GetToken();
        }
    }

    // Eat the ')'.
    this->GetToken();

    return new CallExprAST(IdName, Args);
}

/// numberexpr ::= number

ExprAST *TParser::ParseNumberExpr(void) {

    ExprAST *Result = new NumberExprAST(pToken->Value().real);
    this->GetToken(); // consume the number
    return Result;
}

/// parenexpr ::= '(' expression ')'

ExprAST *TParser::ParseParenExpr(void) {
    this->GetToken(); // eat (.
    ExprAST *V = ParseExpression();
    if (!V) return 0;

    if (token != tcRParen)
        return Error("expected ')'");
    this->GetToken(); // eat ).
    return V;
}

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr

ExprAST *TParser::ParsePrimary(void) {
    switch (token) {
        default: return Error("unknown token when expecting an expression");
        case tcIdentifier: return ParseIdentifierExpr();
        case tcNumber: return ParseNumberExpr();
        case tcLParen: return ParseParenExpr();
    }
}

/// binoprhs
///   ::= ('+' primary)*

ExprAST *TParser::ParseBinOpRHS(int ExprPrec, ExprAST *LHS) {
    // If this is a binop, find its precedence.
    while (1) {
        int TokPrec = GetTokPrecedence();

        // If this is a binop that binds at least as tightly as the current binop,
        // consume it, otherwise we are done.
        if (TokPrec < ExprPrec)
            return LHS;

        // Okay, we know this is a binop.
        int BinOp = token;
        this->GetToken(); // eat binop

        // Parse the primary expression after the binary operator.
        ExprAST *RHS = ParsePrimary();
        if (!RHS) return 0;

        // If BinOp binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int NextPrec = GetTokPrecedence();
        if (TokPrec < NextPrec) {
            RHS = ParseBinOpRHS(TokPrec + 1, RHS);
            if (RHS == 0) return 0;
        }

        // Merge LHS/RHS.
        LHS = new BinaryExprAST(BinOp, LHS, RHS);
    }
}

/// expression
///   ::= primary binoprhs
///

ExprAST *TParser::ParseExpression(void) {
    ExprAST *LHS = ParsePrimary();
    if (!LHS) return 0;

    return ParseBinOpRHS(0, LHS);
}

/// prototype
///   ::= id '(' id* ')'

PrototypeAST *TParser::ParsePrototype() {
    if (token != tcIdentifier)
        return ErrorP("Expected function name in prototype");

    string FnName = pToken->String();
    this->GetToken();

    if (token != tcLParen)
        return ErrorP("Expected '(' in prototype");

    this->GetToken();
    
    vector<string> ArgNames;
    while (token == tcIdentifier)
        ArgNames.push_back(pToken->String());
    if (token != tcRParen)
        return ErrorP("Expected ')' in prototype");

    // success.
        this->GetToken(); // eat ')'.

    return new PrototypeAST(FnName, ArgNames);
}

/// definition ::= 'def' prototype expression
FunctionAST *TParser::ParseDefinition() {
    this->GetToken(); // eat def.
    PrototypeAST *Proto = ParsePrototype();
    if (Proto == 0) return 0;

    if (ExprAST * E = ParseExpression())
        return new FunctionAST(Proto, E);
    return 0;
}

/// toplevelexpr ::= expression
FunctionAST *TParser::ParseTopLevelExpr() {
    if (ExprAST * E = ParseExpression()) {
        // Make an anonymous proto.
        PrototypeAST *Proto = new PrototypeAST("", vector<string>());
        return new FunctionAST(Proto, E);
    }
    return 0;
}

/// external ::= 'extern' prototype

PrototypeAST *TParser::ParseExtern() {
    this->GetToken(); // eat extern.
    return ParsePrototype();
}

//===----------------------------------------------------------------------===//
// Top-Level parsing
//===----------------------------------------------------------------------===//

void TParser::HandleDefinition() {
    if (ParseDefinition()) {
        fprintf(stderr, "Parsed a function definition.\n");
    } else {
        // Skip token for error recovery.
        this->GetToken();
    }
}

void TParser::HandleExtern() {
    if (ParseExtern()) {
        fprintf(stderr, "Parsed an extern\n");
    } else {
        // Skip token for error recovery.
        this->GetToken();
    }
}

void TParser::HandleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function.
    if (ParseTopLevelExpr()) {
        fprintf(stderr, "Parsed a top-level expr\n");
    } else {
        // Skip token for error recovery.
        this->GetToken();
    }
}


