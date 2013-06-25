#include <cstdio>
#include "parser.h"
#include "common.h"

void TParser::ParseExpression(void) {
    ParseSimpleExpression();

    if (TokenIn(token, tlRelOps)) {
        GetTokenAppend();
        ParseSimpleExpression();
    }

    Resync(tlExpressionFollow, tlStatementFollow, tlStatementStart);
}

void TParser::ParseSuffix(TSymtabNode *pNode) {
    if (token == tcPlusPlus) {
        GetTokenAppend();
    } else if (token == tcMinusMinus) {
        GetTokenAppend();
    }
}

void TParser::ParseSimpleExpression(void) {

    if (TokenIn(token, tlUnaryOps)) {
        GetTokenAppend();
    }

    ParseTerm();

    while (TokenIn(token, tlAddOps)) {
        GetTokenAppend();
        ParseTerm();
    }
}

void TParser::ParseTerm(void) {

    ParseFactor();

    while (TokenIn(token, tlMulOps)) {
        GetTokenAppend();
        ParseFactor();
    }
}

void TParser::ParseFactor(void) {
    switch (token) {
        case tcIdentifier:
        {
            TSymtabNode *pNode = SearchAll(pToken->String());
            if (pNode) {
                icode.Put(pNode);
                GetTokenAppend();

                ParseSuffix(pNode);

            } else {
                Error(errUndefinedIdentifier);
                EnterLocal(pToken->String());
                GetTokenAppend();
            }
        }
            break;


        case tcNumber:
        {
            TSymtabNode *pNode = SearchAll(pToken->String());
            if (!pNode) {
                pNode = EnterLocal(pToken->String());
                pNode->value = pToken->Type() == tyInteger ?
                        (float) pToken->Value().integer : pToken->Value().real;
            }

            icode.Put(pNode);
            GetTokenAppend();
        }
            break;

        case tcString:
            GetTokenAppend();
            break;

        case tcLParen:
            GetTokenAppend();
            ParseExpression();

            if (token == tcRParen) GetTokenAppend();
            else Error(errMissingRightParen);
            break;
        case tcLogicNOT:
            GetTokenAppend();
            ParseFactor();
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
                GetTokenAppend();

                sprintf(::list.text, "\t>> %s == %g", pNode->String(), pNode->value);
                ::list.PutLine();
            }
        }
            break;
        case tcInt:
            GetTokenAppend();
            break;
        case tcShort:
            GetTokenAppend();
            break;
        case tcBool:
            GetTokenAppend();
            break;
        case tcDouble:
            GetTokenAppend();
            break;
        case tcLong:
            GetTokenAppend();
            break;
        case tcChar:
            GetTokenAppend();
            break;
        case tcChar16_t:
            GetTokenAppend();
            break;
        case tcChar32_t:
            GetTokenAppend();
            break;
        case tcFloat:
            GetTokenAppend();
            break;
        case tcLParen:
            GetTokenAppend();
            ParseSizeOf();

            if (token == tcRParen) GetTokenAppend();
            else Error(errMissingRightParen);

            break;
    }
}
