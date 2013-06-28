#include <cstdio>
#include "parser.h"
#include "common.h"
#include "types.h"

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

TType * TParser::ParseSuffix(TSymtabNode *pNode) {
    if (token == tcPlusPlus) {
        GetTokenAppend();
    } else if (token == tcMinusMinus) {
        GetTokenAppend();
    }
    
    return pNode->pType;
}

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
                    pResultType = pRealType;
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
                    pResultType = pRealType;
                } else Error(errIncompatibleTypes);
                break;
            case tcForwardSlash:
                if (IntegerOperands(pResultType, pOperandType) ||
                        RealOperands(pResultType, pOperandType)) {
                    pResultType = pRealType;
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

TType *TParser::ParseFactor(void) {

    TType *pResultType;

    switch (token) {
        case tcIdentifier:
        {
            TSymtabNode *pNode = SearchAll(pToken->String());
            if (pNode->defn.how != dcUndefined) {
                icode.Put(pNode);
                GetTokenAppend();

                if ((pNode->pType == pIntegerType) ||
                        (pNode->pType == pRealType))ParseSuffix(pNode);

            } else {
                pNode->defn.how = dcVariable;
                SetType(pNode->pType, pDummyType);
            }/*else {
                Error(errUndefinedIdentifier);
                EnterLocal(pToken->String());
                GetTokenAppend();
            }*/

            if (pNode->defn.how == dcConstant) {
                pResultType = pNode->pType;
                GetTokenAppend();
            } else pResultType = ParseVariable(pNode);
        }
            break;


        case tcNumber:
        {
            TSymtabNode *pNode = SearchAll(pToken->String());
            if (!pNode) {
                pNode = EnterLocal(pToken->String());

                if (pToken->Type() == tyInteger) {
                    pResultType = pIntegerType;
                    pNode->defn.constant.value.integer = pToken->Value().integer;
                } else {
                    pResultType = pRealType;
                    pNode->defn.constant.value.real = pToken->Value().real;
                }
                SetType(pNode->pType, pResultType);
            }

            icode.Put(pNode);

            pResultType = pNode->pType;
                    GetTokenAppend();
        }
            break;

        case tcString:
        {

            char *pString = pToken->String();
            TSymtabNode *pNode = SearchAll(pString);

            int length = strlen(pString) - 2;

            pResultType = length == 1 ?
                    pCharType : new TType(length);

            SetType(pNode->pType, pResultType);

            if (length == 1) {
                pNode->defn.constant.value.character = pString[1];
            } else {
                pNode->defn.constant.value.pString = &pString[1];
            }

            icode.Put(pNode);

            pResultType = pNode->pType;

            GetTokenAppend();
        }
            break;

        case tcLParen:
            GetTokenAppend();
            pResultType = ParseExpression();

            if (token == tcRParen) GetTokenAppend();
            else Error(errMissingRightParen);
            break;
        case tcLogicNOT:
            GetTokenAppend();
            CheckBoolean(ParseFactor());
            pResultType = pBooleanType;

            break;
        case tcSemicolon:
            pResultType = pDummyType;
            break;
        default:
            Error(errInvalidExpression);
            pResultType = pDummyType;
            break;
    }
    
    return pResultType;
}

TType *TParser::ParseVariable(const TSymtabNode* pId){
    TType *pResultType = pId->pType;
    
    switch(pId->defn.how){
        case dcVariable:
        case dcValueParm:
        case dcVarParm:
        case dcFunction:
        case dcUndefined: break;
        
        default:
            pResultType = pDummyType;
            Error(errInvalidIdentifierUsage);
            break;
    }
    
    GetTokenAppend();
    
    while(TokenIn(token, tlSubscriptOrFieldStart)){
        pResultType = token == tcLeftSubscript ? ParseSubscripts(pResultType)
                : ParseField(pResultType);
    }
    
    return pResultType;
}

TType *TParser::ParseSubscripts(const TType* pType){
    do{
        GetTokenAppend();
        
        if(pType->form == fcArray){
            CheckAssignmentTypeCompatible(pType->array.pIndexType,
                                          ParseExpression(),
                                          errIncompatibleTypes);
            
            pType = pType->array.pElmtType;
        } else {
            Error(errTooManySubscripts);
            ParseExpression();
        }
        
    }while(token == tcComma);
    
    CondGetTokenAppend(tcRightSubscript, errMissingRightSubscript);
    
    return (TType *)pType;
}

TType *TParser::ParseField(const TType* pType){
    GetTokenAppend();
    
    if((token == tcIdentifier) && (pType->form == fcRecord)){
        TSymtabNode *pFieldId = pType->record.pSymtab->Search(pToken->String());
    
        if(!pFieldId) Error(errInvalidField);
        
        icode.Put(pFieldId);
        
        GetTokenAppend();
        
        return pFieldId ? pFieldId->pType : pDummyType;
    } else {
        
        Error(errInvalidField);
        GetTokenAppend();
        return pDummyType;
                
    }
}

/*
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
}*/
