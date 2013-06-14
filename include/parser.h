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

#include "misc.h"
#include "buffer.h"
#include "token.h"
#include "scanner.h"
#include "complist.h"

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

public:

    TParser(TTextInBuffer *pBuffer)
    : pScanner(new TTextScanner(pBuffer)),
    pCompact(new TCompactListBuffer) {
    }

    ~TParser(void) {
        delete pScanner;
        delete pCompact;
    }

    void Parse(void);
};

#endif
//endfig
