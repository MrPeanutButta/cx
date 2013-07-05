#include "common.h"
#include "parser.h"

TType *TParser::ParseArrayType(TSymtabNode *pArrayNode) {
    TType *pArrayType = pArrayNode->pType;
    //TType *pElmtType = pArrayNode->pType;
    //bool indexFlag(false);

    //GetToken();
    CondGetToken(tcLeftSubscript, errMissingLeftSubscript);

    pArrayNode->pType->form = fcArray;
    pArrayNode->pType->array.pElmtType = pArrayNode->pType;

    SetType(pArrayType->array.pElmtType, pArrayType->pTypeId->pType);
    SetType(pArrayType->array.pIndexType, pIntegerType);
    
    if(token == tcRightSubscript){
        //CondGetToken(tcRightSubscript, errMissingRightSubscript);
        ParseAssignment(pArrayNode);
    }else{
        int max_index = pToken->Value().__int;
        CondGetToken(tcNumber, errInvalidNumber);
    }
        
    if (pArrayType->form != fcNone) {
        pArrayType->size = ArraySize(pArrayType);
    }

    return pArrayType;
}

void TParser::ParseIndexType(TSymtabNode *pArrayNode) {
    if (TokenIn(token, tlIndexStart)) {
        TType *pArrayType = pArrayNode->pType;
        TType *pIndexType = pIntegerType; //ParseTypeSpec(pArrayNode);

        SetType(pArrayType->array.pIndexType, pIndexType);

        switch (pIndexType->form) {
            case fcSubrange:
                pArrayType->array.elmtCount = pIndexType->subrange.max -
                        pIndexType->subrange.min + 1;
                pArrayType->array.minIndex = pIndexType->subrange.min;
                pArrayType->array.maxIndex = pIndexType->subrange.max;
                return;

            case fcEnum:
                pArrayType->array.elmtCount = pIndexType->enumeration.max + 1;
                pArrayType->array.minIndex = 0;
                pArrayType->array.maxIndex = pIndexType->enumeration.max;

                return;

            default:
                SetType(pArrayType->array.pIndexType, pIndexType);
                pArrayType->array.elmtCount = 0;
                pArrayType->array.minIndex = pArrayType->array.maxIndex = 0;
                //Error(errInvalidIndexType);
        }
    }
}

int TParser::ArraySize(TType* pArrayType) {
    if (pArrayType->array.pElmtType->size == 0) {
        pArrayType->array.pElmtType->size = ArraySize(pArrayType->array.pElmtType);
    }

    return (pArrayType->array.elmtCount * pArrayType->array.pElmtType->size);
}

TType *TParser::ParseComplexType(TSymtabNode *pNode) {
    //TType *pType = new TType(fcComplex, 0, nullptr);

    pNode->pType->complex.pSymtabPublic = new TSymtab;

    //GetToken();
    ParseMemberDecls(pNode, 0);

    CondGetToken(tcSemicolon, errMissingSemicolon);
    
    SetType(pNode->pType, pComplexType);
    pNode->defn.how = dcType;
    
    //TType *pId = pNode->pType->complex.pSymtabPublic
    
    return pNode->pType;
}