#include <cstdio>
#include "parser.h"

void TParser::ParseExpression(void) {
    ParseSimpleExpression();
}

void TParser::ParseSuffix(TSymtabNode *pNode) {
    if (token == tcPlusPlus) {
        runStack.Push(runStack.Pop() + 1);
        ++pNode->value;
        GetToken();
    } else if (token == tcMinusMinus) {
        runStack.Push(runStack.Pop() - 1);
        --pNode->value;
        GetToken();
    }
}

void TParser::ParseSimpleExpression(void) {
    TTokenCode op; //binary operator
    TTokenCode unaryOp = tcPlus; //unary operator

    if ((token == tcMinus) || (token == tcPlus) ||
            (token == tcBitNOT) || (token == tcLogicNOT)) {
        unaryOp = token;
        GetToken();
    }

    ParseTerm();
    if (unaryOp == tcMinus) runStack.Push(-runStack.Pop());
    if (unaryOp == tcBitNOT) runStack.Push(~((int) runStack.Pop()));
    if (unaryOp == tcLogicNOT) runStack.Push(!((int) runStack.Pop()));

    while ((token == tcPlus) || (token == tcMinus) ||
            (token == tcBitLeftShift) || (token == tcBitRightShift) ||
            (token == tcBitANDorAddrOf) || (token == tcBitXOR) ||
            (token == tcBitOR)) {

        op = token;

        GetToken();
        ParseTerm();

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
        }
    }
}

void TParser::ParseTerm(void) {
    TTokenCode op; // binary operator

    ParseFactor();

    while ((token == tcStar) || (token == tcForwardSlash) ||
            (token == tcMod)) {

        op = token;

        GetToken();
        ParseFactor();

        float __2 = runStack.Pop();
        float __1 = runStack.Pop();

        if (op == tcStar)runStack.Push(__1 * __2);
        else if (op == tcForwardSlash) {
            if (__2 != 0.0) runStack.Push(__1 / __2);
            else {
                ::list.PutLine("runtime error: division by 0");
                runStack.Push(0.0);
            }
        } else if (op == tcMod) runStack.Push((int) __1 % (int) __2);
    }
}

void TParser::ParseFactor(void) {
    switch (token) {
        case tcIdentifier:
        {
            TSymtabNode *pNode = SearchAll(pToken->String());
            if (pNode) {
                runStack.Push(pNode->value);
                GetToken();

                ParseSuffix(pNode);

                sprintf(::list.text, "\t>> %s == %g", pNode->String(), pNode->value);
                ::list.PutLine();
            } else {
                Error(errUndefinedIdentifier);
                runStack.Push(0.0);
                GetToken();
            }
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
        case tcSizeOf:
            GetToken();
            ParseSizeOf();
            break;
        case tcSemicolon:
            break;
        default:
            Error(errInvalidExpression);
            break;
    }
}

void TParser::ParseSizeOf(void) {
    switch (token) {
        case tcIdentifier:
        {
            TSymtabNode *pNode = SearchAll(pToken->String());
            if (pNode) {
                runStack.Push(sizeof (pNode->value));
                GetToken();


                sprintf(::list.text, "\t>> %s == %g", pNode->String(), pNode->value);
                ::list.PutLine();
            }
        }
            break;
        case tcInt:
            runStack.Push(sizeof (int));
            GetToken();
            break;
        case tcShort:
            runStack.Push(sizeof (short));
            GetToken();
            break;
        case tcBool:
            runStack.Push(sizeof (bool));
            GetToken();
            break;
        case tcDouble:
            runStack.Push(sizeof (double));
            GetToken();
            break;
        case tcLong:
            runStack.Push(sizeof (long));
            GetToken();
            break;
        case tcChar:
            runStack.Push(sizeof (char));
            GetToken();
            break;
        case tcChar16_t:
            runStack.Push(sizeof (char16_t));
            GetToken();
            break;
        case tcChar32_t:
            runStack.Push(sizeof (char32_t));
            GetToken();
            break;
        case tcFloat:
            runStack.Push(sizeof (float));
            GetToken();
            break;
        case tcLParen:
            GetToken();
            ParseSizeOf();

            if (token == tcRParen) GetToken();
            else Error(errMissingRightParen);

            runStack.Push(sizeof (runStack.Pop()));
            break;
    }
}
