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

#include <string>
#include "misc.h"
#include "buffer.h"
#include "error.h"
#include "token.h"
#include "scanner.h"
#include "complist.h"
#include "icode.h"
#include "exec.h"
#include "symtable.h"
#include "types.h"

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

    const char *file_name;
    //TRuntimeStack runStack;
    //TCompactListBuffer * const pCompact; // compact list buffer

    // declarations
    void ParseDeclarations(TSymtabNode *pRoutineId);
    void ParseConstantDeclarations(TSymtabNode *pRoutineId);
    void ParseConstant(TSymtabNode *pConstId);
    void ParseIdentifierConstant(TSymtabNode *pId1, TTokenCode sign);

    void ParseDefinitions(TSymtabNode *pRoutineId);
    void ParseIntegerDeclaration(TSymtabNode *pRoutineId);
    void ParseTypeDefinitions(TSymtabNode *pRoutineId);
    TType *ParseTypeSpec(void);

    TType *ParseIdentifierType(const TSymtabNode *pId2);

    TType *ParseEnumHeader(TSymtabNode *pRoutineId);
    TType *ParseEnumerationType(void);

    TType *ParseSubrangeType(TSymtabNode *pMinId);
    TType *ParseSubrangeLimit(TSymtabNode *pLimitId, int &limit);

    TType*ParseArrayType(void);
    void ParseIndexType(TType *pArrayType);
    int ArraySize(TType *pArrayType);
    TType *ParseRecordType(void);

    void ParseVariableDeclarations(TSymtabNode *pRoutineId);
    void ParseFieldDeclarations(TType *pRecordType, int offset);
    void ParseVarOrFieldDecls(TSymtabNode *pRoutineId,
            TType *pRecordType,
            int offset);
    TSymtabNode *ParseIdSublist(const TSymtabNode *pRoutineId,
            const TType *pRecordType, TSymtabNode *&pLastId);

    // expressions
    TType *ParseExpression(void);
    TType *ParseSuffix(TSymtabNode *pNode);
    //void ParseSizeOf(void);
    TType *ParseSimpleExpression(void);
    TType *ParseTerm(void);
    TType *ParseFactor(void);
    TType *ParseVariable(const TSymtabNode *pId);
    TType *ParseSubscripts(const TType *pType);
    TType *ParseField(const TType *pType);

    // statements
    void ParseStatement(TSymtabNode* pRoutineId);
    void ParseAssignment(TSymtabNode* pRoutineId);
    void ParseStatementList(TSymtabNode* pRoutineId, TTokenCode terminator);
    void ParseDO(TSymtabNode* pRoutineId);
    void ParseWHILE(TSymtabNode* pRoutineId);
    void ParseIF(TSymtabNode* pRoutineId);
    void ParseFOR(TSymtabNode* pRoutineId);
    void ParseSWITCH(TSymtabNode* pRoutineId);
    void ParseCaseBranch(TSymtabNode* pRoutineId, const TType *pExprType);
    void ParseCaseLabel(TSymtabNode* pRoutineId, const TType *pExprType);
    void ParseCompound(TSymtabNode* pRoutineId);
    void ParseRETURN(TSymtabNode* pRoutineId);

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

    TSymtabNode *Find(const char *pString) const {
        TSymtabNode *pNode = SearchAll(pString);

        if (!pNode) {
            Error(errUndefinedIdentifier);
            pNode = globalSymtab.Enter(pString);
        }

        return pNode;
    }

    void CopyQuotedString(char *pString, const char *pQuotedString) const {
        int length = strlen(pQuotedString) - 2;
        strncpy(pString, &pQuotedString[1], length);
        pString[length] = '\0';
    }

    TSymtabNode *EnterLocal(const char *pString,
            TDefnCode dc = dcUndefined) const {
        return globalSymtab.Enter(pString, dc);
    }

    TSymtabNode *EnterNewLocal(const char *pString,
            TDefnCode dc = dcUndefined) const {
        return globalSymtab.EnterNew(pString, dc);
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

        file_name = pBuffer->FileName();

        EnterLocal("input");
        EnterLocal("output");

        InitializePredefinedTypes(&globalSymtab);

    }

    ~TParser(void) {
        delete pScanner;
        RemovePredefinedTypes();
    }

    void Parse(void);
};

#endif
//endfig
