#include <cstdio>
#include "parser.h"

void TParser::ParseExpression(void) {
    ParseSimpleExpression();
}

void TParser::ParseSimpleExpression(void) {
    TTokenCode op; //binary operator
    TTokenCode unaryOp = tcPlus; //unary operator

    if ((token == tcMinus) || (token == tcPlus)) {
        unaryOp = token;
        GetToken();
    }

    ParseTerm();
    if (unaryOp == tcMinus) runStack.Push(-runStack.Pop());

    while ((token == tcMinus) || (token == tcPlus)) {
        op = token;

        GetToken();
        ParseTerm();

        float operand2 = runStack.Pop();
        float operand1 = runStack.Pop();

        runStack.Push(op == tcPlus ? operand1 + operand2 : operand1 - operand2);
    }
}

void TParser::ParseTerm(void) {
    TTokenCode op; // binary operator

    ParseFactor();

    while ((token == tcStar) || (token == tcForwardSlash)) {
        op = token;

        GetToken();
        ParseFactor();

        float operand2 = runStack.Pop();
        float operand1 = runStack.Pop();

        if (op == tcStar)runStack.Push(operand1 * operand2);
        else if (op == tcForwardSlash) {
            if (operand2 != 0.0) runStack.Push(operand1 / operand2);
            else {
                ::list.PutLine("runtime error: division by 0");
                runStack.Push(0.0);
            }
        }
    }
}

void TParser::ParseFactor(void) {
    switch (token) {
        case tcIdentifier:
        {
            TSymtabNode *pNode = SearchAll(pToken->String());
            if (pNode) {
                runStack.Push(pNode->value);
                sprintf(::list.text, "\t>> %s == %g", pNode->String(), pNode->value);
                ::list.PutLine();
            } else {
                Error(errUndefinedIdentifier);
                runStack.Push(0.0);
            }

            GetToken();
            break;
        }

        case tcNumber:
            runStack.Push(pToken->Type() == tyInteger ?
                    (float) pToken->Value().integer : pToken->Value().real);

            GetToken();
            break;

        case tcLParen:
            GetToken();
            ParseExpression();

            if (token == tcRParen) GetToken();
            else Error(errMissingRightParen);

            break;
        default:
            Error(errInvalidExpression);
            break;
    }
}
