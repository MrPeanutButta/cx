
#include <iostream>
#include "exec.h"

using namespace std;

void TExecutor::ExecuteExpression(void) {
    ExecuteSimpleExpression();

    TTokenCode op;
    if ((token == tcEqualEqual) || (token == tcNe) ||
            (token == tcLt) || (token == tcGt) ||
            (token == tcLe) || (token == tcGe)) {

        op = token;

        GetToken();
        ExecuteSimpleExpression();

        float __2 = runStack.Pop();
        float __1 = runStack.Pop();

        switch (op) {
            case tcEqualEqual:
                runStack.Push(__1 == __2 ? 1.0 : 0.0);
                break;
            case tcNe:
                runStack.Push(__1 != __2 ? 1.0 : 0.0);
                break;
            case tcLt:
                runStack.Push(__1 < __2 ? 1.0 : 0.0);
                break;
            case tcGt:
                runStack.Push(__1 > __2 ? 1.0 : 0.0);
                break;
            case tcLe:
                runStack.Push(__1 <= __2 ? 1.0 : 0.0);
            case tcGe:
                runStack.Push(__1 >= __2 ? 1.0 : 0.0);
                break;
        }
    }
}

void TExecutor::ExecuteSuffix(TSymtabNode *pNode) {
    TSymtabNode *__n = pNode;
    GetToken();

    if (token == tcPlusPlus) {
        runStack.Push(runStack.Pop() + 1);
        ++__n->value;
        GetToken();
    } else if (token == tcMinusMinus) {
        runStack.Push(runStack.Pop() - 1);
        --__n->value;
        GetToken();
    }
}

void TExecutor::ExecuteSimpleExpression(void) {
    TTokenCode op; //binary operator
    TTokenCode unaryOp = tcPlus; //unary operator

    if ((token == tcMinus) || (token == tcPlus) ||
            (token == tcBitNOT)) {
        unaryOp = token;
        GetToken();
    }

    ExecuteTerm();

    switch (unaryOp) {
        case tcMinus:
            runStack.Push(-runStack.Pop());
            break;
        case tcBitNOT:
            runStack.Push(~((int) runStack.Pop()));
            break;
    }

    while ((token == tcPlus) || (token == tcMinus) ||
            (token == tcBitLeftShift) || (token == tcBitRightShift) ||
            (token == tcBitANDorAddrOf) || (token == tcBitXOR) ||
            (token == tcBitOR) || (token == tcLogicOr)) {

        op = token;

        GetToken();
        ExecuteTerm();

        float __2 = runStack.Pop();
        float __1 = runStack.Pop();

        switch (op) {
            case tcPlus:
                runStack.Push(__1 + __2);
                break;
            case tcMinus:
                runStack.Push(__1 - __2);
                break;
            case tcBitLeftShift:
                runStack.Push((int) __1 << (int) __2);
                break;
            case tcBitRightShift:
                runStack.Push((int) __1 >> (int) __2);
                break;
            case tcBitANDorAddrOf:
                runStack.Push((int) __1 & (int) __2);
                break;
            case tcBitXOR:
                runStack.Push((int) __1 ^ (int) __2);
                break;
            case tcBitOR:
                runStack.Push((int) __1 | (int) __2);
                break;
            case tcLogicOr:
                runStack.Push((__1 != 0.0) || (__2 != 0.0) ? 1.0 : 0.0);
                break;
        }
    }
}

void TExecutor::ExecuteTerm(void) {
    TTokenCode op; // binary operator

    ExecuteFactor();

    while ((token == tcStar) || (token == tcForwardSlash) ||
            (token == tcMod) || (token == tcLogicAnd)) {

        op = token;

        GetToken();
        ExecuteFactor();

        float __2 = runStack.Pop();
        float __1 = runStack.Pop();

        bool divZeroFlag = false;

        switch (op) {
            case tcStar:
                runStack.Push(__1 * __2);
                break;
            case tcForwardSlash:
                if (__2 != 0.0) runStack.Push(__1 / __2);
                else divZeroFlag = true;
                break;
            case tcMod:
                if (__2 != 0.0) runStack.Push(int(__1) % int(__2));
                else divZeroFlag = true;
                break;
            case tcLogicAnd:
                runStack.Push((__1 != 0.0) && (__2 != 0.0) ? 1.0 : 0.0);
                break;
        }

        if (divZeroFlag) {
            RuntimeError(rteDivisionByZero);
            runStack.Push(0.0);
        }
    }
}

void TExecutor::ExecuteFactor(void) {
    switch (token) {
        case tcIdentifier:
        {

            if (pNode == pInputNode) {
                cout << "input = "; cin >> pNode->value;
                if (!cin.good())RuntimeError(rteInvalidUserInput);
            }

            runStack.Push(pNode->value);

            if (pNode != pInputNode)ExecuteSuffix(pNode);
            else GetToken();

        }
            break;
        case tcNumber:
        {
            runStack.Push(pNode->value);
            GetToken();
        }
            break;
        case tcString:
            runStack.Push(0.0);
            GetToken();
            break;
        case tcLogicNOT:
            GetToken();
            ExecuteFactor();
            runStack.Push(!(runStack.Pop()));
            break;
        case tcLParen:
        {
            GetToken();
            ExecuteExpression();
            GetToken();
        }
            break;
        case tcSemicolon:
            break;
    }
}