#include <cstdio>
#include "parser.h"

void TParser::ParseStatement(void) {
    if (token == tcIdentifier) ParseAssignment();
}

void TParser::ParseAssignment(void) {
    TSymtabNode *pTargetNode = SearchAll(pToken->String());

    if (!pTargetNode) pTargetNode = EnterLocal(pToken->String());

    GetToken();


    switch (token) {
        case tcEqual:
            GetToken();
            ParseExpression();
            pTargetNode->value = runStack.Pop();
            break;
        case tcPlusPlus:
            ++pTargetNode->value;
            runStack.Push(pTargetNode->value);
            GetToken();
            break;
        case tcMinusMinus:
            --pTargetNode->value;
            runStack.Push(pTargetNode->value);
            GetToken();
            break;
        case tcPlusEqual:
            GetToken();
            ParseExpression();
            pTargetNode->value += runStack.Pop();
            break;
        case tcMinusEqual:
            GetToken();
            ParseExpression();
            pTargetNode->value -= runStack.Pop();
            break;
        case tcStarEqual:
            GetToken();
            ParseExpression();
            pTargetNode->value *= runStack.Pop();
            break;
        case tcForwardSlashEqual:
            GetToken();
            ParseExpression();
            pTargetNode->value /= runStack.Pop();
            break;
        case tcModEqual:
        {
            GetToken();
            ParseExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 %= __2);
        }
            break;
        case tcBitLeftShiftEqual:
        {
            GetToken();
            ParseExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 <<= __2);
        }
            break;
        case tcBitRightShiftEqual:
        {
            GetToken();
            ParseExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 >>= __2);
        }
            break;
        case tcBitANDEqual:
        {
            GetToken();
            ParseExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 &= __2);
        }
            break;
        case tcBitXOREqual:
        {
            GetToken();
            ParseExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 ^= __2);
        }
            break;
        case tcBitOREqual:
        {
            GetToken();
            ParseExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 |= __2);
        }
            break;
        default:
            Error(errInvalidAssignment);
            break;
    }

    sprintf(::list.text, "\t> %s == %g", pTargetNode->String(), pTargetNode->value);

    list.PutLine();
}
