#include <cstdio>
#include "parser.h"
#include "common.h"

void TParser::ParseStatement(void) {
    InsertLineMarker();

    switch (token) {
        case tcIdentifier: ParseAssignment();
            break;
        case tcDo: ParseDO();
            break;
        case tcWhile: ParseWHILE();
            break;
        case tcIf: ParseIF();
            break;
        case tcFor: ParseFOR();
            break;
        case tcSwitch: ParseSWITCH();
            break;
        case tcCase:
        case tcDefault:ParseCaseLabel();
            break;
        case tcBreak: GetTokenAppend();
            break;
        case tcLBracket: ParseCompound();
            break;
        case tcReturn: ParseRETURN();
            break;
    }

    if (token != tcEndOfFile) {
        Resync(tlStatementFollow, tlStatementStart);
    }
}

void TParser::ParseStatementList(TTokenCode terminator) {

    do {
        ParseStatement();

        //if (TokenIn(token, tlStatementStart)) {
        //  Error(errMissingSemicolon);
        //} else
        while (token == tcSemicolon)GetTokenAppend();
    } while ((token != terminator) && (token != tcEndOfFile));
}

void TParser::ParseAssignment(void) {
    TSymtabNode *pTargetNode = SearchAll(pToken->String());

    if (!pTargetNode) pTargetNode = EnterLocal(pToken->String());

    icode.Put(pTargetNode);
    GetTokenAppend();

    switch (token) {
        case tcEqual:
            //Resync(tcEqual, tlExpressionStart);
            //CondGetTokenAppend(tcEqual, errMissingEqual);
            GetTokenAppend();
            ParseExpression();
            break;
        case tcPlusPlus:
            //Resync(tcPlusPlus, tlExpressionFollow);
            GetTokenAppend();
            break;
        case tcMinusMinus:
            //Resync(tcMinusMinus, tlExpressionFollow);
            GetTokenAppend();
            break;
        case tcPlusEqual:
            //Resync(tcPlusEqual, tlExpressionStart);
            GetTokenAppend();
            ParseExpression();
            break;
        case tcMinusEqual:
            //Resync(tcMinusEqual, tlExpressionStart);
            GetTokenAppend();
            ParseExpression();
            break;
        case tcStarEqual:
            //Resync(tcStarEqual, tlExpressionStart);
            GetTokenAppend();
            ParseExpression();
            break;
        case tcForwardSlashEqual:
            //Resync(tcForwardSlashEqual, tlExpressionStart);
            GetTokenAppend();
            ParseExpression();
            break;
        case tcModEqual:
            //Resync(tcModEqual, tlExpressionStart);
            GetTokenAppend();
            ParseExpression();
            break;
        case tcBitLeftShiftEqual:
            //Resync(tcBitLeftShiftEqual, tlExpressionStart);
            GetTokenAppend();
            ParseExpression();
            break;
        case tcBitRightShiftEqual:
            //Resync(tcBitRightShiftEqual, tlExpressionStart);
            GetTokenAppend();
            ParseExpression();
            break;
        case tcBitANDEqual:
            //Resync(tcBitANDEqual, tlExpressionStart);
            GetTokenAppend();
            ParseExpression();
            break;
        case tcBitXOREqual:
            //Resync(tcBitXOREqual, tlExpressionStart);
            GetTokenAppend();
            ParseExpression();
            break;
        case tcBitOREqual:
            //Resync(tcBitOREqual, tlExpressionStart);
            GetTokenAppend();
            ParseExpression();
            break;
        default:
            Error(errInvalidAssignment);
            break;
    }
}

void TParser::ParseDO(void) {
    GetTokenAppend();

    ParseStatementList(tcWhile);

    CondGetTokenAppend(tcWhile, errMissingWHILE);
    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    InsertLineMarker();
    ParseExpression();

    CondGetTokenAppend(tcRParen, errMissingRightParen);

}

void TParser::ParseWHILE(void) {
    GetTokenAppend();
    ParseExpression();

    CondGetTokenAppend(tcLBracket, errMissingLeftBracket);

    ParseStatement();
}

void TParser::ParseIF(void) {

    GetTokenAppend();
    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    ParseExpression();

    CondGetTokenAppend(tcRParen, errMissingRightParen);

    ParseStatement();

    if (token == tcSemicolon) GetTokenAppend();

    if (token == tcElse) {
        GetTokenAppend();
        ParseStatement();
    }
}

void TParser::ParseFOR(void) {

    GetTokenAppend();
    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    if ((token == tcIdentifier) && (!SearchAll(pToken->String()))) {
        Error(errUndefinedIdentifier);
    }

    if (token != tcSemicolon) {

        // declaration would go here //

        CondGetTokenAppend(tcIdentifier, errMissingIdentifier);

        //Resync(tcEqual, tlExpressionStart);
        CondGetTokenAppend(tcEqual, errMissingEqual);
        // expr 1
        ParseExpression();

        CondGetTokenAppend(tcSemicolon, errMissingSemicolon);
    } else GetTokenAppend();


    if (token != tcSemicolon) {

        // expr 2
        ParseExpression();

        CondGetTokenAppend(tcSemicolon, errMissingSemicolon);
    } else GetTokenAppend();

    if (token != tcRParen) {
        // expr 3
        ParseExpression();

    }

    CondGetTokenAppend(tcRParen, errMissingRightParen);

    ParseStatement();
}

void TParser::ParseSWITCH(void) {

    GetTokenAppend();
    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    ParseExpression();

    CondGetTokenAppend(tcRParen, errMissingRightParen);

    ParseStatement();

}

void TParser::ParseCaseBranch(void) {
    // c switch easier to parse that Pascal???
}

void TParser::ParseCaseLabel(void) {
    GetTokenAppend();

    bool signFlag(false);

    if (TokenIn(token, tlUnaryOps)) {
        signFlag = true;
        GetTokenAppend();
    }

    switch (token) {
        case tcIdentifier:
            if (!SearchAll(pToken->String())) {
                Error(errUndefinedIdentifier);
            }

            GetTokenAppend();
            break;
        case tcNumber:
            if (pToken->Type() != tyInteger) Error(errInvalidConstant);
            GetTokenAppend();
            break;
        case tcString:
            if (signFlag || (strlen(pToken->String()) != 3)) {
                Error(errInvalidConstant);
            }
    }

    CondGetTokenAppend(tcColon, errMissingColon);

    ParseStatementList(tcBreak);
}

void TParser::ParseCompound(void) {
    GetTokenAppend();

    ParseStatementList(tcRBracket);

    CondGetTokenAppend(tcRBracket, errMissingRightBracket);

}

void TParser::ParseRETURN(void) {
    GetTokenAppend();

    ParseExpression();
}