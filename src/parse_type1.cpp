#include "common.h"
#include "parser.h"

/** ParseTypeDefinitions    Parse a list of type definitions
 *                          separated by commas:
 *
 *                              <type> <id>, ... ;
 * 
 * @param pRoutineId : ptr to symbol table node of program,
 *                     procedure, or function identifier.
 */
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

        GetTokenAppend();
        CondGetToken(tcEqual, errMissingEqual);

        //        SetType(pTypeId->pType, ParseTypeSpec());
        pTypeId->defn.how = dcType;

        if (!pTypeId->pType->pTypeId) {
            pTypeId->pType->pTypeId = pTypeId;
        }

        Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);
        CondGetToken(tcSemicolon, errMissingSemicolon);

        while (token == tcSemicolon)GetTokenAppend();

        Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);

    }
}

/** ParseTypeSpec       Parse a type specification.
 * 
 * @param pNode : ptr to object node.
 * @return ptr to type object.
 */
TType *TParser::ParseTypeSpec(TSymtabNode *pNode) {
    switch (token) {
        case tcIdentifier:
        {
            TSymtabNode *pId = Find(pToken->String());

            switch (pId->defn.how) {
                case dcType: return ParseIdentifierType(pId);
                case dcConstant: return ParseSubrangeType(pId);
                default:
                    Error(errNotATypeIdentifier);
                    GetTokenAppend();
                    return (pDummyType);
            }
        }

        case tcLBracket: return ParseEnumerationType();

            /* found empty subscript.
             * array must have initializer */
        case tcRightSubscript: return ParseAssignment(pNode);
            //        case tcRecord: return ParseComplexType();
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

/** ParseIdentifierType     In a type defintion of the form
 *
 *                               <id-1> = <id-2>
 *
 *                          parse <id-2>.
 * 
 * @param pId2 : ptr to symbol table node of <id-2>.
 * @return ptr to type object of <id-2>.
 */
TType *TParser::ParseIdentifierType(const TSymtabNode *pId2) {
    GetTokenAppend();
    return pId2->pType;
}

/** ParseEnumerationType    Parse a enumeration type
 *                          specification:
 *      enum <id> { <enum-list> };
 *  
 * @return  
 */
TType *TParser::ParseEnumerationType(void) {
    TType *pType = new TType(fcEnum, sizeof (int), nullptr);
    TSymtabNode *pLastId = nullptr;

    int constValue = -1;

    GetTokenAppend();
    Resync(tlEnumConstStart);

    while (token == tcIdentifier) {
        TSymtabNode *pConstId = EnterNewLocal(pToken->String());
        ++constValue;

        if (pConstId->defn.how == dcUndefined) {
            pConstId->defn.how = dcConstant;

            pConstId->defn.constant.value.__int = constValue;
            SetType(pConstId->pType, pType);

            if (!pLastId) {
                pType->enumeration.pConstIds = pLastId = pConstId;
            } else {
                pLastId->next = pConstId;
                pLastId = pConstId;
            }
        }

        GetTokenAppend();
        Resync(tlEnumConstFollow);

        if (token == tcComma) {

            do {
                GetTokenAppend();
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

/** ParseSubrangeType       Parse a subrange type specification.
 * 
 * NOTE:
 *      Not implemented.
 * 
 * @param pMinId
 * @return 
 */
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

/** ParseSubrangeLimit      Parse a mininum or maximum limit
 *                          constant of a subrange type.
 * 
 * NOTE:
 *      Not implemented.
 * 
 * @param pLimitId
 * @param limit
 * @return 
 */
TType *TParser::ParseSubrangeLimit(TSymtabNode* pLimitId, int& limit) {
    TType *pType = pDummyType;
    TTokenCode sign = tcDummy;

    limit = 0;

    if (TokenIn(token, tlUnaryOps)) {
        if (token == tcMinus) sign = tcMinus;
        GetTokenAppend();
    }

    switch (token) {
        case tcNumber:
            if (pToken->Type() == tyInteger) {
                limit = sign == tcMinus ?
                        -pToken->Value().__int :
                        pToken->Value().__int;

                pType = pIntegerType;

            } else Error(errInvalidSubrangeType);
            break;
        case tcIdentifier:
            if (!pLimitId) pLimitId = Find(pToken->String());

            if (pLimitId->defn.how == dcUndefined) {
                pLimitId->defn.how = dcConstant;
                pType = SetType(pLimitId->pType, pDummyType);
                break;
            } else if ((pLimitId->pType == pFloatType) ||
                    (pLimitId->pType == pDummyType) ||
                    (pLimitId->pType->form == fcArray)) {
                Error(errInvalidSubrangeType);
            } else if (pLimitId->defn.how == dcConstant) {

                if (pLimitId->pType == pIntegerType) {
                    limit = sign == tcMinus
                            ? -pLimitId->defn.constant.value.__int
                            : pLimitId->defn.constant.value.__int;
                } else if (pLimitId->pType == pCharType) {
                    if (sign != tcDummy) Error(errInvalidConstant);
                    limit = pLimitId->defn.constant.value.__char;
                } else if (pLimitId->pType->form == fcEnum) {
                    if (sign != tcDummy) Error(errInvalidConstant);
                    limit = pLimitId->defn.constant.value.__int;
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

    GetTokenAppend();
    return pType;
}

