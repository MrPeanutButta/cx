#include <cstdio>
#include <cstring>
#include "common.h"
#include "parser.h"

bool execFlag(true);

void TParser::ParseDeclarationsOrAssignment(TSymtabNode *pRoutineId) {

    TSymtabNode *pNode = Find(pToken->String());

    // if complex then this is an object
    if (pNode->pType->form == fcComplex) {
        ParseComplexType(pRoutineId, pNode);
        // predefined type name found
    } else if ((pNode->defn.how == dcType) && (pNode->pType->form != fcComplex) &&
            (pNode->defn.how != dcFunction)) {

        GetToken();

        do {
            while (token == tcComma)GetTokenAppend();

            TSymtabNode *pNewId = nullptr;

            pNewId = SearchAll(pToken->String());

            /* if not nullptr, it's already defined.
             * check if forwarded */
            if (pNewId != nullptr) {
                if (pNewId->defn.how == dcFunction && pNewId->defn.routine.which == ::rcForward) {
                    ParseFunctionHeader(pNewId);
                } else Error(errRedefinedIdentifier);
            } else {
                pNewId = EnterNewLocal(pToken->String());
                icode.Put(pNewId);
            }

            // set type
            SetType(pNewId->pType, pNode->pType);

            GetTokenAppend();

            // check for array type
            if (token == tcLeftSubscript) {
                ParseArrayType(pNewId);
                pNewId->defn.how = dcVariable;
            } else if (token == tcLParen) {
                ParseFunctionHeader(pNewId);
            } else if ((token != tcComma) && (token != tcEndOfFile)) {
                // check for assignment
                ParseAssignment(pNewId);
                pNewId->defn.how = dcVariable;
            }

            if (pNewId->defn.how == dcVariable) {
                // add to routines variable list
                if (pRoutineId && (!pRoutineId->defn.routine.locals.pVariableIds)) {
                    pRoutineId->defn.routine.locals.pVariableIds = pNewId;
                    pNewId->defn.data.offset = pRoutineId->defn.routine.parmCount + 1;
                    pRoutineId->defn.routine.totalLocalSize = pNewId->pType->size;
                } else {
                    TSymtabNode *__var = pRoutineId->defn.routine.locals.pVariableIds;
                    int offset = pRoutineId->defn.routine.parmCount + 2;
                    int totalSize = 0;
                    
                    while(__var->next){
                        totalSize += __var->pType->size;
                        offset++;
                        __var = __var->next;
                    }
                    
                    __var->next = pNewId;
                    pNewId->defn.data.offset = offset;
                    pRoutineId->defn.routine.totalLocalSize = totalSize;

                }
            } else if (pNewId->defn.how == dcFunction) {
                if (pRoutineId && (!pRoutineId->defn.routine.locals.pRoutineIds)) {
                    pRoutineId->defn.routine.locals.pRoutineIds = pNewId;
                } else {
                    TSymtabNode *__fun = pRoutineId->defn.routine.locals.pRoutineIds;

                    while (__fun->next)
                        __fun = __fun->next;

                    __fun->next = pNewId;

                }
            }

        } while (token == tcComma);
    } else if (pNode->defn.how == dcFunction) {
        icode.Put(pNode);

        GetTokenAppend();

        if (token == tcLParen) {
            ParseSubroutineCall(pNode, true);
        } else {
            ParseAssignment(pNode);
        }

    } else {
        icode.Put(pNode);
        GetTokenAppend();
        ParseAssignment(pNode);
    }
}

/**
 * Setup a basic un-named const.
 * The following type keyword will search the AST for a
 * __un_const__ to rename and set it's type.
 * 'const' will only set it's qualifier as dcConstant.
 *
 * @param pRoutineId - AST node of this constant.
 */
void TParser::ParseConstantDeclaration(TSymtabNode* pRoutineId) {
    TSymtabNode *pLastId = nullptr;
    TSymtabNode *pConstId = nullptr;
    TSymtabNode *pTypeNode = Find(pToken->String());

    GetTokenAppend();

    // while (token == tcIdentifier) {
    pConstId = EnterNewLocal(pToken->String());

    if (!pRoutineId->defn.routine.locals.pConstantIds) {
        pRoutineId->defn.routine.locals.pConstantIds = pConstId;
    } else {

        pLastId = pRoutineId->defn.routine.locals.pConstantIds;

        while (pLastId->next)
            pLastId = pLastId->next;

        pLastId->next = pConstId;

    }
    //}

    GetTokenAppend();
    CondGetToken(tcEqual, errMissingEqual);

    SetType(pConstId->pType, pTypeNode->pType);
    ParseConstant(pConstId);
    pConstId->defn.how = dcConstant;


    Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);

    ///CondGetToken(tcSemicolon, errMissingSemicolon);

    //while (token == tcSemicolon) GetTokenAppend();
    Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);

}

void TParser::ParseConstant(TSymtabNode *pConstId) {
    TTokenCode sign = tcDummy;

    if (TokenIn(token, tlUnaryOps)) {
        if (token == tcMinus) sign = tcMinus;
        GetTokenAppend();
    }

    switch (token) {
        case tcNumber:
            if ((pToken->Type() == tyInteger) && (pConstId->pType == pIntegerType)) {
                pConstId->defn.constant.value.__int = sign == tcMinus ?
                        -pToken->Value().__int : pToken->Value().__int;
            } else if ((pToken->Type() == tyReal) &&
                    (((pConstId->pType == pFloatType) ||
                    pConstId->pType == pDoubleType))) {

                if (pConstId->pType == pFloatType) {
                    pConstId->defn.constant.value.__float = sign == tcMinus ?
                            -pToken->Value().__float : pToken->Value().__float;
                } else {
                    pConstId->defn.constant.value.__double = sign == tcMinus ?
                            -pToken->Value().__float : pToken->Value().__float;
                }

                //SetType(pConstId->pType, pFloatType);
            }

            GetTokenAppend();
            break;

        case tcIdentifier:
            ParseIdentifierConstant(pConstId, sign);
            break;
        case tcChar:
        case tcString:
            if (pConstId->pType == pCharType) {
                int length = strlen(pToken->String()) - 2;

                if (sign != tcDummy) Error(errInvalidConstant);

                if (length == 1) {
                    pConstId->defn.constant.value.__char = pToken->String()[1];

                    //SetType(pConstId->pType, pCharType);
                } else {
                    char *pString = new char[length];
                    CopyQuotedString(pString, pToken->String());

                    pConstId->defn.constant.value.pString = pString;

                    //SetType(pConstId->pType, new TType(length));
                }

                GetTokenAppend();
            } else Error(errInvalidType);
            break;
    }
}

void TParser::ParseIdentifierConstant(TSymtabNode* pId1, TTokenCode sign) {
    TSymtabNode *pId2 = Find(pToken->String());

    if (pId2->defn.how != dcConstant) {
        Error(errNotAConstantIdentifier);
        SetType(pId1->pType, pDummyType);
        GetTokenAppend();
        return;
    }

    if (pId2->pType == pIntegerType) {
        pId2->defn.constant.value.__int = sign == tcMinus ?
                -pId2->defn.constant.value.__int :
                pId2->defn.constant.value.__int;

        SetType(pId1->pType, pIntegerType);
    } else if (pId2->pType == pFloatType) {
        pId1->defn.constant.value.__float = sign == tcMinus ?
                -pId2->defn.constant.value.__float :
                pId2->defn.constant.value.__float;
        SetType(pId1->pType, pFloatType);
    } else if (pId2->pType == pCharType) {
        if (sign != tcDummy) Error(errInvalidConstant);

        pId1->defn.constant.value.__char = pId2->defn.constant.value.__char;

        SetType(pId1->pType, pCharType);
    } else if (pId2->pType->form == fcEnum) {
        if (sign != tcDummy)Error(errInvalidConstant);

        pId1->defn.constant.value.__int = pId2->defn.constant.value.__int;

        SetType(pId1->pType, pId2->pType);
    } else if (pId2->pType->form == fcArray) {
        if ((sign != tcDummy) || (pId2->pType->array.pElmtType != pCharType)) {
            Error(errInvalidConstant);
        }

        pId1->defn.constant.value.pString = pId2->defn.constant.value.pString;

        SetType(pId1->pType, pId2->pType);
    }

    GetTokenAppend();
}

TSymtabNode *TParser::ParseIdSublist(const TSymtabNode* pRoutineId,
        const TType* pComplexType,
        TSymtabNode*& pLastId) {

    TSymtabNode *pId;
    TSymtabNode *pFirstId = nullptr;

    pLastId = nullptr;

    while (token == tcIdentifier) {
        //  pId = pRoutineId ?
        //EnterNewLocal(pToken->String()) :
        //pComplexType->complex.pSymtabPublic->EnterNew(pToken->String());

        if (pId->defn.how == dcUndefined) {
            pId->defn.how = pRoutineId ? dcVariable : dcMember;
            if (!pFirstId) pFirstId = pLastId = pId;
            else {
                pLastId->next = pId;
                pLastId = pId;
            }
        }

        GetTokenAppend();

        Resync(tlIdentifierFollow);
        if (token == tcComma) {
            do {
                GetTokenAppend();
                Resync(tlIdentifierStart, tlIdentifierFollow);
                if (token == tcComma) Error(errMissingIdentifier);
            } while (token == tcComma);

            if (token != tcIdentifier) Error(errMissingIdentifier);
        } else if (token == tcIdentifier) Error(errMissingComma);
    }

    return pFirstId;
}

