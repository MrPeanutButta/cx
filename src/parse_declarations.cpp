#include <cstdio>
#include <cstring>
#include "common.h"
#include "parser.h"

bool execFlag(true);
//
//void TParser::ParseDeclarations(TSymtabNode *pRoutineId) {
//    switch(token){
//        case tcConst:
//            GetToken();
//            ParseConstantDeclarations(pRoutineId);
//            break;
//        case tcTypeDef:
//            GetToken();
//            ParseTypeDefinitions(pRoutineId);
//            break;
//        case tcInt:
//            GetToken();
//            ParseVariableDeclarations(pRoutineId);
//            break;
//    }
//}
//
//void TParser::ParseConstantDeclarations(TSymtabNode* pRoutineId){
//    TSymtabNode *pLastId = nullptr;
//
//    while(token == tcIdentifier) {
//        TSymtabNode *pConstId = EnterNewLocal(pToken->String());
//
//        if(!pRoutineId->defn.routine.locals.pConstantIds){
//            pRoutineId->defn.routine.locals.pConstantIds = pConstId;
//        } else {
//            pLastId->next = pConstId;
//        }
//
//        pLastId = pConstId;
//
//        GetToken();
//        CondGetToken(tcEqual, errMissingEqual);
//
//        ParseConstant(pConstId);
//        pConstId->defn.how = dcConstant;
//
//        Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);
//
//        CondGetToken(tcSemicolon, errMissingSemicolon);
//
//        while(token == tcSemicolon) GetToken();
//        Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);
//    }
//}
//
//void TParser::ParseConstant(TSymtabNode *pConstId){
//    TTokenCode sign = tcDummy;
//
//    if(TokenIn(token, tlUnaryOps)) {
//        if (token == tcMinus) sign = tcMinus;
//        GetToken();
//    }
//
//    switch(token){
//        case tcNumber:
//            if(pToken->Type() == tyInteger){
//                pConstId->defn.constant.value.integer = sign == tcMinus ?
//                    -pToken->Value().integer : pToken->Value().integer;
//
//                SetType(pConstId->pType, pIntegerType);
//            }else{
//                pConstId->defn.constant.value.real = sign == tcMinus ?
//                    -pToken->Value().real : pToken->Value().real;
//
//                SetType(pConstId->pType, pRealType);
//            }
//
//            GetToken();
//            break;
//
//        case tcIdentifier:
//            ParseIdentifierConstant(pConstId, sign);
//            break;
//        case tcString:
//            int length = strlen(pToken->String()) - 2;
//
//            if(sign != tcDummy) Error(errInvalidConstant);
//
//            if(length == 1){
//                pConstId->defn.constant.value.character = pToken->String()[1];
//
//                SetType(pConstId->pType, pCharType);
//            }else {
//                char *pString = new char[length];
//                CopyQuotedString(pString, pToken->String());
//
//                pConstId->defn.constant.value.pString = pString;
//
//                SetType(pConstId->pType, new TType(length));
//            }
//
//            GetToken();
//            break;
//    }
//}
//
//void TParser::ParseIdentifierConstant(TSymtabNode* pId1, TTokenCode sign){
//    TSymtabNode *pId2 = Find(pToken->String());
//
//    if(pId2->defn.how != dcConstant){
//        Error(errNotAConstantIdentifier);
//        SetType(pId1->pType, pDummyType);
//        GetToken();
//        return;
//    }
//
//    if(pId2->pType == pIntegerType){
//        pId2->defn.constant.value.integer = sign == tcMinus ?
//            -pId2->defn.constant.value.integer :
//            pId2->defn.constant.value.integer;
//
//        SetType(pId1->pType, pIntegerType);
//    } else if(pId2->pType == pRealType){
//        pId1->defn.constant.value.real = sign == tcMinus ?
//            -pId2->defn.constant.value.real :
//            pId2->defn.constant.value.real;
//        SetType(pId1->pType, pRealType);
//    }else if(pId2->pType == pCharType){
//        if(sign != tcDummy) Error(errInvalidConstant);
//
//        pId1->defn.constant.value.character = pId2->defn.constant.value.character;
//
//        SetType(pId1->pType, pCharType);
//    }else if(pId2->pType->form == fcEnum) {
//        if (sign != tcDummy)Error(errInvalidConstant);
//
//        pId1->defn.constant.value.integer = pId2->defn.constant.value.integer;
//
//        SetType(pId1->pType, pId2->pType);
//    } else if(pId2->pType->form == fcArray){
//        if((sign != tcDummy) || (pId2->pType->array.pElmtType != pCharType)){
//            Error(errInvalidConstant);
//        }
//
//        pId1->defn.constant.value.pString = pId2->defn.constant.value.pString;
//
//        SetType(pId1->pType, pId2->pType);
//    }
//
//    GetToken();
//}
//
//void TParser::ParseVariableDeclarations(TSymtabNode* pRoutineId){
//    if(execFlag) {
//        ParseVarOrFieldDecls(pRoutineId, nullptr, 0);
//    }
//}
//
//void TParser::ParseFieldDeclarations(TType* pRecordType, int offset) {
//    ParseVarOrFieldDecls(nullptr, pRecordType, offset);
//}
//
//void TParser::ParseVarOrFieldDecls(TSymtabNode* pRoutineId,
//        TType* pRecordType, int offset){
//    TSymtabNode *pId, *pFirstId, *pLastId;
//    TSymtabNode *pPrevSublistLastId = nullptr;
//
//    int totalSize = 0;
//
//    while(token == tcIdentifier){
//        pFirstId = ParseIdSublist(pRoutineId, pRecordType, pLastId);
//
//        Resync(tlSublistFollow, tlDeclarationFollow);
//        CondGetToken(tcColon, errMissingColon);
//
//        TType *pType = ParseTypeSpec();
//
//        for(pId = pFirstId; pId; pId = pId->next){
//            SetType(pId->pType, pType);
//
//            if(pRoutineId){
//                if(execFlag){
//                    pId->defn.data.offset = offset++;
//                }
//                totalSize+=pType->size;
//            } else {
//                pId->defn.data.offset = offset;
//                offset += pType->size;
//            }
//        }
//
//        if(pFirstId){
//            if(pRoutineId && (!pRoutineId->defn.routine.locals.pVariableIds)){
//                pRoutineId->defn.routine.locals.pVariableIds = pFirstId;
//            }
//
//            if(pPrevSublistLastId) pPrevSublistLastId->next = pFirstId;
//            pPrevSublistLastId = pLastId;
//        }
//
//        if(pRoutineId){
//            Resync(tlDeclarationFollow, tlStatementStart);
//            CondGetToken(tcSemicolon, errMissingSemicolon);
//
//            while(token == tcSemicolon) GetToken();
//
//            Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);
//        } else {
//            Resync(tlFieldDeclFollow);
//            if(token != tcRBracket){
//                CondGetToken(tcSemicolon, errMissingSemicolon);
//
//                while(token == tcSemicolon)GetToken();
//                Resync(tlFieldDeclFollow, tlDeclarationStart, tlStatementStart);
//            }
//        }
//    }
//
//    if(pRoutineId){
//        pRoutineId->defn.routine.totalLocalSize = totalSize;
//    }else{
//        pRecordType->size = offset;
//    }
//}
//
//TSymtabNode *TParser::ParseIdSublist(const TSymtabNode* pRoutineId,
//        const TType* pRecordType,
//        TSymtabNode*& pLastId){
//
//    TSymtabNode *pId;
//    TSymtabNode *pFirstId = nullptr;
//
//    pLastId = nullptr;
//
//    while(token == tcIdentifier){
//        pId = pRoutineId ?
//            EnterNewLocal(pToken->String()) :
//            pRecordType->record.pSymtab->EnterNew(pToken->String());
//
//        if(pId->defn.how == dcUndefined){
//            pId->defn.how = pRoutineId ? dcVariable : dcField;
//            if(!pFirstId) pFirstId = pLastId = pId;
//            else {
//                pLastId->next = pId;
//                pLastId = pId;
//            }
//        }
//
//        GetToken();
//
//        Resync(tlIdentifierFollow);
//        if(token == tcComma){
//            do{
//                GetToken();
//                Resync(tlIdentifierStart, tlIdentifierFollow);
//                if(token ==tcComma) Error(errMissingIdentifier);
//            }while ( token == tcComma);
//
//            if(token != tcIdentifier) Error(errMissingIdentifier);
//        }else if(token == tcIdentifier) Error(errMissingComma);
//    }
//
//    return pFirstId;
//}
//
