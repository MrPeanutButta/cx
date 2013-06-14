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
    tcDummy,
    tcIdentifier, tcNumber, tcString, tcEndOfFile, tcError,

    //operators and punctuation
    //bitwise
    tcBlockCommentStart, // /*
    tcBlockCommentEnd, // */
    tcLineCommentStart, // //
    tcNewLine, // \n
    tcReturnLine, // \r
    tcBitXORorPow, // ^
    tcBitANDorAddrOf, // &
    tcBitOR, // |
    tcBitNOT, // ~
    tcBitNotEqual, // ~=
    tcBitXOREqual, // ^=
    tcBitANDEqual, // &=
    tcBitOREqual, // |=
    tcBitLeftShift, // <<
    tcBitRightShift, // >>

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

    tcLBracket, // }
    tcRBracket, // {
    tcColon, // :
    tcSemicolon, // ;

    tcComma, // ,
    tcDotDotDot, // ...

    // member access
    tcDot, // .
    tcColonColon, // ::
    tcPointerMember, // ->
    tcMemberPointer, // ->*

    //logic
    tcLogicOr, // ||
    tcLogicAnd, // &&
    tcLogicNOT, // !
    tcQuote,
    tcQuoteQuote,

    tcAND, tcARRAY, tcBEGIN, tcCASE, tcCONST, tcDIV,
    tcDO, tcDOWNTO, tcELSE, tcEND, tcFILE, tcFOR, tcFUNCTION,
    tcGOTO, tcIF, tcIN, tcLABEL, tcMOD, tcNIL, tcNOT, tcOF, tcOR,
    tcPACKED, tcPROCEDURE, tcPROGRAM, tcRECORD, tcREPEAT, tcSET,
    tcTHEN, tcTO, tcTYPE, tcUNTIL, tcVAR, tcWHILE, tcWITH,
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
    int integer;
    float real;
    char character;
    char *pString;
};

#endif

