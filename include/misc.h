//  *************************************************************
//  *                                                           *
//  *   M I S C E L L A N E O U S   (Header)			*
//  *                                                           *
//  *   FILE:    prog3-2/misc.h					*
//  *                                                           *
//  *   MODULE:  Common                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef misc_h
#define misc_h


#include <map>
#include <string>

//--------------------------------------------------------------
//  TCharCode           Character codes.
//--------------------------------------------------------------

enum TCharCode {
    ccLetter, ccDigit, ccSpecial, ccQuote, ccQuoteQuote, ccWhiteSpace,
    ccEndOfFile, ccError,
};

//fig 3-14
//--------------------------------------------------------------
//  TTokenCode          Token codes.
//--------------------------------------------------------------

enum TTokenCode {
    tcDummy = 0,
    tcIdentifier, tcNumber, tcString, tcEndOfFile, tcError,

    //operators and punctuation
    //bitwise
    tcBitXOR, // ^
    tcBitANDorAddrOf, // &
    tcBitOR, // |
    tcBitNOT, // ~
    tcBitXOREqual, // ^=
    tcBitANDEqual, // &=
    tcBitOREqual, // |=
    tcBitLeftShift, // <<
    tcBitLeftShiftEqual, // <<=
    tcBitRightShift, // >>
    tcBitRightShiftEqual, // >>=

    tcMinus, // -
    tcMinusEqual, // -=
    tcPlus, // +
    tcPlusEqual, // +=
    tcEqual, // =
    tcMinusMinus, // --
    tcPlusPlus, // ++
    tcForwardSlash, // /
    tcForwardSlashEqual, // /=
    tcStar, // *
    tcStarEqual, // *=
    tcLt, // <
    tcGt, // >
    tcEqualEqual, // ==
    tcLe, // <=
    tcGe, // >=
    tcNe, // !=
    tcMod, // %
    tcModEqual, // %=

    //punctuation
    tcLeftSubscript, // [
    tcRightSubscript, // ]
    tcQuestionMark, // ?
    tcPound, // #
    tcDotStar, // .*
    tcLParen, // (
    tcRParen, // )

    tcLBracket, // {
    tcRBracket, // }
    tcColon, // :
    tcSemicolon, // ;

    tcComma, // ,
    tcDotDotDot, // ...

    // member access
    tcDot, // .
    tcDotDot, // ..
    tcColonColon, // ::
    tcPointerMember, // ->
    tcMemberPointer, // ->*

    //logic
    tcLogicOr, // ||
    tcLogicAnd, // &&
    tcLogicNOT, // !
    tcQuote, // '
    tcQuoteQuote, // "

    tcIf, tcReturn, tcContinue, tcFriend, tcTrue, tcGoto, tcTry, tcDelete,
    tcShort, tcTypeId, tcBool, tcDo, tcInt, tcSigned, tcTypeName,
    tcBreak, tcDouble, tcLong, tcSizeOf, tcUnion, tcCase, tcStatic,
    tcUnsigned, tcCatch, tcElse, tcNameSpace, tcUsing, tcChar, tcCharDef,
    tcEnum, tcNew, tcVirtual, tcChar16_t, tcExplicit, tcNoExcept,
    tcChar32_t, tcExport, tcNullptr, tcSwitch, tcStruct, tcVoid,
    tcClass, tcExtern, tcOperator, tcTemplate, tcConst, tcFalse,
    tcPrivate, tcThis, tcWhile, tcFloat, tcProtected, tcThreadLocal,
    tcFor, tcPublic, tcThrow, tcDefault, tcTypeDef, tcMutable, tcInclude,
    tcStringDef
};
//endfig

//--------------------------------------------------------------
//  TDataType           Data type.
//--------------------------------------------------------------

enum TDataType {
    tyDummy, tyInteger, tyReal, tyCharacter, tyString,
};

//--------------------------------------------------------------
//  TDataValue          Data value.
//--------------------------------------------------------------

union TDataValue {
    int __int;
    unsigned int __u_int;
    float __float;
    double __double;
    char __char;
    unsigned char __u_char;

    bool __bool;

    //needed???
    char *pString;
};

typedef std::map<char, TCharCode> char_code_map;
typedef std::map<std::string, TTokenCode> token_map;

#endif

