#include <cstdio>
#include <cstring>
#include "common.h"
#include "parser.h"

bool execFlag(true);

void TParser::ParseDeclarations(TSymtabNode *pNode) {

    TSymtabNode *pNewNode = nullptr;

    switch (token) {
        case tcConst:
            GetToken();
            ParseConstantDeclaration(pNode);
            break;
        case tcTypeDef:
            GetToken();
            ParseTypeDefinitions(pNode);
            break;
        case tcInt:
            GetToken();
            pNewNode = new TSymtabNode(pToken->String(), dcVariable);
            ParseIntegerDeclaration(pNewNode);
            break;
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

    GetToken();

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

    GetToken();
    CondGetToken(tcEqual, errMissingEqual);

    SetType(pConstId->pType, pTypeNode->pType);
    ParseConstant(pConstId);
    pConstId->defn.how = dcConstant;


    Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);

    ///CondGetToken(tcSemicolon, errMissingSemicolon);

    //while (token == tcSemicolon) GetToken();
    Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);

}

void TParser::ParseConstant(TSymtabNode *pConstId) {
    TTokenCode sign = tcDummy;

    if (TokenIn(token, tlUnaryOps)) {
        if (token == tcMinus) sign = tcMinus;
        GetToken();
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

            GetToken();
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

                GetToken();
            } else Error(errInvalidType);
            break;
    }
}

void TParser::ParseIdentifierConstant(TSymtabNode* pId1, TTokenCode sign) {
    TSymtabNode *pId2 = Find(pToken->String());

    if (pId2->defn.how != dcConstant) {
        Error(errNotAConstantIdentifier);
        SetType(pId1->pType, pDummyType);
        GetToken();
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

    GetToken();
}

/**
 * deprecated
 * @param pRoutineId
 */
void TParser::ParseIntegerDeclaration(TSymtabNode* pRoutineId) {
    TSymtabNode *__int = nullptr;
    TSymtabNode *pIntId = nullptr;

    while (token == tcIdentifier) {
        // check if this int follows a const qualifier
        TSymtabNode *pNode = SearchAll("__un_const__");

        // we found an unamed constant
        if (pNode) {

            if (pNode->pType->form == dcConstant) {
                pNode->RenameNode(pToken->String());
                pIntId = pNode;
            }
            // else - new scalar variable
        } else {
            if (!pRoutineId->defn.routine.locals.pVariableIds) {
                pRoutineId->defn.routine.locals.pVariableIds = pIntId;
            } else {
                __int = pRoutineId->defn.routine.locals.pVariableIds;

                while (__int->next)
                    __int = __int->next;

                __int->next = pIntId;

            }
        }

        SetType(pIntId->pType, pIntegerType);
        pIntId->defn.how = dcVariable;

        if (!pIntId->pType->pTypeId) {
            pIntId->pType->pTypeId = pIntId;
        }

        GetToken();

        switch (token) {
            case tcEqual:
            {
                TTokenCode sign;

                GetToken();
                if (TokenIn(token, tlUnaryOps)) {
                    if (token == tcMinus) sign = tcMinus;
                    GetToken();
                }

                CondGetToken(tcNumber, errInvalidNumber);
            }
                break;
            case tcComma: GetToken();
                break;
            case tcLeftSubscript:
                //found array form
                break;
        }
    }

    while (token == tcSemicolon)GetToken();

    //Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);
    //CondGetToken(tcSemicolon, errMissingSemicolon);
}

void TParser::ParseVariableDeclarations(TSymtabNode* pRoutineId) {
    if (execFlag) {
        ParseVarOrFieldDecls(pRoutineId, nullptr, 0);
    }
}

void TParser::ParseFieldDeclarations(TType* pRecordType, int offset) {
    ParseVarOrFieldDecls(nullptr, pRecordType, offset);
}

void TParser::ParseVarOrFieldDecls(TSymtabNode* pRoutineId,
        TType* pRecordType, int offset) {
    TSymtabNode *pId, *pFirstId, *pLastId;
    TSymtabNode *pPrevSublistLastId = nullptr;

    int totalSize = 0;

    while (token == tcIdentifier) {
        pFirstId = ParseIdSublist(pRoutineId, pRecordType, pLastId);

        Resync(tlSublistFollow, tlDeclarationFollow);
        CondGetToken(tcColon, errMissingColon);

        //        TType *pType = ParseTypeSpec();

        for (pId = pFirstId; pId; pId = pId->next) {
            //            SetType(pId->pType, pType);

            if (pRoutineId) {
                if (execFlag) {
                    pId->defn.data.offset = offset++;
                }
                //           totalSize += pType->size;
            } else {
                pId->defn.data.offset = offset;
                //         offset += pType->size;
            }
        }

        if (pFirstId) {
            if (pRoutineId && (!pRoutineId->defn.routine.locals.pVariableIds)) {
                pRoutineId->defn.routine.locals.pVariableIds = pFirstId;
            }

            if (pPrevSublistLastId) pPrevSublistLastId->next = pFirstId;
            pPrevSublistLastId = pLastId;
        }

        if (pRoutineId) {
            Resync(tlDeclarationFollow, tlStatementStart);
            CondGetToken(tcSemicolon, errMissingSemicolon);

            while (token == tcSemicolon) GetToken();

            Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);
        } else {
            Resync(tlFieldDeclFollow);
            if (token != tcRBracket) {
                CondGetToken(tcSemicolon, errMissingSemicolon);

                while (token == tcSemicolon)GetToken();
                Resync(tlFieldDeclFollow, tlDeclarationStart, tlStatementStart);
            }
        }
    }

    if (pRoutineId) {
        pRoutineId->defn.routine.totalLocalSize = totalSize;
    } else {
        pRecordType->size = offset;
    }
}

TSymtabNode *TParser::ParseIdSublist(const TSymtabNode* pRoutineId,
        const TType* pRecordType,
        TSymtabNode*& pLastId) {

    TSymtabNode *pId;
    TSymtabNode *pFirstId = nullptr;

    pLastId = nullptr;

    while (token == tcIdentifier) {
        pId = pRoutineId ?
                EnterNewLocal(pToken->String()) :
                pRecordType->record.pSymtab->EnterNew(pToken->String());

        if (pId->defn.how == dcUndefined) {
            pId->defn.how = pRoutineId ? dcVariable : dcField;
            if (!pFirstId) pFirstId = pLastId = pId;
            else {
                pLastId->next = pId;
                pLastId = pId;
            }
        }

        GetToken();

        Resync(tlIdentifierFollow);
        if (token == tcComma) {
            do {
                GetToken();
                Resync(tlIdentifierStart, tlIdentifierFollow);
                if (token == tcComma) Error(errMissingIdentifier);
            } while (token == tcComma);

            if (token != tcIdentifier) Error(errMissingIdentifier);
        } else if (token == tcIdentifier) Error(errMissingComma);
    }

    return pFirstId;
}

