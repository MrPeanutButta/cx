#include <cstdio>
#include "parser.h"
#include "common.h"
#include "types.h"

/** ParseExpression     Parse an expression (binary relational
 *                      operators = < > <> <= and >= ).
 * 
 * @return ptr to the expression's type object.
 */
TType *TParser::ParseExpression(void) {

    TType *pResultType;
    TType *pOperandType;

    pResultType = ParseSimpleExpression();

    if (TokenIn(token, tlRelOps)) {
        GetTokenAppend();
        pOperandType = ParseSimpleExpression();
        CheckRelOpOperands(pResultType, pOperandType);
        pResultType = pBooleanType;
    }

    Resync(tlExpressionFollow, tlStatementFollow, tlStatementStart);

    return pResultType;
}

/** ParseSimpleExpression       Parse a simple expression
 *                              (unary operators + or - , and
 *                              binary operators + - and ||).
 * 
 * @return ptr to the simple expression's type object.
 */
TType *TParser::ParseSimpleExpression(void) {

    TType *pResultType;
    TType *pOperandType;
    TTokenCode op;
    bool unaryOpFlag = false;

    if (TokenIn(token, tlUnaryOps)) {
        unaryOpFlag = true;
        GetTokenAppend();
    }

    pResultType = ParseTerm();

    if (unaryOpFlag) CheckIntegerOrReal(pResultType);

    while (TokenIn(token, tlAddOps)) {
        op = token;
        GetTokenAppend();
        pOperandType = ParseTerm();

        switch (op) {
            case tcPlus:
            case tcMinus:
                if (IntegerOperands(pResultType, pOperandType)) {
                    pResultType = pIntegerType;
                } else if (RealOperands(pResultType, pOperandType)) {
                    pResultType = pFloatType;
                } else Error(errIncompatibleTypes);
                break;
            case tcLogicOr:
                CheckBoolean(pResultType, pOperandType);
                pResultType = pBooleanType;
                break;
        }
    }

    return pResultType;
}

/** ParseTerm           Parse a term (binary operators * / 
 *                      % and &&).
 * 
 * @return ptr to the term's type object.
 */
TType *TParser::ParseTerm(void) {

    TType *pResultType;
    TType *pOperandType;
    TTokenCode op;

    pResultType = ParseFactor();

    while (TokenIn(token, tlMulOps)) {

        op = token;
        GetTokenAppend();
        pOperandType = ParseFactor();

        switch (op) {
            case tcStar:
                if (IntegerOperands(pResultType, pOperandType)) {
                    pResultType = pIntegerType;
                } else if (RealOperands(pResultType, pOperandType)) {
                    pResultType = pFloatType;
                } else Error(errIncompatibleTypes);
                break;
            case tcForwardSlash:
                if (IntegerOperands(pResultType, pOperandType) ||
                        RealOperands(pResultType, pOperandType)) {
                    pResultType = pIntegerType;
                } else Error(errIncompatibleTypes);
                break;
            case tcMod:
                if (IntegerOperands(pResultType, pOperandType)) {
                    pResultType = pIntegerType;
                } else Error(errIncompatibleTypes);
                break;
            case tcLogicAnd:
                CheckBoolean(pResultType, pOperandType);
                pResultType = pBooleanType;
                break;
        }
    }

    return pResultType;
}

/** ParseFactor         Parse a factor (identifier, number,
 *                      string, ! <factor>, or parenthesized
 *                      subexpression).
 * 
 * @return ptr to the factor's type object.
 */
TType *TParser::ParseFactor(void) {

    TType *pResultType;

    switch (token) {
        case tcIdentifier:
        {
            TSymtabNode *pNode = SearchAll(pToken->String());

            if (pNode == nullptr)
                Error(errUndefinedIdentifier);

            icode.Put(pNode);

            switch (pNode->defn.how) {
                case dcFunction:
                    GetTokenAppend();
                    //CondGetTokenAppend(tcLParen, ::errMissingLeftParen);
                    pResultType = ParseSubroutineCall(pNode, true);
                    break;
                case dcConstant:
                    GetTokenAppend();
                    pResultType = pNode->pType;
                    break;

                case dcType:
                    GetTokenAppend();
                    pResultType = pNode->pType;
                    break;
                case dcVariable:
                case dcValueParm:
                case dcReference:
                case dcMember:
                    GetTokenAppend();
                    pResultType = ParseVariable(pNode);
                    break;
                default:
                    Error(errUndefinedIdentifier);
                    break;

            }
        }
            break;
        case tcNumber:
        {
            TSymtabNode *pNode = SearchAll(pToken->String());

            if (!pNode) {
                pNode = EnterLocal(pToken->String());

                if (pToken->Type() == tyInteger) {
                    pNode->pType = pIntegerType;
                    pNode->defn.constant.value.__int = pToken->Value().__int;
                } else {
                    pNode->pType = pFloatType;
                    pNode->defn.constant.value.__float = pToken->Value().__float;
                }
            }

            pResultType = pNode->pType;
            icode.Put(pNode);
        }
            GetTokenAppend();
            break;

        case tcChar:
        case tcString:
        {

            char *pString = pToken->String();
            TSymtabNode *pNode = SearchAll(pToken->String());

            if (!pNode) {
                pNode = EnterLocal(pToken->String());


                //pString = pNode->String();
                //int length = strlen(pString) - 2;
                int length = pNode->strLength = strlen(pString) - 2;
                //pResultType = length == 1 ?
                // pCharType : new TType(length);

                SetType(pNode->pType, pCharType);

                if (length == 1) {
                    pNode->defn.constant.value.__char = pString[1];
                } else {
                    pNode->defn.constant.value.pString = &pString[1];
                    pNode->pType->form = fcArray;
                    pNode->pType->array.elmtCount = length;
                    pNode->pType->array.maxIndex = (pNode->pType->array.elmtCount - 1);
                    pNode->pType->array.minIndex = 0;
                    pNode->pType->array.pElmtType = pCharType;
                    pNode->pType->array.pIndexType = pIntegerType;
                }

                pResultType = pNode->pType;
            }
            icode.Put(pNode);

            GetTokenAppend();
        }
            break;

        case tcLParen:
            GetTokenAppend();
            pResultType = ParseExpression();

            CondGetTokenAppend(tcRParen, errMissingRightParen);
            break;
        case tcLogicNOT:
            GetTokenAppend();
            CheckBoolean(ParseFactor());
            pResultType = pBooleanType;

            break;
        case tcSemicolon:
            //pResultType = pDummyType;
            break;
        default:
            Error(errInvalidExpression);
            pResultType = pDummyType;
            break;
    }

    return pResultType;
}

/** ParseVariable       Parse variable type, and assignment operators (= -- ++
 *                      += -= *= /= %= <<= >>= &= ^= |=).
 *                      Also parsed ([] and .).
 * 
 * @param pId : variable node id.
 * @return variables type object ptr.
 */
TType *TParser::ParseVariable(const TSymtabNode* pId) {
    TType *pResultType = pId->pType;

    switch (pId->defn.how) {
        case dcVariable:
        case dcValueParm:
        case dcReference:
        case dcPointer:
        case dcFunction:
        case dcUndefined:
            break;

        default:
            pResultType = pDummyType;
            Error(errInvalidIdentifierUsage);
            break;
    }

    //-- [ or . : Loop to parse any subscripts and fields.
    int doneFlag = false;
    do {
        switch (token) {

            case tcLeftSubscript:
                pResultType = ParseSubscripts(pResultType);
                break;

            case tcDot:
                pResultType = ParseField(pResultType);
                break;

            default: doneFlag = true;
        }
    } while (!doneFlag);

    if (TokenIn(token, tlAssignOps)) {
        TType *pExprType = nullptr;
        TType *pExprTypeCastFollow = nullptr;

        switch (token) {
            case tcEqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();

                // if not semicolon, this may be an expression following a cast.
                // Keep pExprType same type, but we need to parse the expr.
                if (token != tcSemicolon) {
                    pExprTypeCastFollow = ParseExpression();

                    // check if compatible
                    CheckAssignmentTypeCompatible(pExprType, pExprTypeCastFollow,
                            errIncompatibleAssignment);
                }

                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcMinusMinus:
                GetTokenAppend();
                break;
            case tcPlusPlus:
                GetTokenAppend();
                break;
            case tcPlusEqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcMinusEqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcStarEqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcForwardSlashEqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcModEqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcBitLeftShiftEqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcBitRightShiftEqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcBitANDEqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcBitXOREqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcBitOREqual:
            {
                GetTokenAppend();
                pExprType = ParseExpression();
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            }
                break;
            case tcComma:
            case tcSemicolon:
                break;
                break;
            case tcIdentifier:
                GetTokenAppend();
                pExprType = pResultType;
                CheckAssignmentTypeCompatible(pResultType, pExprType,
                        errIncompatibleAssignment);
            default:
                Error(errInvalidAssignment);
                break;
        }
    }

    while (TokenIn(token, tlSubscriptOrFieldStart)) {
        pResultType = token == tcLeftSubscript ? ParseSubscripts(pResultType)
                : ParseField(pResultType);
    }

    return pResultType;
}

/** ParseSubscripts     Parse a bracketed list of subscripts
 *                      following an array variable:
 *
 *                          [ <expr> ]
 * 
 * @param pType : ptr to the array's type object.
 * @return ptr to the array element's type object.
 */
TType *TParser::ParseSubscripts(const TType* pType) {
    do {
        GetTokenAppend();

        if (pType->form == fcArray) {
            CheckAssignmentTypeCompatible(pType->array.pIndexType,
                    ParseExpression(),
                    errIncompatibleTypes);

            pType = pType->array.pElmtType;
        } else {
            Error(errTooManySubscripts);
            ParseExpression();
        }

    } while (token == tcComma);

    CondGetTokenAppend(tcRightSubscript, errMissingRightSubscript);

    return (TType *) pType;
}

/** ParseField          Parse a field following a record
 *                      variable:
 *
 *                          . <id>
 * 
 * @param pType : ptr to the record's type object
 * @return ptr to the field's type object.
 */
TType *TParser::ParseField(const TType* pType) {
    GetTokenAppend();

    if ((token == tcIdentifier) && (pType->form == fcComplex)) {
        //  TSymtabNode *pFieldId = pType->complex.//->Search(pToken->String());

        // if (!pFieldId) Error(errInvalidField);

        //  icode.Put(pFieldId);

        GetTokenAppend();

        // return pFieldId ? pFieldId->pType : pDummyType;
    } else {

        Error(errInvalidField);
        GetTokenAppend();
        return pDummyType;

    }
}