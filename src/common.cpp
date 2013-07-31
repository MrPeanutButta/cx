/**
 * common.cpp
 *
 * Data and routines common to the front and back ends.
 *
 * NOTE:
 *      token lists below are subject to change drastically
 *      once the parser and scanner are complete. -aaron
 */


#include "common.h"

// current scope level
int currentNestingLevel = 0;
int currentLineNumber = 0;

/** Global symbols
 *
 * NOTE:
 *      when writing the Cx stdlib, this global symtab should be visible
 *      to the dynamic library to allow a library to add it's own symbols. - aaron
 */
TSymtab globalSymtab;

// number of symtabs
int cntSymtabs = 0;

TSymtab *pSymtabList = nullptr;
TSymtab **vpSymtabs = nullptr;

/// Tokens for resyncing the parser

// tokens that start a declaration
extern const TTokenCode tlDeclarationStart[] = {
    tcShort, tcBool, tcInt, tcSigned, tcDouble,
    tcLong, tcUnion, tcStatic, tcUnsigned,
    tcNameSpace, tcUsing, tcChar, tcEnum, tcVirtual,
    tcChar16_t, tcChar32_t, tcExport, tcStruct, tcVoid,
    tcClass, tcExtern, tcTemplate, tcConst, tcPrivate,
    tcFloat, tcProtected, tcThreadLocal, tcPublic, tcTypeDef,
    tcStringDef, tcPound, tcDummy
};

// tokens that follow a declaration
extern const TTokenCode tlDeclarationFollow[] = {
    tcSemicolon, tcIdentifier, tcDummy
};

// identifier start
extern const TTokenCode tlIdentifierStart[] = {
    tcIdentifier, tcDummy
};

// tokens that follow a sublist
extern const TTokenCode tlSublistFollow[] = {
    tcColon, tcDummy
};

// tokens that follow a member/field declaration
extern const TTokenCode tlFieldDeclFollow[] = {
    tcSemicolon, tcIdentifier, tcRBracket, tcDummy
};

// tokens that start an enum
// XXX broken
extern const TTokenCode tlEnumConstStart[] = {
    tcIdentifier, tcDummy
};

// tokens that follow an enum
// XXX not implemented
extern const TTokenCode tlEnumConstFollow[] = {
    tcRBracket, tcComma, tcSemicolon, tcDummy
};

// tokens that follow subrange
// XXX deprecated from the old Pascal Interp
extern const TTokenCode tlSubrangeLimitFollow[] = {
    tcDotDot, tcIdentifier, tcPlus, tcMinus, tcString,
    tcRightSubscript, tcComma, tcSemicolon, tcDummy
};

// tokens that start an index
// XXX broken
extern const TTokenCode tlIndexStart[] = {
    tcIdentifier, tcNumber, tcString, tcLParen, tcPlus, tcMinus,
    tcLeftSubscript, tcRightSubscript, tcDummy
};

// tokens that follow an index
extern const TTokenCode tlIndexFollow[] = {
    tcComma, tcRightSubscript, tcSemicolon, tcDummy
};

// XXX idk if this is used
extern const TTokenCode tlIndexListFollow[] = {
    tcIdentifier, tcRightSubscript, tcLParen, tcPlus, tcMinus, tcNumber,
    tcString, tcSemicolon,
    tcDummy
};

// XXX deprecated
extern const TTokenCode tlSubscriptOrFieldStart[] = {
    tcColonColon,
    tcPointerMember, tcMemberPointer, tcDot, tcLeftSubscript, tcDummy
};

// tokens that follow an identifier
extern const TTokenCode tlIdentifierFollow[] = {
    tcComma, tcIdentifier, tcColon, tcColonColon, tcSemicolon,
    tcPointerMember, tcRParen, tcMemberPointer, tcDot, tcDummy
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

extern const TTokenCode tlAssignOps[] = {
    tcBitXOREqual, // ^=
    tcBitANDEqual, // &=
    tcBitOREqual, // |=
    tcBitLeftShiftEqual, // <<=
    tcBitRightShiftEqual, // >>=

    tcMinusEqual, // -=
    tcPlusEqual, // +=
    tcMinusMinus, // --
    tcPlusPlus, // ++
    tcForwardSlashEqual, // /=
    tcStarEqual, // *=
    tcModEqual, // %=
    tcEqual, // =
    tcReturn, // return (expr)
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