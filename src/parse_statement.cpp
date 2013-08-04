#include <cstdio>
#include "parser.h"
#include "common.h"


/** ParseStatement          Parse a statement.
 * 
 * @param pRoutineId : function in which this statement is executed.
 */
void TParser::ParseStatement(TSymtabNode *pRoutineId) {
    InsertLineMarker();

    switch (token) {
        case tcIdentifier: ParseDeclarationsOrAssignment(pRoutineId);

            break;
            // not a type but a cv-qualifier
        case tcConst:
            GetTokenAppend();
            ParseConstantDeclaration(pRoutineId);
            break;
        case tcEnum:
            GetTokenAppend();
            //            ParseEnumHeader(pRoutineId);
            break;
        case tcDo: ParseDO(pRoutineId);
            break;
        case tcWhile: ParseWHILE(pRoutineId);
            break;
        case tcIf: ParseIF(pRoutineId);
            break;
        case tcFor: ParseFOR(pRoutineId);
            break;
        case tcSwitch: ParseSWITCH(pRoutineId);
            break;
            //case tcCase:
            //case tcDefault:ParseCaseLabel(pRoutineId);
            //  break;
        case tcBreak: GetTokenAppend();
            break;
        case tcLBracket: ParseCompound(pRoutineId);
            break;
        case tcReturn: ParseRETURN(pRoutineId);
            break;
        case tcPound:
            GetTokenAppend();
            ParseDirective(pRoutineId);
            break;
    }

    if (token != tcEndOfFile) {
        Resync(tlStatementFollow, tlStatementStart);
    }
}

/** ParseStatementList      Parse a statement list until the
 *                          terminator token.
 * 
 * @param pRoutineId : function in which these statements are executed.
 * @param terminator : the token that terminates the list.
 */
void TParser::ParseStatementList(TSymtabNode* pRoutineId, TTokenCode terminator) {

    do {
        ParseStatement(pRoutineId);
        while (token == tcSemicolon)GetTokenAppend();
    } while ((token != terminator) && (token != tcEndOfFile));
}

/** ParseAssignment         Parse an assignment statement.
 * 
 * NOTE:
 *      Just calls ParseVariable; This is because expressions are fully 
 *      recursive.
 * 
 * @param pTargetId : ptr to target id's symbol table node
 * @return ptr to the pTargetId type object.
 */
TType *TParser::ParseAssignment(const TSymtabNode *pTargetId) {

    TType *pTargetType = ParseVariable(pTargetId);

    return pTargetType;
}

/** ParseDO     Parse do/while statement.
 * 
 *      do
 *        <statement>;
 *      while(<expression>);
 * 
 * @param pRoutineId : ptr to this statements function Id. 
 */
void TParser::ParseDO(TSymtabNode* pRoutineId) {


    int breakPoint = PutLocationMarker();
    GetTokenAppend(); //do

    ParseStatementList(pRoutineId, tcWhile);

    CondGetTokenAppend(tcWhile, errMissingWHILE);
    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    CheckBoolean(ParseExpression());

    CondGetTokenAppend(tcRParen, errMissingRightParen);

    FixupLocationMarker(breakPoint);
}

/** ParseWHILE          Parse while statement.
 * 
 *      while(<expression>)
 *            <statement>;
 * 
 * @param pRoutineId : ptr to this statements function Id. 
 */
void TParser::ParseWHILE(TSymtabNode* pRoutineId) {

    int breakPoint = PutLocationMarker();

    GetTokenAppend(); // while
    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    CheckBoolean(ParseExpression());

    CondGetTokenAppend(tcRParen, errMissingRightParen);

    ParseStatement(pRoutineId);

    FixupLocationMarker(breakPoint);
}

/** ParseIF             Parse if/else statements.
 * 
 *      if(<expression>)
 *         <statement>;
 *      else if (<expression>)
 *         <statement>;
 *      else 
 *         <statement>;
 * 
 * @param pRoutineId : ptr to this statements function Id. 
 */
void TParser::ParseIF(TSymtabNode* pRoutineId) {

    //--Append a placeholder location marker for where to go to if
    //--<expr> is false.  Remember the location of this placeholder
    //--so it can be fixed up below.
    int atFalseLocationMarker = PutLocationMarker();

    GetTokenAppend();
    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    CheckBoolean(ParseExpression());

    CondGetTokenAppend(tcRParen, errMissingRightParen);

    ParseStatement(pRoutineId);
    while (token == tcSemicolon) GetTokenAppend();

    FixupLocationMarker(atFalseLocationMarker);
    if (token == tcElse) {
        //--Append a placeholder location marker for the token that
        //--follows the IF statement.  Remember the location of this
        //--placeholder so it can be fixed up below.
        int atFollowLocationMarker = PutLocationMarker();

        GetTokenAppend();
        ParseStatement(pRoutineId);
        while (token == tcSemicolon) GetTokenAppend();

        FixupLocationMarker(atFollowLocationMarker);
    }
}

/** ParseFOR            Parse for statements.
 * 
 *      for(<statement>; <expression>; <expression>)
 *              <statement>;
 * 
 * @param pRoutineId : ptr to this statements function Id. 
 */
void TParser::ParseFOR(TSymtabNode* pRoutineId) {

    int breakPoint = PutLocationMarker();
    int statementMarker = PutLocationMarker();
    int conditionMarker = PutLocationMarker();
    int incrementMarker = PutLocationMarker();

    GetTokenAppend(); // for

    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    if (token != tcSemicolon) {
        // declaration would go here //
        ParseDeclarationsOrAssignment(pRoutineId);
        CondGetTokenAppend(tcSemicolon, errMissingSemicolon);
    } else GetTokenAppend();

    FixupLocationMarker(conditionMarker);
    if (token != tcSemicolon) {

        // expr 2
        CheckBoolean(ParseExpression());
        CondGetTokenAppend(tcSemicolon, errMissingSemicolon);
    } else GetTokenAppend();

    FixupLocationMarker(incrementMarker);
    if (token != tcRParen) {
        // expr 3
        ParseExpression();
    }

    CondGetTokenAppend(tcRParen, errMissingRightParen);
    FixupLocationMarker(statementMarker);
    ParseStatement(pRoutineId);
    FixupLocationMarker(breakPoint);

}

/** ParseSWITCH         Parse switch statements.
 * 
 *      switch(<expression>){
 *              case <const-expression>:
 *              default:
 *      }
 * 
 * NOTE:
 *      Broken/not implemented yet.
 * 
 * @param pRoutineId : ptr to this statements function Id. 
 */
void TParser::ParseSWITCH(TSymtabNode* pRoutineId) {

    GetTokenAppend();
    CondGetTokenAppend(tcLParen, errMissingLeftParen);

    TType *pExprType = ParseExpression()->Base();

    CondGetTokenAppend(tcRParen, errMissingRightParen);

    if ((pExprType != pIntegerType)
            && (pExprType != pCharType)
            && (pExprType->form != fcEnum)) {
        Error(errIncompatibleTypes);
    }

    ParseStatement(pRoutineId);

}

/// XXX fixme
void TParser::ParseCaseBranch(TSymtabNode* pRoutineId, const TType *pExprType) {
    // c switch easier to parse that Pascal???
}

/// XXX fixme
void TParser::ParseCaseLabel(TSymtabNode* pRoutineId, const TType *pExprType) {
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

    ParseStatementList(pRoutineId, tcBreak);
}

/** ParseCompound       Parse compounded statements.
 * 
 *      {
 *         <statements>;
 *      }
 * 
 * @param pRoutineId : ptr to this statements function Id. 
 */
void TParser::ParseCompound(TSymtabNode* pRoutineId) {
    GetTokenAppend();

    ParseStatementList(pRoutineId, tcRBracket);

    CondGetTokenAppend(tcRBracket, errMissingRightBracket);

}

/** ParseRETURN         Parse return statements.
 * 
 *      return;
 *      or
 *      return <expression>;
 * 
 * @param pRoutineId : ptr to this statements function Id. 
 */
void TParser::ParseRETURN(TSymtabNode* pRoutineId) {
    GetTokenAppend();

    // expr 1
    CheckAssignmentTypeCompatible(pRoutineId->pType, ParseExpression(),
            errIncompatibleTypes);
}