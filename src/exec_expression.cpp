
#include <iostream>
#include "exec.h"
#include "common.h"

using namespace std;

TType *TExecutor::ExecuteExpression(void) {
    TType *pOperand1Type; // ptr to first  operand's type
    TType *pOperand2Type; // ptr to second operand's type
    TType *pResultType; // ptr to result type
    TTokenCode op; // operator

    //--Execute the first simple expression.
    pResultType = ExecuteSimpleExpression();

    //--If we now see a relational operator,
    //--execute the second simple expression.
    if (TokenIn(token, tlRelOps)) {
        op = token;
        pOperand1Type = pResultType->Base();
        pResultType = pBooleanType;

        GetToken();
        pOperand2Type = ExecuteSimpleExpression()->Base();

        //--Perform the operation, and push the resulting value
        //--onto the stack.
        if (((pOperand1Type == pIntegerType) &&
                (pOperand2Type == pIntegerType))
                || ((pOperand1Type == pCharType) &&
                (pOperand2Type == pCharType))
                || (pOperand1Type->form == fcEnum)) {

            //--integer <op> integer
            //--boolean <op> boolean
            //--char    <op> char
            //--enum    <op> enum
            int value1, value2;
            if (pOperand1Type == pCharType) {
                value2 = Pop()->__char;
                value1 = Pop()->__char;
            } else {
                value2 = Pop()->__int;
                value1 = Pop()->__int;
            }

            switch (op) {

                case tcEqualEqual:
                    Push(value1 == value2);
                    break;

                case tcNe:
                    Push(value1 != value2);
                    break;

                case tcLt:
                    Push(value1 < value2);
                    break;

                case tcGt:
                    Push(value1 > value2);
                    break;

                case tcLe:
                    Push(value1 <= value2);
                    break;

                case tcGe:
                    Push(value1 >= value2);
                    break;
            }
        } else if ((pOperand1Type == pFloatType) ||
                (pOperand2Type == pFloatType)) {

            //--real    <op> real
            //--real    <op> integer
            //--integer <op> real
            float value2 = pOperand2Type == pFloatType ? Pop()->__float
                    : Pop()->__int;
            float value1 = pOperand1Type == pFloatType ? Pop()->__float
                    : Pop()->__int;

            switch (op) {

                case tcEqualEqual:
                    Push(value1 == value2);
                    break;

                case tcNe:
                    Push(value1 != value2);
                    break;

                case tcLt:
                    Push(value1 < value2);
                    break;

                case tcGt:
                    Push(value1 > value2);
                    break;

                case tcLe:
                    Push(value1 <= value2);
                    break;

                case tcGe:
                    Push(value1 >= value2);
                    break;
            }
        } else {

            //--string <op> string
            char *addr2 = (char *) Pop()->__addr;
            char *addr1 = (char *) Pop()->__addr;

            int cmp = strncmp(addr1, addr2, pOperand1Type->size);

            switch (op) {
                case tcEqualEqual: Push(cmp == 0);
                    break;
                case tcNe: Push(cmp != 0);
                    break;
                case tcLt: Push(cmp < 0);
                    break;
                case tcGt: Push(cmp > 0);
                    break;
                case tcLe: Push(cmp <= 0);
                    break;
                case tcGe: Push(cmp >= 0);
                    break;
            }
        }
    }

    return pResultType;
}

TType *TExecutor::ExecuteSimpleExpression(void) {

    TType *pOperandType; // ptr to operand's type
    TType *pResultType; // ptr to result type
    TTokenCode op; // operator
    TTokenCode unaryOp = tcPlus; // unary operator

    //--Unary + or -
    if (TokenIn(token, tlUnaryOps)) {
        unaryOp = token;
        GetToken();
    }

    //--Execute the first term.
    pResultType = ExecuteTerm();

    switch (unaryOp) {
        case tcMinus:
            if (pResultType == pFloatType) Push(-Pop()->__float);
            else Push(-Pop()->__int);
            break;
        case tcBitNOT:
            Push(~(Pop()->__int));
            break;
    }

    //--Loop to execute subsequent additive operators and terms.
    while (TokenIn(token, tlAddOps)) {
        op = token;
        pResultType = pResultType->Base();

        GetToken();
        pOperandType = ExecuteTerm()->Base();

        switch (op) {
            case tcPlus:
            case tcMinus:
            {
                if ((pResultType == pIntegerType) &&
                        (pOperandType == pIntegerType)) {

                    //--integer +|- integer
                    int value2 = Pop()->__int;
                    int value1 = Pop()->__int;

                    Push(op == tcPlus ? value1 + value2
                            : value1 - value2);
                    pResultType = pIntegerType;
                } else {

                    //--real    +|- real
                    //--real    +|- integer
                    //--integer +|- real
                    float value2 = pOperandType == pFloatType ? Pop()->__float
                            : Pop()->__int;
                    float value1 = pResultType == pFloatType ? Pop()->__float
                            : Pop()->__int;

                    Push(op == tcPlus ? value1 + value2
                            : value1 - value2);
                    pResultType = pFloatType;
                }

            }
                break;
            case tcBitLeftShift:
            {
                //--bit left shift
                int value2 = Pop()->__int;
                int value1 = Pop()->__int;

                Push(value1 << value2);
                pResultType = pIntegerType;
            }
                break;
            case tcBitRightShift:
            {
                //--bit right shift
                int value2 = Pop()->__int;
                int value1 = Pop()->__int;

                Push(value1 >> value2);
                pResultType = pIntegerType;
            }
                break;
            case tcBitANDorAddrOf:
            {
                //--bit and
                int value2 = Pop()->__int;
                int value1 = Pop()->__int;

                Push(value1 & value2);
                pResultType = pIntegerType;
            }
                break;
            case tcBitXOR:
            {
                //--bit left shift
                int value2 = Pop()->__int;
                int value1 = Pop()->__int;

                Push(value1 ^ value2);
                pResultType = pIntegerType;
            }
                break;
            case tcBitOR:
            {
                //--bit left shift
                int value2 = Pop()->__int;
                int value1 = Pop()->__int;

                Push(value1 | value2);
                pResultType = pIntegerType;
            }
                break;
            case tcLogicOr:
            {
                //--boolean OR boolean
                int value2 = Pop()->__int;
                int value1 = Pop()->__int;

                Push(value1 || value2);
                pResultType = pBooleanType;
            }
                break;
        }
    }

    return pResultType;
}

//--------------------------------------------------------------
//  ExecuteTerm         Execute a term (binary operators * / DIV
//                      MOD and AND).
//
//  Return: ptr to term's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteTerm(void) {
    TType *pOperandType; // ptr to operand's type
    TType *pResultType; // ptr to result type
    TTokenCode op; // operator

    //--Execute the first factor.
    pResultType = ExecuteFactor();

    //--Loop to execute subsequent multiplicative operators and factors.
    while (TokenIn(token, tlMulOps)) {
        op = token;
        pResultType = pResultType->Base();

        GetToken();
        pOperandType = ExecuteFactor()->Base();

        bool divZeroFlag = false;

        switch (op) {
            case tcStar:
                if ((pResultType == pIntegerType) &&
                        (pOperandType == pIntegerType)) {

                    //--integer * integer
                    int value2 = Pop()->__int;
                    int value1 = Pop()->__int;

                    Push(value1 * value2);
                    pResultType = pIntegerType;
                } else {

                    //--real    * real
                    //--real    * integer
                    //--integer * real
                    float value2 = pOperandType == pFloatType
                            ? Pop()->__float
                            : Pop()->__int;
                    float value1 = pResultType == pFloatType
                            ? Pop()->__float
                            : Pop()->__int;

                    Push(value1 * value2);
                    pResultType = pFloatType;
                }
                break;
            case tcForwardSlash:
            {

                //--real    / real
                //--real    / integer
                //--integer / real
                //--integer / integer

                float value2 = pOperandType == pFloatType
                        ? Pop()->__float
                        : Pop()->__int;
                float value1 = pResultType == pFloatType
                        ? Pop()->__float
                        : Pop()->__int;

                if (value2 == 0.0f) RuntimeError(rteDivisionByZero);

                Push(value1 / value2);
                pResultType = pFloatType;
            }
                break;
            case tcMod:
            {
                //--integer DIV|MOD integer
                int value2 = Pop()->__int;
                int value1 = Pop()->__int;

                if (value2 == 0) RuntimeError(rteDivisionByZero);

                pResultType = pIntegerType;
            }
                break;
            case tcLogicAnd:
            {
                //--boolean AND boolean
                int value2 = Pop()->__int;
                int value1 = Pop()->__int;

                Push(value1 && value2);
                pResultType = pBooleanType;
            }
                break;
        }
    }

    return pResultType;
}

//--------------------------------------------------------------
//  ExecuteFactor       Execute a factor (identifier, number,
//                      string, NOT <factor>, or parenthesized
//                      subexpression).  An identifier can be
//                      a function, constant, or variable.
//
//  Return: ptr to factor's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteFactor(void) {
    TType *pResultType = nullptr; // ptr to result type
    TSymtabNode *pId = nullptr;

    switch (token) {
        case tcIdentifier:
        {
            switch (pNode->defn.how) {

                case dcFunction:
                    pResultType = ExecuteSubroutineCall(pNode);
                    break;

                case dcConstant:
                    pResultType = ExecuteConstant(pNode);
                    break;
                default:
                    pId = pNode;
                    GetToken();
                    pResultType = ExecuteVariable(pId, false);
                    
                    if (TokenIn(token, tlAssignOps)) {
                        
                        ExecuteAssignment(pId);
                        pResultType = ExecuteVariable(pId, false);
                    }

                    break;
            }
        }
            break;
        case tcBitANDorAddrOf:
            GetToken();
            switch (pNode->defn.how) {

                case dcFunction:
                case dcConstant:
                case dcVariable:
                    pId = pNode;

					GetToken();
					// leave address TOS
					pResultType = ExecuteVariable(pId, true);

                    if (TokenIn(token, tlAssignOps)) {
                        ExecuteAssignment(pId);
                        pResultType = ExecuteVariable(pId, true);
                    }
                    break;
            }
            break;
			        
		case tcStar:
            GetToken();
            switch (pNode->defn.how) {

			case dcPointer:
                    // leave address value TOS
                    pResultType = ExecuteFactor();
                    break;
            }
            break;
        case tcNumber:
        {
            //--Push the number's integer or real value onto the stack.
            if (pNode->pType == pIntegerType) {
                Push(pNode->defn.constant.value.__int);
            } else {
                Push(pNode->defn.constant.value.__float);
            }
            pResultType = pNode->pType;
            GetToken();
        }
            break;
        case tcChar:
        case tcString:
        {
            //--Push either a character or a string address onto the
            //--runtime stack, depending on the string length.
            int length = strlen(pNode->String()) - 2; // skip quotes
            if (length == 1) {

                //--Character
                Push(pNode->defn.constant.value.__char);
                pResultType = pCharType;
            } else {

                //--String address
                Push(pNode->defn.constant.value.pString);
                pResultType = pNode->pType;
            }

            GetToken();
        }
            break;
        case tcLogicNOT:
            //--Execute boolean factor and invert its value.
            GetToken();
            ExecuteFactor();

            Push(1 - Pop()->__int);
            pResultType = pBooleanType;
            break;
        case tcLParen:
        {

            //--Parenthesized subexpression:  Call ExecuteExpression
            //--                              recursively.
            GetToken(); // first token after (

            pResultType = ExecuteExpression();

            GetToken(); // first token after )
        }
            break;
        case tcSemicolon:
            break;
    }

    return pResultType;
}

//--------------------------------------------------------------
//  ExecuteConstant     Push a constant onto the runtime stack.
//
//      pId : ptr to constant identifier's symbol table node
//
//  Return: ptr to constant's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteConstant(const TSymtabNode *pId) {
    TType *pType = pId->pType;
    TDataValue value = pId->defn.constant.value;

    if (pType == pFloatType) Push(value.__float);
    else if (pType == pCharType) Push(value.__char);
    else if (pType->form == fcArray) Push(value.pString);
    else Push(value.__int);

    GetToken();
    TraceDataFetch(pId, TOS(), pType);
    return pType;
}

//--------------------------------------------------------------
//  ExecuteVariable     Push a variable's value or address onto
//                      the runtime stack.
//
//      pId         : ptr to variable's symbol table node
//      addressFlag : true to push address, false to push value
//
//  Return: ptr to variable's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteVariable(const TSymtabNode *pId,
        int addressFlag) {

    TType *pType = pId->pType;

    //--Get the variable's runtime stack address.
    TStackItem *pEntry = runStack.GetValueAddress(pId);
    Push((pId->defn.how == dcReference) || (!pType->IsScalar())
            ? pEntry->__addr : pEntry);

    //--Loop to execute any subscripts and field designators,
    //--which will modify the data address at the top of the stack.
    int doneFlag = false;
    do {
        switch (token) {

            case tcLeftSubscript:

                pType = ExecuteSubscripts(pType);

                break;

            case tcDot:
                pType = ExecuteField();
                break;

            default: doneFlag = true;
        }
    } while (!doneFlag);

	//if (!TokenIn(token, tlAssignOps))GetToken();

    //--If addressFlag is false, and the data is not an array
    //--or a record, replace the address at the top of the stack
    //--with the data value.
    if ((!addressFlag) && (pType->IsScalar())) {
        if (pType == pFloatType) {
            Push(((TStackItem *) Pop()->__addr)->__float);
        } else if (pType->Base() == pCharType) {
            Push(((TStackItem *) Pop()->__addr)->__char);
        } else {
            Push(((TStackItem *) Pop()->__addr)->__int);
        }
    }

    if (!addressFlag) {
        void *pDataValue = pType->IsScalar() ? TOS() : TOS()->__addr;

        TraceDataFetch(pId, pDataValue, pType);
    }

    return pType;
}

//--------------------------------------------------------------
//  ExecuteSubscripts   Execute each subscript expression to
//                      modify the data address at the top of
//                      the runtime stack.
//
//      pType : ptr to array type object
//
//  Return: ptr to subscripted variable's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteSubscripts(const TType *pType) {
    //--Loop to executed subscript lists enclosed in brackets.
    while (token == tcLeftSubscript) {

        //--Loop to execute comma-separated subscript expressions
        //--within a subscript list.
        do {
            GetToken(); // index
            ExecuteExpression();

            //--Evaluate and range check the subscript.
            int value = Pop()->__int;
            RangeCheck(pType, value);


            //--Modify the data address at the top of the stack.
            Push(((char *) Pop()->__addr) +
                    pType->array.pElmtType->size * (value - pType->array.minIndex));

            //--Prepare for another subscript in this list.
            if (token == tcComma) pType = pType->array.pElmtType;

        } while (token == tcComma);

        //--Prepare for another subscript list.
        GetToken(); // ]
        if (token == tcLeftSubscript) pType = pType->array.pElmtType;
    }

    return pType->array.pElmtType;
}

//--------------------------------------------------------------
//  ExecuteField        Execute a field designator to modify the
//                      data address at the top of the runtime
//                      stack
//
//  Return: ptr to record field's type object
//--------------------------------------------------------------

TType *TExecutor::ExecuteField(void) {
    GetToken();
    TSymtabNode *pFieldId = pNode;

    Push(((char *) (Pop()->__addr)) + pFieldId->defn.data.offset);

    GetToken();
    return pFieldId->pType;
}
//endfig

