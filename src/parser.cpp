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

TSymtabNode *TParser::Parse(void) {

    TSymtabNode *pProgramId = new TSymtabNode("__global__", dcProgram);
    pProgramId->defn.routine.which = rcDeclared;
    pProgramId->defn.routine.parmCount = 0;
    pProgramId->defn.routine.totalParmSize = 0;
    pProgramId->defn.routine.totalLocalSize = 0;
    pProgramId->defn.routine.locals.pParmsIds = NULL;
    pProgramId->defn.routine.locals.pConstantIds = NULL;
    pProgramId->defn.routine.locals.pTypeIds = NULL;
    pProgramId->defn.routine.locals.pVariableIds = NULL;
    pProgramId->defn.routine.locals.pRoutineIds = NULL;
    pProgramId->defn.routine.pSymtab = NULL;
    pProgramId->defn.routine.pIcode = NULL;
    SetType(pProgramId->pType, pIntegerType);

    icode.Reset();

    currentNestingLevel = 0;
    //--Enter the nesting level 1 and open a new scope for the program.
    //symtabStack.SetCurrentSymtab(&globalSymtab);
    symtabStack.EnterScope();

    GetTokenAppend();

    ParseStatementList(pProgramId, tcEndOfFile);
    pProgramId->defn.routine.returnMarker = PutLocationMarker();

    GetTokenAppend();

    pProgramId->defn.routine.pSymtab = symtabStack.ExitScope();

    Resync(tlProgramEnd);
    CondGetTokenAppend(tcEndOfFile, errMissingRightBracket);

    //--Set the program's icode.
    pProgramId->defn.routine.pIcode = new TIcode(icode);

    list.PutLine();
    sprintf(list.text, "%20d source lines.", currentLineNumber);
    list.PutLine();
    sprintf(list.text, "%20d syntax errors.", errorCount);
    list.PutLine();

    return pProgramId;

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
            GetTokenAppend();
        }

        if ((token == tcEndOfFile) &&
                (errorCode != errUnexpectedEndOfFile)) {
            Error(errUnexpectedEndOfFile);
        }
    }
}
