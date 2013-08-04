#include <cstring>
#include "common.h"
#include "parser.h"

extern TSymtabNode *pProgram_ptr;

/** ParseFunctionHeader         Parse a function header:
 *
 *                              <type-id> <id> (<parm-list>);
 * 
 *                          or:
 *
 *                              <type-id> <id> (<parm-list>){}
 * 
 * NOTE:
 *      If scope == 0 and pProgram_ptr->foundGlobalEnd == false;
 *      Set main's location in icode only when function body is found.
 * 
 * @param pFunctionNode : ptr to the function id's symbol table node.
 * @return ptr to function id's symbol table node.
 */
TSymtabNode *TParser::ParseFunctionHeader(TSymtabNode *pFunctionNode) {
    //--Enter the next nesting level and open a new scope
    //--for the function.
    symtabStack.EnterScope();

    //-- (
    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    int parmCount; // count of formal parms
    int totalParmSize; // total byte size of all parms

    TSymtabNode *pParmList = ParseFormalParmList(parmCount,
            totalParmSize);

    pFunctionNode->defn.routine.parmCount = parmCount;
    pFunctionNode->defn.routine.totalParmSize = totalParmSize;
    pFunctionNode->defn.routine.locals.pParmsIds = pParmList;
    pFunctionNode->defn.how = ::dcFunction;

    //--Not forwarded.
    pFunctionNode->defn.routine.locals.pConstantIds = nullptr;
    pFunctionNode->defn.routine.locals.pTypeIds = nullptr;
    pFunctionNode->defn.routine.locals.pVariableIds = nullptr;
    pFunctionNode->defn.routine.locals.pRoutineIds = nullptr;

    //-- )
    CondGetTokenAppend(tcRParen, errMissingRightParen);

    if (token == tcSemicolon) {
        pFunctionNode->defn.routine.which = rcForward;
    } else if (token == tcLBracket) {

        if (!pProgram_ptr->foundGlobalEnd) {
            pProgram_ptr->foundGlobalEnd = true;
            icode.GoTo(pProgram_ptr->globalFinishLocation);
            icode.Put(__MAIN_ENTRY__);
            icode.Put(tcSemicolon);
            icode.Put(tcRBracket);

            //--Set the program's icode.
            pProgram_ptr->defn.routine.pIcode = new TIcode(icode);
        }

        pFunctionNode->defn.routine.which = rcDeclared;
        ParseBlock(pFunctionNode);
        pFunctionNode->defn.routine.returnMarker = PutLocationMarker();
    }

    pFunctionNode->defn.routine.pSymtab = symtabStack.ExitScope();

    return pFunctionNode;
}

/** ParseBlock      Parse a function's block:
 *                      
 *                      {
 *                              <compound-statement>
 *                      }
 * 
 * @param pRoutineId : ptr to symbol table node of function's id.
 */
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
