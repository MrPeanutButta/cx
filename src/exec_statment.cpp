#include <iostream>
#include "exec.h"

using namespace std;

void TExecutor::ExecuteStatement(void) {
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
        case tcDo: ExecuteDO();
            break;
        case tcWhile: //ParseWHILE();
            break;
        case tcIf: //ParseIF();
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
        case tcLBracket: ExecuteCompound();
            break;
        case tcReturn: //ParseRETURN();
            break;
    }
}

void TExecutor::ExecuteStatementList(TTokenCode terminator) {
    do {
        ExecuteStatement();
        while (token == tcSemicolon) GetToken();
    } while (token != terminator);
}

void TExecutor::ExecuteAssignment(const TSymtabNode *pTargetId) {
    TStackItem *pTarget; // runtime stack address of target
    TType *pTargetType; // ptr to target type object
    TType *pExprType; // ptr to expression type object

    //--Assignment to function name.
    if (pTargetId->defn.how == dcFunction) {
        pTargetType = pTargetId->pType;
        pTarget = runStack.GetValueAddress(pTargetId);
        GetToken();
    }        //--Assignment to variable or formal parameter.
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
        case tcEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //            pTargetNode->value = runStack.Pop();
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
        default:
            Error(errInvalidAssignment);
            break;
    }
}

void TExecutor::ExecuteDO(void) {

    int atLoopStart = CurrentLocation(); // location of loop start in icode

    do {
        GetToken(); // do

        ExecuteStatementList(tcWhile);

        GetToken(); //while
        ExecuteExpression(); // (condition)

        if (Pop()->__int) this->GoTo(atLoopStart);
    } while (CurrentLocation() == atLoopStart);
}

void TExecutor::ExecuteCompound(void) {
    GetToken();

    ExecuteStatementList(tcRBracket);

    GetToken();
}
