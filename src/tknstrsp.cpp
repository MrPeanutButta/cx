//fig 3-19
//  *************************************************************
//  *                                                           *
//  *   T O K E N S   (Strings and Specials)                    *
//  *                                                           *
//  *   Routines to extract string and special symbol tokens    *
//  *   from the source file.                                   *
//  *                                                           *
//  *   CLASSES: TStringToken, TSpecialToken, TErrorToken       *
//  *                                                           *
//  *   FILE:    prog3-2/tknstrsp.cpp                           *
//  *                                                           *
//  *   MODULE:  Scanner                                        *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <cstdio>
#include "token.h"

//              *******************
//              *                 *
//              *  String Tokens  *
//              *                 *
//              *******************

//--------------------------------------------------------------
//  Get     Get a string token from the source.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TStringToken::Get(TTextInBuffer &buffer) {
    char ch; // current character
    char *ps = string; // ptr to char in string

    *ps++ = '\"'; // opening quote

    //--Get the string.
    ch = buffer.GetChar(); // first char after opening quote
    while (ch != eofChar) {
        if (ch == '\"') { // look for another quote

            //--Fetched a quote.  Now check for an adjacent quote,
            //--since two consecutive quotes represent a single
            //--quote in the string.
            ch = buffer.GetChar();
            if (ch != '\"') break; // not another quote, so previous
            //   quote ended the string
        }//--Replace the end of line character with a blank.
        else if (ch == '\0') ch = ' ';

        //--Append current char to string, then get the next char.
        *ps++ = ch;
        ch = buffer.GetChar();
    }

    if (ch == eofChar) Error(errUnexpectedEndOfFile);

    *ps++ = '\"'; // closing quote
    *ps = '\0';
}

void TCharToken::Get(TTextInBuffer &buffer) {
    char ch; // current character
    char *ps = string; // ptr to char in string

    *ps++ = '\''; // opening quote
    //--Get the string.
    ch = buffer.GetChar(); // first char after opening quote
    *ps++ = ch;
    //--Append current char to string, then get the next char.
    ch = buffer.GetChar();

    if (ch != '\'') Error(errMissingSingleQuote);
    if (ch == eofChar) Error(errUnexpectedEndOfFile);

    ch = buffer.GetChar();
    *ps++ = '\''; // closing quote
    *ps = '\0';
}

void TCharToken::Print(void) const {
    sprintf(list.text, "\t%-18s %-s", ">> char:", string);
    list.PutLine();
}

//--------------------------------------------------------------
//  Print       Print the token to the list file.
//--------------------------------------------------------------

void TStringToken::Print(void) const {
    sprintf(list.text, "\t%-18s %-s", ">> string:", string);
    list.PutLine();
}

//              ********************
//              *                  *
//              *  Special Tokens  *
//              *                  *
//              ********************

//--------------------------------------------------------------
//  Get         Extract a one- or two-character special symbol
//              token from the source.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TSpecialToken::Get(TTextInBuffer &buffer) {
    char ch = buffer.Char();
    char *ps = string;

    *ps++ = ch;

    switch (ch) {
        case '^': ch = buffer.GetChar();
            if (ch == '=') {
                *ps++ = '=';
                code = tcBitXOREqual;
                buffer.GetChar();
            } else code = tcBitXOR;
            break;
        case '&': ch = buffer.GetChar();
            if (ch == '=') {
                *ps++ = '=';
                code = tcBitANDEqual;
                buffer.GetChar();
            } else if (ch == '&') {
                *ps++ = '&';
                code = tcLogicAnd;
                buffer.GetChar();
            } else code = tcBitANDorAddrOf;
            break;
        case '|': ch = buffer.GetChar();
            if (ch == '=') {
                *ps++ = '=';
                code = tcBitOREqual;
                buffer.GetChar();
            } else if (ch == '|') {
                *ps++ = '|';
                code = tcLogicOr;
                buffer.GetChar();
            } else code = tcBitOR;
            break;
        case '~': ch = buffer.GetChar();
//            if (ch == '=') {
//                *ps++ = '=';
//                code = tcBitNotEqual;
//                buffer.GetChar();
//            } else code = tcBitNOT;
            break;
        case '*': ch = buffer.GetChar();
            if (ch == '=') {
                *ps++ = '=';
                code = tcStarEqual;
                buffer.GetChar();
            } else code = tcStar;
            break;
        case '(': code = tcLParen;
            buffer.GetChar();
            break;
        case ')': code = tcRParen;
            buffer.GetChar();
            break;
        case '-': ch = buffer.GetChar();
            if (ch == '=') {
                *ps++ = '=';
                code = tcMinusEqual;
                buffer.GetChar();
            } else if (ch == '-') {
                *ps++ = '-';
                code = tcMinusMinus;
                buffer.GetChar();
            } else if (ch == '>') {
                *ps++ = '>';
                code = tcPointerMember;
                buffer.GetChar();
            } else code = tcMinus;
            break;
        case '+': ch = buffer.GetChar();
            if (ch == '=') {
                *ps++ = '=';
                code = tcPlusEqual;
                buffer.GetChar();
            } else if (ch == '+') {
                *ps++ = '+';
                code = tcPlusPlus;
                buffer.GetChar();
            } else code = tcPlus;
            break;
        case '=': ch = buffer.GetChar();
            if (ch == '=') {
                *ps++ = '=';
                code = tcEqualEqual;
                buffer.GetChar();
            } else code = tcEqual;
            break;
        case '[': code = tcLeftSubscript;
            buffer.GetChar();
            break;
        case ']': code = tcRightSubscript;
            buffer.GetChar();
            break;
        case '{': code = tcLBracket;
            buffer.GetChar();
            break;
        case '}': code = tcRBracket;
            buffer.GetChar();
            break;
        case ';': code = tcSemicolon;
            buffer.GetChar();
            break;
        case ',': code = tcComma;
            buffer.GetChar();
            break;
        case '/': ch = buffer.GetChar(); // /, /=
            if (ch == '=') {
                *ps++ = '=';
                code = tcForwardSlashEqual;
                buffer.GetChar();
            } else code = tcForwardSlash;
            break;
        case ':': ch = buffer.GetChar(); // : or ::
            if (ch == ':') {
                *ps++ = ':';
                code = tcColonColon;
                buffer.GetChar();
            } else code = tcColon;
            break;
        case '<': ch = buffer.GetChar(); // < or <=
            if (ch == '=') {
                *ps++ = '=';
                code = tcLe;
                buffer.GetChar();
            } else if (ch == '<') {
                *ps++ = '<';
                ch = buffer.GetChar();
                if (ch == '=') {
                    *ps++ = '=';
                    code = tcBitLeftShiftEqual;
                    buffer.GetChar();
                } else code = tcBitLeftShift;
            } else code = tcLt;
            break;
        case '>': ch = buffer.GetChar(); // > or >=
            if (ch == '=') {
                *ps++ = '=';
                code = tcGe;
                buffer.GetChar();
            } else if (ch == '>') {
                *ps++ = '>';
                ch = buffer.GetChar();
                if (ch == '=') {
                    *ps++ = '=';
                    code = tcBitRightShiftEqual;
                    buffer.GetChar();
                } else code = tcBitRightShift;
            } else code = tcGt;
            break;
        case '!': ch = buffer.GetChar(); // ! or !=
            if (ch == '=') {
                *ps++ = '=';
                code = tcNe;
                buffer.GetChar();
            } else code = tcLogicNOT;
            break;
        case '%': ch = buffer.GetChar(); // ! or !=
            if (ch == '=') {
                *ps++ = '=';
                code = tcModEqual;
                buffer.GetChar();
            } else code = tcMod;
            break;
        case '.': ch = buffer.GetChar();
            if (ch == '.') {
                *ps++ = '.';
                code = tcDotDot;
                buffer.GetChar();
            } else code = tcDot;
            break;
        case '?': code = tcQuestionMark;
            buffer.GetChar();
            break;
        case '#': code = tcPound;
            buffer.GetChar();
            break;
        case '\"': code = tcQuoteQuote;
            buffer.GetChar();
        default: code = tcError; // error
            buffer.GetChar();
            Error(errUnrecognizable);
            break;
    }

    *ps = '\0';
}

//--------------------------------------------------------------
//  Print       Print the token to the list file.
//--------------------------------------------------------------

void TSpecialToken::Print(void) const {
    sprintf(list.text, "\t%-18s %-s", ">> special:", string);
    list.PutLine();
}

//              *****************
//              *               *
//              *  Error Token  *
//              *               *
//              *****************

//--------------------------------------------------------------
//  Get         Extract an invalid character from the source.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TErrorToken::Get(TTextInBuffer &buffer) {
    string[0] = buffer.Char();
    string[1] = '\0';

    buffer.GetChar();
    Error(errUnrecognizable);
}
//endfig
