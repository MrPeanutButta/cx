//--------------------------------------------------------------
//  ParseFunctionHeader     Parse a function header:
//
//                              FUNCTION <id>
//
//                          or:
//
//                              FUNCTION <id> : <type-id>
//
//                          or:
//
//                              FUNCTION <id> ( <parm-list> )
//                                            : <type-id>
//
//  Return: ptr to function id's symbol table node
//--------------------------------------------------------------

#include <cstring>
#include "common.h"
#include "parser.h"

TSymtabNode *TParser::ParseFunctionHeader(TSymtabNode *pFunctionNode) {

    //--Enter the next nesting level and open a new scope
    //--for the function.
    //if (__MAIN_ENTRY__) {
    //symtabStack.SetScope(__MAIN_SCOPE__);
    //pFunctionNode->defn.routine.pSymtab = symtabStack.GetCurrentSymtab();
    //} else {
        symtabStack.EnterScope();
    //}

    //-- (
    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    int parmCount; // count of formal parms
    int totalParmSize; // total byte size of all parms

    TSymtabNode *pParmList = ParseFormalParmList(parmCount,
            totalParmSize);

    pFunctionNode->defn.routine.parmCount = parmCount;
    pFunctionNode->defn.routine.totalParmSize = totalParmSize;
    pFunctionNode->defn.routine.locals.pParmsIds = pParmList;

    //--Not forwarded.
    pFunctionNode->defn.routine.locals.pConstantIds = NULL;
    pFunctionNode->defn.routine.locals.pTypeIds = NULL;
    pFunctionNode->defn.routine.locals.pVariableIds = NULL;
    pFunctionNode->defn.routine.locals.pRoutineIds = NULL;
    pFunctionNode->defn.how = ::dcFunction;

    //-- )
    CondGetTokenAppend(tcRParen, errMissingRightParen);

    if (token == tcSemicolon) {
        pFunctionNode->defn.routine.which = rcForward;
    } else if (token == tcLBracket) {
        pFunctionNode->defn.routine.which = rcDeclared;
        ParseBlock(pFunctionNode);
        pFunctionNode->defn.routine.returnMarker = PutLocationMarker();
    }

    //if (__MAIN_ENTRY__) {
    // currentNestingLevel--;
    // } else {
        pFunctionNode->defn.routine.pSymtab = symtabStack.ExitScope();
    // }

    return pFunctionNode;
}

//--------------------------------------------------------------
//  ParseBlock      Parse a routine's block:
//
//                      <declarations> <compound-statement>
//
//      pRoutineId : ptr to symbol table node of routine's id
//--------------------------------------------------------------

void TParser::ParseBlock(TSymtabNode *pRoutineId) {
    //--<compound-statement> : Reset the icode and append BEGIN to it,
    //--                       and then parse the compound statement.
    Resync(tlStatementStart);
    if (token != tcLBracket) Error(errMissingLeftBracket);
    icode.Reset();

    ParseCompound(pRoutineId);

    //--Set the program's or routine's icode.
    pRoutineId->defn.routine.pIcode = new TIcode(icode);
}
//endfig
