#include <iostream>
#include "exec.h"

using namespace std;

void TExecutor::ExecuteStatement(TSymtabNode *pRoutine) {
    if (token != tcLBracket) {
        ++stmtCount;
        TraceStatement();
    }

    switch (token) {
        case tcIdentifier:
        {
            if (pNode->defn.how == dcFunction) {
                ExecuteSubroutineCall(pNode);
            } else {
                ExecuteAssignment(pNode);
            }
        }
            break;
        case tcDo: ExecuteDO(pRoutine);
            break;
        case tcWhile: ExecuteWHILE(pRoutine);
            break;
        case tcIf: ExecuteIF(pRoutine);
            break;
        case tcFor: ExecuteFOR(pRoutine);
            break;
        case tcSwitch: //ParseSWITCH();
            break;
        case tcCase:
        case tcDefault://ParseCaseLabel();
            break;
        case tcBreak:
            GetToken();
            breakLoop = true;
            break;
        case tcLBracket: ExecuteCompound(pRoutine);
            break;
        case tcReturn: ExecuteRETURN(pRoutine);
            break;
    }
}

void TExecutor::ExecuteStatementList(TSymtabNode *pRoutine, TTokenCode terminator) {
    do {
        ExecuteStatement(pRoutine);

        while (token == tcSemicolon) GetToken();
    } while ((token != terminator) && (token != tcDummy) && (!breakLoop));
}

TSymtabNode *TExecutor::EnterNew(TSymtabNode *pFunction, const char *pString) {
    TSymtabNode *pId = nullptr;

    //--local parameters and variables.
    for (pId = pFunction->defn.routine.locals.pVariableIds;
            pId;
            pId = pId->next) {

        if (pId == pNode) return pNode;
    }

    pId = pNode;

    if (pFunction && (!pFunction->defn.routine.locals.pVariableIds)) {
        pFunction->defn.routine.locals.pVariableIds = pId;
        pFunction->defn.routine.locals.pVariableIds->prev = pId;
        pId->defn.data.offset = pFunction->defn.routine.parmCount + 1;
        pFunction->defn.routine.totalLocalSize = pId->pType->size;
    } else {
        TSymtabNode *__var = pFunction->defn.routine.locals.pVariableIds->prev;
        int offset = __var->defn.data.offset + 1;
        pId->defn.data.offset = offset;

        __var->next = pId;
        __var->prev = pId;

        pFunction->defn.routine.totalLocalSize += pId->pType->size;

    }

    runStack.AllocateValue(pId);

    return pId;
}

TSymtabNode *TExecutor::AllocNewNode(TSymtabNode *pRoutineId) {
    //TSymtabNode *pNode = SearchAvailableScopes(pToken->String());
    TSymtabNode *pNewId = nullptr;

    if ((pNode->defn.how == dcType) && (pNode->pType->form != fcComplex) &&
            (pNode->defn.how != dcFunction)) {

        GetToken(); // type

        TSymtabNode *pNewId = nullptr;

        pNewId = EnterNew(pRoutineId, pNode->String());

        //GetToken(); // id
        return pNewId;
    }
}

void TExecutor::ExecuteAssignment(const TSymtabNode *pTargetId) {
    ExecuteVariable(pTargetId, true);
}

void TExecutor::ExecuteDO(TSymtabNode *pRoutine) {

    int breakPoint; // = GetLocationMarker();
    int atLoopStart = CurrentLocation(); // location of loop start in icode;
    int condition = 0;

    do {

        GetToken(); // do
        breakPoint = GetLocationMarker();
        GetToken();

        ExecuteStatementList(pRoutine, tcWhile);

        if (breakLoop) {
            GoTo(breakPoint);
            GetToken();
            break;
        }

        GetToken(); //while
        //GetToken();
        ExecuteExpression(); // (condition)
        //GetToken();
        condition = Pop()->__int;

        if (condition != 0) this->GoTo(atLoopStart);
    } while (CurrentLocation() == atLoopStart);

    // reset break flag
    breakLoop = false;

}

void TExecutor::ExecuteWHILE(TSymtabNode *pRoutine) {

    int breakPoint;
    int atLoopStart = CurrentLocation();
    int condition = 0;

    do {

        GetToken(); // while
        breakPoint = GetLocationMarker();
        GetToken();

        ExecuteExpression();
        condition = Pop()->__int;

        if (condition != 0) {
            ExecuteStatement(pRoutine);

            if (breakLoop) {
                GoTo(breakPoint);
                GetToken();
                break;
            }

            GoTo(atLoopStart);
        } else GoTo(breakPoint);



    } while (CurrentLocation() == atLoopStart);

    // reset break
    breakLoop = false;
}

void TExecutor::ExecuteCompound(TSymtabNode *pRoutine) {

    GetToken();

    ExecuteStatementList(pRoutine, tcRBracket);

    if (token == tcRBracket)GetToken();
}

void TExecutor::ExecuteIF(TSymtabNode* pRoutine) {
    //-- if
    GetToken();

    //--Get the location of where to go to if <expr> is false.
    int atFalse = GetLocationMarker();
    GetToken();

    //--(
    GetToken();

    ExecuteExpression();
    int condition = Pop()->__int;

    //--)
    GetToken();

    if (condition != 0) {

        //--True: { or single statement
        ExecuteStatement(pRoutine);
        while (token == tcSemicolon)GetToken();

        //--If there is an ELSE part, jump around it.
        if (token == tcElse) {
            GetToken();
            GoTo(GetLocationMarker());
            GetToken(); // token following the IF statement
        }
    } else {

        //--False: Go to the false location.
        GoTo(atFalse);
        GetToken();

        if (token == tcElse) {

            //--ELSE <stmt-2>
            GetToken();
            GetLocationMarker(); // skip over location marker
            //--{ or single statement
            GetToken();
            ExecuteStatement(pRoutine);

            while (token == tcSemicolon)GetToken();
        }
    }
}

void TExecutor::ExecuteFOR(TSymtabNode* pRoutineId) {

    int condition = 0;
    //TSymtabNode pControl;

    GetToken(); //--for
    //--Get the location of where to go to if <expr> is false.
    int breakPoint = GetLocationMarker();
    GetToken();
    int statementLocation = GetLocationMarker();
    GetToken();
    int conditionMarker = GetLocationMarker();
    GetToken();
    int incrementMarker = GetLocationMarker();


    GetToken();

    //--(
    GetToken();

    if (token != tcSemicolon) {
        // declaration would go here //
        //pControl = pNode;
        ExecuteAssignment(pNode);
    }

    do {
        GetToken(); //-- ;
        if (token != tcSemicolon) {

            // expr 2
            ExecuteExpression();
            GetToken(); //-- ;
        } else GetToken();

        condition = Pop()->__int;
        if (condition != 0) {
            GoTo(statementLocation);
            GetToken();
            ExecuteStatement(pRoutineId);
            if (breakLoop) GoTo(breakPoint);
        } else {
            GoTo(breakPoint);
            GetToken();
            break;
        }

        GoTo(incrementMarker);
        GetToken();
        // expr 3
        ExecuteExpression();

        GoTo(conditionMarker);
    } while (CurrentLocation() == conditionMarker);

    //runStack.DeallocateValue(pControl);
    breakLoop = false;
}