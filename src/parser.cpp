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
//#include "complist.h"

//fig 3-22
//--------------------------------------------------------------
//  Parse       Parse the source file.  After listing each
//              source line, extract and list its tokens.
//--------------------------------------------------------------

void TParser::Parse(void) {

    TSymtabNode *pProgramId(file_name.c_str(), dcProgram);
    pProgramId->defn.routine.locals.pParmIds = nullptr;
    pProgramId->defn.routine.locals.pConstantIds = nullptr;
    pProgramId->defn.routine.locals.pTypeIds = nullptr;
    pProgramId->defn.routine.locals.pVariableIds = nullptr;
    pProgramId->defn.routine.pSymtab = nullptr;
    pProgramId->defn.routine.pIcode = nullptr;
    
    //bool currIsDelimiter(false); // true if current token is delimeter
    //bool prevIsDelimiter(true); // likewise for previous token

    //pIcode->Put(0);

    GetTokenAppend();
    //ParseStatement();
    ParseDeclarations(&pProgramId);

    Resync(tlProgramEnd);
    CondGetTokenAppend(tcEndOfFile, errMissingRightBracket);

    //--Loop to extract and print tokens
    //--until the end of the source file.
    //    do {
    //
    //        if (token == tcEndOfFile) {
    //            Error(errUnexpectedEndOfFile);
    //            break;
    //        }
    //
    //        ParseStatement();
    //
    //        if (token != tcSemicolon) {
    //            if (token == tcIdentifier) Error(errMissingSemicolon);
    //            else {
    //                Error(errUnexpectedToken);
    //
    //                while ((token != tcSemicolon) &&
    //                        (token != tcEndOfFile)) {
    //                    GetTokenAppend();
    //                }
    //            }
    //        }
    //
    //        while (token == tcSemicolon)GetTokenAppend();
    //
    //        //        if (token != tcError) {
    //        //            currIsDelimiter = pToken->IsDelimiter();
    //        //
    //        //            if (!prevIsDelimiter && !currIsDelimiter) {
    //        //                pCompact->Put(pToken->String());
    //        //
    //        //                prevIsDelimiter = currIsDelimiter;
    //        //            }
    //        //
    //        //            pToken->Print();
    //        //        }
    //        //
    //        //        switch (token) {
    //        //            case tcIdentifier:
    //        //            case tcNumber:
    //        //            case tcString:
    //        //            {
    //        //                TSymtabNode *pNode = globalSymtab.Search(pToken->String());
    //        //                if (pNode == nullptr) pNode = globalSymtab.Enter(pToken->String());
    //        //                break;
    //        //            }
    //        //            default:
    //        //            {
    //        //                pIcode->Put(token);
    //        //                break;
    //        //            }
    //        //        }
    //
    //    } while (token != tcEndOfFile);

    //int programSize = pIcode->Size();

    //    pIcode->Put(globalSymtab.NodeCount());
    //    pIcode->Put(globalSymtab.Root());
    //
    //    pIcode->GoTo(0);
    //    pIcode->Put(programSize);

    //    pCompact->PutLine();

    //--Print the parser's summary.

    list.PutLine();
    sprintf(list.text, "%20d source lines.", currentLineNumber);
    list.PutLine();
    sprintf(list.text, "%20d syntax errors.", errorCount);
    list.PutLine();

}
//endfig

void TParser::Resync(const TTokenCode* pList1,
        const TTokenCode* pList2,
        const TTokenCode* pList3) {

    bool errorFlag = (!TokenIn(token, pList1)) &&
            (!TokenIn(token, pList2)) &&
            (!TokenIn(token, pList3));

    if (errorFlag) {
        TErrorCode errorCode = token == tcEndOfFile
                ? errUnexpectedEndOfFile
                : errUnexpectedToken;

        Error(errorCode);

        while ((!TokenIn(token, pList1)) &&
                (!TokenIn(token, pList2)) &&
                (!TokenIn(token, pList3)) &&
                (token != tcReturn) &&
                (token != tcEndOfFile)) {
            GetToken();
        }

        if ((token == tcEndOfFile) &&
                (errorCode != errUnexpectedEndOfFile)) {
            Error(errUnexpectedEndOfFile);
        }
    }
}
