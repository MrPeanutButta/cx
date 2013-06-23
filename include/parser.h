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
#include "error.h"
#include "token.h"
#include "scanner.h"
#include "complist.h"
#include "icode.h"
#include "exec.h"
#include "symtable.h"

using namespace std;

extern TIcode icode;
extern TSymtab globalSymtab;

//--------------------------------------------------------------
//  TParser     Parser class.
//--------------------------------------------------------------

class TParser {
    TTextScanner * const pScanner; // ptr to the scanner
    TToken *pToken; // ptr to the current token
    TTokenCode token; // code of current token
    //TRuntimeStack runStack;
    //TCompactListBuffer * const pCompact; // compact list buffer

    // statements
    void ParseStatement(void);
    void ParseAssignment(void);

    // expressions
    void ParseExpression(void);
    void ParseSuffix(TSymtabNode *pNode);
    void ParseSizeOf(void);
    void ParseSimpleExpression(void);
    void ParseTerm(void);
    void ParseFactor(void);

    void ParseStatementList(TTokenCode terminator);
    void ParseDO(void);
    void ParseWHILE(void);
    void ParseIF(void);
    void ParseFOR(void);
    void ParseSWITCH(void);
    void ParseCaseBranch(void);
    void ParseCaseLabel(void);
    void ParseCompound(void);
    void ParseRETURN(void);

    void GetToken(void) {
        pToken = pScanner->Get();
        token = pToken->Code();
    }

    void GetTokenAppend(void) {
        GetToken();
        icode.Put(token);
    }

    void InsertLineMarker(void) {
        icode.InsertLineMarker();
    }

    TSymtabNode *SearchAll(const char *pString) const {
        return globalSymtab.Search(pString);
    }

    TSymtabNode *EnterLocal(const char *pString) const {
        return globalSymtab.Enter(pString);
    }

    void CondGetToken(TTokenCode tc, TErrorCode ec) {
        if (tc == token)GetToken();
        else Error(ec);
    }

    void CondGetTokenAppend(TTokenCode tc, TErrorCode ec) {
        if (tc == token) GetTokenAppend();
        else Error(ec);
    }

    void Resync(const TTokenCode *pList1,
            const TTokenCode *pList2 = nullptr,
            const TTokenCode *pList3 = nullptr);

public:

    TParser(TTextInBuffer *pBuffer)
    : pScanner(new TTextScanner(pBuffer)) {
        EnterLocal("input");
        EnterLocal("output");
    }

    ~TParser(void) {
        delete pScanner;
    }

    void Parse(void);
};

#endif
//endfig
