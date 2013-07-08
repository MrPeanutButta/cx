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
    TSymtabNode *pFuncId = pFunctionNode; // ptr to function id node
    int forwardFlag = false; // true if forwarded, false if not

    //--Enter the next nesting level and open a new scope
    //--for the function.
    if (!strcmp(pFunctionNode->String(), "main")) {
        GetToken();
        symtabStack.SetScope(1);
    } else symtabStack.EnterScope();

    //-- (
    CondGetTokenAppend(tcLParen, errMissingLeftParen);


    int parmCount; // count of formal parms
    int totalParmSize; // total byte size of all parms

    TSymtabNode *pParmList = ParseFormalParmList(parmCount,
            totalParmSize);

    //-- )
    CondGetTokenAppend(tcRParen, errMissingRightParen);

    //--Not forwarded.
    pFuncId->defn.routine.parmCount = parmCount;
    pFuncId->defn.routine.totalParmSize = totalParmSize;
    pFuncId->defn.routine.locals.pParmsIds = pParmList;

    pFuncId->level = currentNestingLevel;

    pFuncId->defn.routine.locals.pConstantIds = NULL;
    pFuncId->defn.routine.locals.pTypeIds = NULL;
    pFuncId->defn.routine.locals.pVariableIds = NULL;
    pFuncId->defn.routine.locals.pRoutineIds = NULL;

    if (token == tcSemicolon) {
        pFuncId->defn.routine.which = rcForward;
    } else if (token == tcLBracket) {
        pFuncId->defn.routine.which = rcDeclared;
        ParseBlock(pFuncId);
    }

    pFuncId->defn.routine.pSymtab = symtabStack.ExitScope();
    pFuncId->defn.how = ::dcFunction;

    return pFuncId;
}

//--------------------------------------------------------------
//  ParseBlock      Parse a routine's block:
//
//                      <declarations> <compound-statement>
//
//      pRoutineId : ptr to symbol table node of routine's id
//--------------------------------------------------------------

void TParser::ParseBlock(TSymtabNode *pRoutineId) {
    //--<declarations>
    //ParseDeclarations(pRoutineId);

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
