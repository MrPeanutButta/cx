#include <cstdio>
#include "common.h"
#include "buffer.h"
#include "error.h"
#include "parser.h"
//#include "complist.h"

extern TSymtabNode *pProgram_ptr;

/** Parse       Parse the source file.  After listing each
 *              source line, extract and list its tokens.
 * 
 * @return ptr to '__cx_global__' program Id.
 */
TSymtabNode *TParser::Parse(void) {

    extern bool debugFlag;
    
    TSymtabNode *pProgramId = new TSymtabNode("__cx_global__", dcProgram);
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

    pProgram_ptr = pProgramId;
    icode.Reset();

    currentNestingLevel = 0;
    //--Enter the nesting level 1 and open a new scope for the program.
    symtabStack.SetCurrentSymtab(&globalSymtab);
    icode.Put(tcLBracket);
    GetTokenAppend();

    ParseStatementList(pProgramId, tcEndOfFile);
    FixupLocationMarker(pProgramId->globalFinishLocation);
    pProgramId->defn.routine.returnMarker = PutLocationMarker();

    GetTokenAppend();

    pProgramId->defn.routine.pSymtab = &globalSymtab; //symtabStack.ExitScope();

    Resync(tlProgramEnd);
    CondGetTokenAppend(tcEndOfFile, errMissingRightBracket);

    if (debugFlag) {
        list.PutLine();
        sprintf(list.text, "%20d source lines.", currentLineNumber);
        list.PutLine();
        sprintf(list.text, "%20d syntax errors.", errorCount);
        list.PutLine();
    }

    return pProgramId;

}

/** Resync          Resynchronize the parser.  If the current
 *                  token is not in one of the token lists,
 *                  flag it as an error and then skip tokens
 *                  up to one that is in a list or end of file.
 * 
 * @param pList1 : token list.
 * @param pList2 : token list.
 * @param pList3 : token list.
 */
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
