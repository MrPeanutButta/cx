//fig 2-8
//  *************************************************************
//  *                                                           *
//  *   C O M M O N                                             *
//  *                                                           *
//  *   FILE:    prog2-1/common.cpp                             *
//  *                                                           *
//  *   MODULE:  Common                                         *
//  *                                                           *
//  *   Data and routines common to the front and back ends.    *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include "common.h"

int currentNestingLevel = 0;
int currentLineNumber = 0;
//endfig

//TSymtab globalSymtab;
int cntSymtabs = 0;
TSymtab *pSymtabList = nullptr;
TSymtab **vpSymtabs = nullptr;

//TIcode icode;

extern const TTokenCode tlDeclarationStart[] = {
    tcShort, tcBool, tcInt, tcSigned, tcDouble,
    tcLong, tcUnion, tcStatic, tcUnsigned,
    tcNameSpace, tcUsing, tcChar, tcEnum, tcVirtual,
    tcChar16_t, tcChar32_t, tcExport, tcStruct, tcVoid,
    tcClass, tcExtern, tcTemplate, tcConst, tcPrivate,
    tcFloat, tcProtected, tcThreadLocal, tcPublic, tcTypeDef,
    tcStringDef, tcPound, tcDummy
};

extern const TTokenCode tlDeclarationFollow[] = {
    tcSemicolon, tcIdentifier, tcDummy
};

extern const TTokenCode tlIdentifierStart[] = {
    tcIdentifier, tcDummy
};

extern const TTokenCode tlSublistFollow[] = {
    tcColon, tcDummy
};

extern const TTokenCode tlFieldDeclFollow[] = {
    tcSemicolon, tcIdentifier, tcRBracket, tcDummy
};

extern const TTokenCode tlEnumConstStart[] = {
    tcIdentifier, tcDummy
};

extern const TTokenCode tlEnumConstFollow[] = {
    tcRBracket, tcComma, tcSemicolon, tcDummy
};

extern const TTokenCode tlSubrangeLimitFollow[] = {
    tcDotDot, tcIdentifier, tcPlus, tcMinus, tcString,
    tcRightSubscript, tcComma, tcSemicolon, tcDummy
};

extern const TTokenCode tlIndexStart[] = {
    tcIdentifier, tcNumber, tcString, tcLParen, tcPlus, tcMinus,
    tcLeftSubscript, tcRightSubscript, tcDummy
};

extern const TTokenCode tlIndexFollow[] = {
    tcComma, tcRightSubscript, tcSemicolon, tcDummy
};

extern const TTokenCode tlIndexListFollow[] = {
    tcIdentifier, tcRightSubscript, tcLParen, tcPlus, tcMinus, tcNumber,
    tcString, tcOf, tcSemicolon,
    tcDummy
};

extern const TTokenCode tlSubscriptOrFieldStart[] = {
    tcColonColon,
    tcPointerMember, tcMemberPointer, tcDot, tcLeftSubscript, tcDummy
};

extern const TTokenCode tlIdentifierFollow[] = {
    tcComma, tcIdentifier, tcColon, tcColonColon, tcSemicolon,
    tcPointerMember, tcMemberPointer, tcDot, tcDummy
};

// tokens that can start a statement
extern const TTokenCode tlStatementStart[] = {
    tcSwitch, tcFor, tcDo, tcWhile, tcIdentifier,
    tcColonColon, tcReturn, tcContinue, tcIf,
    tcFriend, tcGoto, tcTry, tcDelete, tcShort, tcBool, tcInt,
    tcSigned, tcBreak, tcDouble, tcLong, tcUnion, tcStatic,
    tcUnsigned, tcCatch, tcNameSpace, tcUsing, tcChar,
    tcEnum, tcVirtual, tcChar16_t, tcChar32_t, tcExport,
    tcStruct, tcVoid, tcClass, tcExtern, tcTemplate, tcConst,
    tcPrivate, tcThis, tcFloat, tcProtected, tcThreadLocal,
    tcPublic, tcThrow, tcTypeDef, tcStringDef, tcPound, tcLBracket,
    tcDummy
};

// tokens that can follow a statement
extern const TTokenCode tlStatementFollow[] = {
    tcSemicolon, tcLBracket, tcRBracket, tcElse, tcWhile,
    tcDummy
};

extern const TTokenCode tlCaseLabelStart[] = {
    tcLBracket, tcCase, tcDummy
};

extern const TTokenCode tlExpressionStart[] = {
    tcPlus, tcMinus, tcIdentifier, tcNumber, tcString,
    tcBitNOT, tcLogicNOT, tcLParen, tcBitANDorAddrOf,
    tcLeftSubscript, tcDummy
};

extern const TTokenCode tlExpressionFollow[] = {
    tcComma, tcRParen, tcRightSubscript, tcPlusPlus,
    tcMinusMinus, tcColon, tcRBracket, tcDo, tcSemicolon,
    tcEqualEqual, tcNe, tcLt, tcGt, tcLe, tcGe,
    tcDummy
};

extern const TTokenCode tlRelOps[] = {
    tcEqualEqual, tcNe, tcLt, tcGt, tcLe, tcGe, tcDummy
};

extern const TTokenCode tlUnaryOps[] = {
    tcPlus, tcMinus, tcBitNOT, tcDummy
};

extern const TTokenCode tlAddOps[] = {
    tcPlus, tcMinus, tcLogicOr, tcBitLeftShift, tcBitRightShift,
    tcBitANDorAddrOf, tcBitXOR, tcBitOR, tcDummy
};

extern const TTokenCode tlMulOps[] = {
    tcStar, tcForwardSlash, tcMod, tcLogicAnd, tcDummy
};

extern const TTokenCode tlProgramEnd[] = {
    tcReturn, tcRBracket, tcEndOfFile, tcDummy
};

extern const TTokenCode tlEqualEqual[] = {tcEqualEqual, tcDummy};
extern const TTokenCode tlDo[] = {tcDo, tcDummy};
extern const TTokenCode tlLBracket [] = {tcLBracket, tcDummy};
extern const TTokenCode tlColonp[] = {tcColon, tcDummy};
extern const TTokenCode tlRBracket[] = {tcRBracket, tcDummy};
extern const TTokenCode tlSemicolon[] = {tcSemicolon, tcDummy};
extern const TTokenCode tlRParen[] = {tcRParen, tcDummy};
extern const TTokenCode tlLParen[] = {tcLParen, tcDummy};

bool TokenIn(TTokenCode tc, const TTokenCode *pList) {
    const TTokenCode *pCode;

    if (!pList) return false;

    for (pCode = pList; *pCode; ++pCode) {
        if (*pCode == tc) return true;
    }

    return false;
}