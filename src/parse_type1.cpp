#include "common.h"
#include "parser.h"

//void TParser::ParseTypeDefinitions(TSymtabNode *pRoutineId) {
//    TSymtabNode *pLastId = nullptr;
//
//    while (token == tcIdentifier) {
//        TSymtabNode *pTypeId = EnterNewLocal(pToken->String());
//
//        if (!pRoutineId->defn.routine.locals.pTypeIds) {
//            pRoutineId->defn.routine.locals.pTypeIds = pTypeId;
//        } else {
//            pLastId->next = pTypeId;
//        }
//
//        pLastId = pTypeId;
//
//        GetToken();
//        CondGetToken(tcEqual, errMissingEqual);
//
//        SetType(pTypeId->pType, ParseTypeSpec());
//        pTypeId->defn.how = dcType;
//
//        if (!pTypeId->pType->pTypeId) {
//            pTypeId->pType->pTypeId = pTypeId;
//        }
//
//        Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);
//        CondGetToken(tcSemicolon, errMissingSemicolon);
//
//        while (token == tcSemicolon)GetToken();
//
//        Resync(tlDeclarationFollow, tlDeclarationStart, tlStatementStart);
//
//    }
//}
//
//TType *TParser::ParseTypeSpec(void) {
//    switch (token) {
//        case tcIdentifier:
//        {
//            TSymtabNode *pId = Find(pToken->String());
//
//            switch (pId->defn.how) {
//                case dcType: return ParseIdentifierType(pId);
//                case dcConstant: return ParseSubrangeType(pId);
//                default:
//                    Error(errNotATypeIdentifier);
//                    GetToken();
//                    return (pDummyType);
//            }
//        }
//
//        case tcLBracket: return ParseEnumerationType();
//        case tcLeftSubscript: return ParseArrayType();
//        case tcStruct: return ParseRecordType();
//        case tcPlus:
//        case tcMinus:
//        case tcNumber:
//        case tcString: return ParseSubrangeType(nullptr);
//
//        default:
//            Error(errInvalidType);
//            return (pDummyType);
//
//    }
//}
