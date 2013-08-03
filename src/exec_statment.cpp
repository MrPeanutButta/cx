/** Executor (Statements)
 * exec_routine.cpp
 *
 * Execute standard Cx statements
 */

#include <iostream>
#include "exec.h"
#include "common.h"

using namespace std;

/** ExecuteStatement   	Execute a Cx statement
 *
 * @param pRoutineId : ptr to the routine symtab node
 */
void TExecutor::ExecuteStatement(TSymtabNode *pRoutineId) {
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
        case tcDo: ExecuteDO(pRoutineId);
            break;
        case tcWhile: ExecuteWHILE(pRoutineId);
            break;
        case tcIf: ExecuteIF(pRoutineId);
            break;
        case tcFor: ExecuteFOR(pRoutineId);
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
        case tcLBracket: ExecuteCompound(pRoutineId);
            break;
        case tcReturn: ExecuteRETURN(pRoutineId);
            break;
    }
}

/** ExecuteStatementList        Execute a list or compounded
 *                              statements until a terminator token
 *                              is reached.
 *
 * @param pRoutineId : ptr to the routine symtab node
 * @param terminator : token to terminate compound execution.
 */
void TExecutor::ExecuteStatementList(TSymtabNode *pRoutineId, TTokenCode terminator) {
    do {
        ExecuteStatement(pRoutineId);

        while (token == tcSemicolon) GetToken();
    } while ((token != terminator) && (token != tcDummy) && (!breakLoop));
}

/** ExecuteAssignment 	Execute assignment.
 *
 *      pTargetId =, +=, -=, ++, --, /=, *=, %=, ^=
 *                >>=, <<=, &=, |= <expression>;
 * 
 * @param pTargetId : ptr to the symtab node being assigned some value
 *                    on the stack.
 */
void TExecutor::ExecuteAssignment(const TSymtabNode *pTargetId) {
    TStackItem *pTarget = nullptr; // runtime stack address of target
    TType *pTargetType = nullptr; // ptr to target type object
    TType *pExprType = nullptr; // ptr to expression type object
    TType *pExprType2 = nullptr; // reserved for casting

    if (pTargetId->defn.how == dcFunction) {
        pTargetType = pTargetId->pType;
        pTarget = runStack.GetValueAddress(pTargetId);
    }//--Assignment to variable or formal parameter.
        //--ExecuteVariable leaves the target address on
        //--top of the runtime stack.
    else if ((pTargetId->defn.how != dcType)) {
        if (!TokenIn(token, tlAssignOps))GetToken();
        pTargetType = ExecuteVariable(pTargetId, true);
        pTarget = (TStackItem *) Pop()->__addr;
    }

    switch (token) {
        case tcReturn:
        case tcEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();

            if (token != tcSemicolon) pExprType2 = ExecuteExpression();

            //--Do the assignment.
            if (pTargetType == pFloatType) {

                if (!pExprType2) {
                    pTarget->__float =
                            (pExprType->Base() == pIntegerType)
                            ? Pop()->__int // real := integer
                            : Pop()->__float; // real := real
                } else {
                    pTarget->__float =
                            (pExprType2->Base() == pIntegerType)
                            ? Pop()->__int // real := integer
                            : Pop()->__float; // real := real
                }
            } else if (((pTargetType->Base() == pIntegerType) &&
                    (pTargetType->Base()->form != fcArray)) ||
                    (pTargetType->Base()->form == fcEnum)) {
                int value(0);

                if (!pExprType2) {
                    value = ((pExprType->Base() == pIntegerType) ||
                            (pExprType->Base() == pBooleanType))
                            ? Pop()->__int // real := integer
                            : Pop()->__float; // real := real
                } else {
                    value = ((pExprType2->Base() == pIntegerType) ||
                            (pExprType2->Base() == pBooleanType))
                            ? Pop()->__int // real := integer
                            : Pop()->__float; // real := real
                }

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
        {
            GetToken();
            //--Do the assignment.
            if (pTargetType == pFloatType) {
                pTarget->__float++;
            } else if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {
                pTarget->__int++;
            } else if (pTargetType->Base() == pCharType) {
                pTarget->__char++;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcMinusMinus:
        {
            GetToken();
            //--Do the assignment.
            if (pTargetType == pFloatType) {
                pTarget->__float--;
            } else if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {
                pTarget->__int--;
            } else if (pTargetType->Base() == pCharType) {
                pTarget->__char--;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcPlusEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //--Do the assignment.
            if (pTargetType == pFloatType) {
                pTarget->__float += pExprType->Base() == pIntegerType
                        ? Pop()->__int // real := integer
                        : Pop()->__float; // real := real
            } else if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {

                int value = pExprType->Base() == pIntegerType
                        ? Pop()->__int // real := integer
                        : Pop()->__float; // real := real

                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int += value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char += value;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcMinusEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //--Do the assignment.
            if (pTargetType == pFloatType) {
                pTarget->__float -= pExprType->Base() == pIntegerType
                        ? Pop()->__int // real := integer
                        : Pop()->__float; // real := real
            } else if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {

                int value = pExprType->Base() == pIntegerType
                        ? Pop()->__int // real := integer
                        : Pop()->__float; // real := real

                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int -= value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char -= value;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcStarEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();

            // if not semicolon, this may be an expression following a cast.
            if (token != tcSemicolon)ExecuteExpression();

            //--Do the assignment.
            if (pTargetType == pFloatType) {
                pTarget->__float *= pExprType->Base() == pIntegerType
                        ? Pop()->__int // real := integer
                        : Pop()->__float; // real := real
            } else if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {

                int value = pExprType->Base() == pIntegerType
                        ? Pop()->__int // real := integer
                        : Pop()->__float; // real := real

                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int *= value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char -= value;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcForwardSlashEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //--Do the assignment.
            if (pTargetType == pFloatType) {
                pTarget->__float /= pExprType->Base() == pIntegerType
                        ? Pop()->__int // real := integer
                        : Pop()->__float; // real := real
            } else if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {

                int value = pExprType->Base() == pIntegerType
                        ? Pop()->__int // real := integer
                        : Pop()->__float; // real := real

                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int /= value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char /= value;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcModEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //--Do the assignment.
            if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {

                int value = Pop()->__int; // real := integer

                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int %= value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char %= value;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcBitLeftShiftEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //--Do the assignment.
            if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {

                int value = Pop()->__int; // real := integer

                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int <<= value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char <<= value;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcBitRightShiftEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //--Do the assignment.
            if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {

                int value = Pop()->__int; // real := integer

                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int >>= value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char >>= value;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcBitANDEqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //--Do the assignment.
            if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {

                int value = Pop()->__int; // real := integer

                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int &= value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char <<= value;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcBitXOREqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //--Do the assignment.
            if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {

                int value = Pop()->__int; // real := integer

                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int <<= value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char ^= value;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
        case tcBitOREqual:
        {
            GetToken();
            pExprType = ExecuteExpression();
            //--Do the assignment.
            if ((pTargetType->Base() == pIntegerType) ||
                    (pTargetType->Base()->form == fcEnum)) {

                int value = Pop()->__int; // real := integer

                RangeCheck(pTargetType, value);

                //--integer     := integer
                //--enumeration := enumeration
                pTarget->__int <<= value;
            } else if (pTargetType->Base() == pCharType) {
                char value = Pop()->__char;
                RangeCheck(pTargetType, value);

                //--character := character
                pTarget->__char |= value;
            }

            TraceDataStore(pTargetId, pTarget, pTargetType);
        }
            break;
    }
}

/** ExecuteDO   Executes do/while statement while <expression> is true.
 * 
 *      do
 *      <staement>
 *      while(<expression>);
 * 
 * @param pRoutineId : routine ID this statement is apart of. 
 */
void TExecutor::ExecuteDO(TSymtabNode * pRoutineId) {

    int breakPoint; // = GetLocationMarker();
    int atLoopStart = CurrentLocation(); // location of loop start in icode;
    int condition = 0;

    do {

        GetToken(); // do
        breakPoint = GetLocationMarker();
        GetToken();

        ExecuteStatementList(pRoutineId, tcWhile);

        if (breakLoop) {
            GoTo(breakPoint);
            GetToken();
            break;
        }

        GetToken(); //while
        ExecuteExpression(); // (condition)
        condition = Pop()->__int;

        if (condition != 0) this->GoTo(atLoopStart);
    } while (CurrentLocation() == atLoopStart);

    // reset break flag
    breakLoop = false;

}

/** ExecuteWHILE        Executes while statement while <expression> is true.
 * 
 *      while(<expression>)
 *            <statement>
 * 
 * @param pRoutineId : routine ID this statement is apart of. 
 */
void TExecutor::ExecuteWHILE(TSymtabNode * pRoutineId) {

    int breakPoint;
    int atLoopStart = CurrentLocation();
    int condition = 0;

    do {

        GetToken(); // while
        breakPoint = GetLocationMarker();
        GetToken();

        GetToken(); //-- (
        ExecuteExpression();
        condition = Pop()->__int;
        GetToken(); //-- )
        if (condition != 0) {
            ExecuteStatement(pRoutineId);

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

/** ExecuteCompound     Execute statement block.
 * 
 *      {       // begin
 *              <statements>
 *      }       // end
 * 
 * @param pRoutineId : routine ID this statement is apart of. 
 */
void TExecutor::ExecuteCompound(TSymtabNode * pRoutineId) {

    GetToken();

    ExecuteStatementList(pRoutineId, tcRBracket);

    if (token == tcRBracket)GetToken();
}

/** ExecuteIF   Executes if statements.
 * 
 *      if(<boolean expression>)
 *              <statement>
 *      else if (<boolean expression>)
 *              <statement>
 *      else 
 *              <statement>
 * 
 * @param pRoutineId : routine ID this statement is apart of. 
 */
void TExecutor::ExecuteIF(TSymtabNode * pRoutineId) {
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
        ExecuteStatement(pRoutineId);
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
            ExecuteStatement(pRoutineId);

            while (token == tcSemicolon)GetToken();
        }
    }
}

/** ExecuteFOR  Executes for statement.
 *          initialize   condition     increment
 *      for(<statement>; <expression>; <expression>)
 *              <statement>
 * 
 * @param pRoutineId
 */
void TExecutor::ExecuteFOR(TSymtabNode * pRoutineId) {

    int condition = 0;

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

    breakLoop = false;
}