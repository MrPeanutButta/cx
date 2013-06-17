//fig 3-10
//  *************************************************************
//  *                                                           *
//  *   P A R S E R   (Header)                                  *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog3-1/parser.h                               *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef parser_h
#define parser_h

#include <map>
#include "misc.h"
#include "buffer.h"
#include "token.h"
#include "scanner.h"
#include "complist.h"
#include "ast.h"

using namespace std;

//--------------------------------------------------------------
//  TParser     Parser class.
//--------------------------------------------------------------

class TParser {
    TTextScanner * const pScanner; // ptr to the scanner
    TToken *pToken; // ptr to the current token
    TTokenCode token; // code of current token

    TCompactListBuffer * const pCompact; // compact list buffer

    void GetToken(void) {
        pToken = pScanner->Get();
        token = pToken->Code();
    }

    //
    int GetTokPrecedence(void);
    ExprAST *ParseExpression(void);
    ExprAST *ParseIdentifierExpr(void);
    ExprAST *ParseNumberExpr(void);
    ExprAST *ParseParenExpr(void);
    ExprAST *ParsePrimary(void);
    ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS);
    PrototypeAST *ParsePrototype(void);
    FunctionAST *ParseDefinition(void);
    FunctionAST *ParseTopLevelExpr(void);
    PrototypeAST *ParseExtern(void);
    void HandleDefinition(void);
    void HandleExtern(void);
    void HandleTopLevelExpression(void);

    /// BinopPrecedence - This holds the precedence for each binary operator that is
    /// defined.
    map<char, int> BinopPrecedence;

public:

    TParser(TTextInBuffer *pBuffer)
    : pScanner(new TTextScanner(pBuffer)),
    pCompact(new TCompactListBuffer) {

        BinopPrecedence['<'] = 10;
        BinopPrecedence['+'] = 20;
        BinopPrecedence['-'] = 20;
        BinopPrecedence['*'] = 40; // highest.

    }

    ~TParser(void) {
        delete pScanner;
        delete pCompact;
    }

    void Parse(void);
};

#endif
//endfig
