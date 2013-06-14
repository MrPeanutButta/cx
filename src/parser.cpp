//  *************************************************************
//  *                                                           *
//  *   P A R S E R                                             *
//  *                                                           *
//  *   Parse the source file for the Pascal Tokenizer          *
//  *   utility program.                                        *
//  *                                                           *
//  *   CLASSES: TParser                                        *
//  *                                                           *
//  *   FILE:    prog3-2/parser.cpp                             *
//  *                                                           *
//  *   MODULE:  Parser                                         *
//  *                                                           *
//  *   Routines to parse the source file.                      *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <cstdio>
#include "common.h"
#include "buffer.h"
#include "error.h"
#include "parser.h"
#include "complist.h"

//fig 3-22
//--------------------------------------------------------------
//  Parse       Parse the source file.  After listing each
//              source line, extract and list its tokens.
//--------------------------------------------------------------

void TParser::Parse(void) {

    bool currIsDelimiter(false); // true if current token is delimeter
    bool prevIsDelimiter(true); // likewise for previous token



    //--Loop to extract and print tokens
    //--until the end of the source file.
    do {
        GetToken();

        if (token != tcError) {
            currIsDelimiter = pToken->IsDelimiter();

            if (!prevIsDelimiter && !currIsDelimiter) {
                pCompact->Put(pToken->String());

                prevIsDelimiter = currIsDelimiter;
            }
        }

        if (token != tcError) pToken->Print();

    } while (token != tcEndOfFile);

    pCompact->PutLine();

    //--Print the parser's summary.
    list.PutLine();
    sprintf(list.text, "%20d source lines.", currentLineNumber);
    list.PutLine();
    sprintf(list.text, "%20d syntax errors.", errorCount);
    list.PutLine();
}
//endfig

