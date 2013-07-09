#include <iostream>
#include "exec.h"

using namespace std;

void TExecutor::ExecuteStatement(const TSymtabNode *pRoutine) {
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
        case tcWhile: //ParseWHILE();
            break;
        case tcIf: ExecuteIF(pRoutine);
            break;
        case tcFor: //ParseFOR();
            break;
        case tcSwitch: //ParseSWITCH();
            break;
        case tcCase:
        case tcDefault://ParseCaseLabel();
            break;
        case tcBreak: //GetToken();
            break;
        case tcLBracket: ExecuteCompound(pRoutine);
            break;
        case tcReturn: ExecuteRETURN(pRoutine);
            break;
    }
}

void TExecutor::ExecuteStatementList(const TSymtabNode *pRoutine, TTokenCode terminator) {
    do {
        ExecuteStatement(pRoutine);
        while (token == tcSemicolon) GetToken();
    } while ((token != terminator) && (token != tcDummy));
}

void TExecutor::ExecuteAssignment(const TSymtabNode *pTargetId) {
    TStackItem *pTarget; // runtime stack address of target
    TType *pTargetType; // ptr to target type object
    TType *pExprType; // ptr to expression type object

    if (pTargetId->defn.how == dcFunction) {
        pTargetType = pTargetId->pType;
        pTarget = runStack.GetValueAddress(pTargetId);
        //GetToken();
    }//--Assignment to variable or formal parameter.
        //--ExecuteVariable leaves the target address on
        //--top of the runtime stack.
    else {
        pTargetType = ExecuteVariable(pTargetId, true);
        pTarget = (TStackItem *) Pop()->__addr;
    }
    //--Execute the expression and leave its value
    //--on top of the runtime stack.
    //GetToken();

    switch (token) {
        case tcReturn:
        case tcEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //--Do the assignment.
            if (pTargetType == pFloatType) {
                pTarget->__float = pExprType->Base() == pIntegerType
                        ? Pop()->__int // real := integer
                        : Pop()->__float; // real := real
            } else if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {
                int value = Pop()->__int;
                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int = value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char = value;
            } else {
                void *pSource = Pop()->__addr;

                //--array  := array
                //--record := record
                memcpy(pTarget, pSource, pTargetType->size);
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcPlusPlus:
            ++pTarget->__int;
            GetToken();
            break;
        case tcMinusMinus:
            --pTarget->__int;
            GetToken();
            break;
        case tcPlusEqual:
            GetToken();
            pExprType = ExecuteExpression();
            //            pTargetNode->value += runStack.Pop();
            break;
        case tcMinusEqual:
            GetToken();
            pExprType = ExecuteExpression();
            //            pTargetNode->value -= runStack.Pop();
            break;
        case tcStarEqual:
            GetToken();
            pExprType = ExecuteExpression();
            //            pTargetNode->value *= runStack.Pop();
            break;
        case tcForwardSlashEqual:
            GetToken();
            pExprType = ExecuteExpression();
            //            pTargetNode->value /= runStack.Pop();
            break;
        case tcModEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //            int __1 = (int) pTargetNode->value;
            //  int __2 = (int) runStack.Pop();

            //            pTargetNode->value = (__1 %= __2);
        }
            break;
        case tcBitLeftShiftEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //           int __1 = (int) pTargetNode->value;
            //  int __2 = (int) runStack.Pop();

            //         pTargetNode->value = (__1 <<= __2);
        }
            break;
        case tcBitRightShiftEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //       int __1 = (int) pTargetNode->value;
            // int __2 = (int) runStack.Pop();

            //     pTargetNode->value = (__1 >>= __2);
        }
            break;
        case tcBitANDEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //          int __1 = (int) pTargetNode->value;
            // int __2 = (int) runStack.Pop();

            //        pTargetNode->value = (__1 &= __2);
        }
            break;
        case tcBitXOREqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //      int __1 = (int) pTargetNode->value;
            //   int __2 = (int) runStack.Pop();

            //    pTargetNode->value = (__1 ^= __2);
        }
            break;
        case tcBitOREqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //            int __1 = (int) pTargetNode->value;
            //            int __2 = (int) runStack.Pop();

            //          pTargetNode->value = (__1 |= __2);
        }
            break;
        case tcSemicolon:
            break;
        case tcIdentifier:
        {
            pExprType = ExecuteExpression();
        }
        default:
            Error(errInvalidAssignment);
            break;
    }
}

void TExecutor::ExecuteDO(const TSymtabNode *pRoutine) {

    int atLoopStart = CurrentLocation(); // location of loop start in icode

    do {
        GetToken(); // do

        ExecuteStatementList(pRoutine, tcWhile);

        GetToken(); //while
        ExecuteExpression(); // (condition)

        if (Pop()->__int) this->GoTo(atLoopStart);
    } while (CurrentLocation() == atLoopStart);
}

void TExecutor::ExecuteCompound(const TSymtabNode *pRoutine) {
    GetToken();

    ExecuteStatementList(pRoutine, tcRBracket);

    //GetToken();
}

void TExecutor::ExecuteIF(const TSymtabNode* pRoutine) {
    GetToken();

    //--Get the location of where to go to if <expr> is false.
    int atFalse = GetLocationMarker();

    //--(
    GetToken();
    ExecuteExpression();
    //--)
    GetToken();
    if (Pop()->__int) {

        //--True: {
        GetToken();
        ExecuteStatement(pRoutine);

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
            GetToken();
            ExecuteStatement(pRoutine);
        }
    }

}
