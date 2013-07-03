#include "common.h"
#include "parser.h"

void TParser::ParseTypeDefinitions(TSymtabNode *pRoutineId) {
    TSymtabNode *pLastId = nullptr;

    while (token == tcIdentifier) {
        TSymtabNode *pTypeId = EnterNewLocal(pToken->String());

        if (!pRoutineId->defn.routine.locals.pTypeIds) {
            pRoutineId->defn.routine.locals.pTypeIds = pTypeId;
        } else {
            pLastId->next = pTypeId;
        }

        pLastId = pTypeId;

        GetToken();
        CondGetToken(tcEqual, errMissingEqual);

        SetType(pTypeId->pType, ParseTypeSpec());
        pTypeId->defn.how = dcType;

        if (!pTypeId->pType->pTypeId) {
            pTypeId->pType->pTypeId = pTypeId;
        }

        Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);
        CondGetToken(tcSemicolon, errMissingSemicolon);

        while (token == tcSemicolon)GetToken();

        Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);

    }
}

TType *TParser::ParseTypeSpec(void) {
    switch (token) {
        case tcIdentifier:
        {
            TSymtabNode *pId = Find(pToken->String());

            switch (pId->defn.how) {
                case dcType: return ParseIdentifierType(pId);
                case dcConstant: return ParseSubrangeType(pId);
                default:
                    Error(errNotATypeIdentifier);
                    GetToken();
                    return (pDummyType);
            }
        }

        case tcLBracket: return ParseEnumerationType();
        case tcArray: return ParseArrayType();
        case tcRecord: return ParseRecordType();
        case tcPlus:
        case tcMinus:
        case tcNumber:
        case tcChar:
        case tcString: return ParseSubrangeType(nullptr);

        default:
            Error(errInvalidType);
            return (pDummyType);

    }
}

TType *TParser::ParseIdentifierType(const TSymtabNode *pId2) {
    GetToken();
    return pId2->pType;
}

TType *TParser::ParseEnumerationType(void) {
    TType *pType = new TType(fcEnum, sizeof (int), nullptr);
    TSymtabNode *pLastId = nullptr;

    int constValue = -1;

    GetToken();
    Resync(tlEnumConstStart);

    while (token == tcIdentifier) {
        TSymtabNode *pConstId = EnterNewLocal(pToken->String());
        ++constValue;

        if (pConstId->defn.how == dcUndefined) {
            pConstId->defn.how = dcConstant;

            pConstId->defn.constant.value.integer = constValue;
            SetType(pConstId->pType, pType);

            if (!pLastId) {
                pType->enumeration.pConstIds = pLastId = pConstId;
            } else {
                pLastId->next = pConstId;
                pLastId = pConstId;
            }
        }

        GetToken();
        Resync(tlEnumConstFollow);

        if (token == tcComma) {

            do {
                GetToken();
                Resync(tlEnumConstStart, tlEnumConstFollow);
                if (token == tcComma) Error(errMissingIdentifier);
            } while (token == tcComma);
            if (token != tcIdentifier) Error(errMissingIdentifier);
        } else if (token == tcIdentifier) Error(errMissingComma);
    }

    CondGetToken(tcRBracket, errMissingRightBracket);

    pType->enumeration.max = constValue;
    return pType;
}

TType *TParser::ParseSubrangeType(TSymtabNode* pMinId) {
    TType *pType = new TType(fcSubrange, 0, nullptr);

    SetType(pType->subrange.pBaseType, ParseSubrangeLimit(pMinId, pType->subrange.min));

    Resync(tlSubrangeLimitFollow, tlDeclarationStart);
    CondGetToken(tcDotDot, errMissingDotDot);

    TType *pMaxType = ParseSubrangeLimit(nullptr, pType->subrange.max);

    if (pMaxType != pType->subrange.pBaseType) {
        Error(errIncompatibleTypes);
        pType->subrange.min = pType->subrange.max = 0;
    } else if (pType->subrange.min > pType->subrange.max) {
        Error(errMinGtMax);

        int temp = pType->subrange.min;
        pType->subrange.min = pType->subrange.max;
        pType->subrange.max = temp;
    }

    pType->size = pType->subrange.pBaseType->size;
    return pType;
}

TType *TParser::ParseSubrangeLimit(TSymtabNode* pLimitId, int& limit) {
    TType *pType = pDummyType;
    TTokenCode sign = tcDummy;

    limit = 0;

    if (TokenIn(token, tlUnaryOps)) {
        if (token == tcMinus) sign = tcMinus;
        GetToken();
    }

    switch (token) {
        case tcNumber:
            if (pToken->Type() == tyInteger) {
                limit = sign == tcMinus ?
                        -pToken->Value().integer :
                        pToken->Value().integer;

                pType = pIntegerType;

            } else Error(errInvalidSubrangeType);
            break;
        case tcIdentifier:
            if (!pLimitId) pLimitId = Find(pToken->String());

            if (pLimitId->defn.how == dcUndefined) {
                pLimitId->defn.how = dcConstant;
                pType = SetType(pLimitId->pType, pDummyType);
                break;
            } else if ((pLimitId->pType == pRealType) ||
                    (pLimitId->pType == pDummyType) ||
                    (pLimitId->pType->form == fcArray)) {
                Error(errInvalidSubrangeType);
            } else if (pLimitId->defn.how == dcConstant) {

                if (pLimitId->pType == pIntegerType) {
                    limit = sign == tcMinus
                            ? -pLimitId->defn.constant.value.integer
                            : pLimitId->defn.constant.value.integer;
                } else if (pLimitId->pType == pCharType) {
                    if (sign != tcDummy) Error(errInvalidConstant);
                    limit = pLimitId->defn.constant.value.character;
                } else if (pLimitId->pType->form == fcEnum) {
                    if (sign != tcDummy) Error(errInvalidConstant);
                    limit = pLimitId->defn.constant.value.integer;
                }
                pType = pLimitId->pType;
            } else Error(errNotAConstantIdentifier);

            break;

        case tcChar:
        case tcString:
            if (sign != tcDummy) Error(errInvalidConstant);

            if (strlen(pToken->String()) != 3) {
                Error(errInvalidSubrangeType);
            }

            limit = pToken->String()[1];
            pType = pCharType;
            break;

        default:
            Error(errMissingConstant);
            return pType;
    }

    GetToken();
    return pType;
}

