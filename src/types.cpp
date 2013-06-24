
#include <cstdio>
#include "buffer.h"
#include "error.h"
#include "types.h"

const char *formStrings[] = {
    "*error*", "scalar", "enum", "subrange", "array", "record"
};

TType *pIntegerType = nullptr;
TType *pRealType = nullptr;
TType *pBooleanType = nullptr;
TType *pCharType = nullptr;
TType *pDummyType = nullptr;

TType::TType(TFormCode fc, int s, TSymtabNode* pId)
: form(fc), size(s), pTypeId(pId), refCount(0) {

    switch (fc) {
        case fcSubrange:
            subrange.pBaseType = nullptr;
            break;
        case fcArray:
            array.pIndexType = array.pElmtType = nullptr;
            break;
        default:
            break;
    }
}

TType::TType(int length)
: size(length), form(fcArray), refCount(0) {
    pTypeId = nullptr;

    array.pIndexType = array.pElmtType = nullptr;
    SetType(array.pIndexType, new TType(fcSubrange, sizeof (int), nullptr));
    SetType(array.pElmtType, pCharType);
    array.elmtCount = length;

    SetType(array.pIndexType->subrange.pBaseType, pIntegerType);
    array.pIndexType->subrange.min = 1;
    array.pIndexType->subrange.max = length;
}

TType::~TType() {
    switch (form) {
        case fcSubrange:
            RemoveType(subrange.pBaseType);
            break;
        case fcArray:
            RemoveType(array.pIndexType);
            RemoveType(array.pElmtType);
            break;
        case fcRecord:
            delete record.pSymtab;
            break;

        default:
            break;
    }
}

void TType::PrintTypeSpec(TVerbosityCode vc) const {
    sprintf(list.text, "%s, size &d bytes. type id: ", formStrings[form], size);

    if (pTypeId) strcat(list.text, pTypeId->String());
    else {
        strcat(list.text, "<unnamed>");
        vc = vcVerbose;
    }

    list.PutLine();

    switch (form) {
        case fcEnum: PrintEnumType(vc);
            break;
        case fcSubrange: PrintSubrangeType(vc);
            break;
        case fcArray: PrintArrayType(vc);
            break;
        case fcRecord: PrintRecordType(vc);
            break;
    }
}

void TType::PrintEnumType(TVerbosityCode vc) const {
    if (vc == vcTerse) return;

    list.PutLine("---enum constant identifiers (value = name)---");

    for (TSymtabNode *pConstId = enumeration.pConstIds;
            pConstId; pConstId = pConstId->next) {
        sprintf(list.text, "\t%d = %s",
                pConstId->defn.constant.value.integer,
                pConstId->String());

        list.PutLine();
    }
}

void TType::PrintSubrangeType(TVerbosityCode vc) const {
    if (vc == vcTerse) return;

    sprintf(list.text, "min value = %d, max value = %d",
            subrange.min, subrange.max);

    list.PutLine();

    if (subrange.pBaseType) {
        list.PutLine("---base type---");
        subrange.pBaseType->PrintTypeSpec(vcTerse);
    }
}

void TType::PrintArrayType(TVerbosityCode vc) const {
    if (vc == vcTerse) return;

    sprintf(list.text, "%d elements", array.elmtCount);
    list.PutLine();

    if (array.pIndexType) {
        list.PutLine("---index type---");
        array.pIndexType->PrintTypeSpec(vcTerse);
    }

    if (array.pElmtType) {
        list.PutLine("---element type---");
        array.pElmtType->PrintTypeSpec(vcTerse);
    }
}

void TType::PrintRecordType(TVerbosityCode vc) const {
    if (vc == vcTerse) return;

    list.PutLine("record field identifiers (offset : name)---");
    list.PutLine();

    for (TSymtabNode *pFieldId = record.pSymtab->Root();
            pFieldId; pFieldId = pFieldId->next) {
        sprintf(list.text, "\t%d : %s",
                pFieldId->defn.data.offset,
                pFieldId->String());
        list.PutLine();
        pFieldId->PrintVarOrField();
    }
}

void InitializePredefinedTypes(TSymtab *pSymtab) {
    TSymtabNode *pIntegerId = pSymtab->Enter("int", dcType);
    TSymtabNode *pRealId = pSymtab->Enter("float", dcType);
    TSymtabNode *pBooleanId = pSymtab->Enter("bool", dcType);
    TSymtabNode *pCharId = pSymtab->Enter("char", dcType);
    TSymtabNode *pFalseId = pSymtab->Enter("false", dcConstant);
    TSymtabNode *pTrueId = pSymtab->Enter("true", dcConstant);

    if (!pIntegerType) {
        SetType(pIntegerType, new TType(fcScalar, sizeof (int), pIntegerId));
    }
    if (!pRealType) {
        SetType(pRealType, new TType(fcScalar, sizeof (float), pRealId));
    }
    if (!pBooleanType) {
        SetType(pBooleanType, new TType(fcEnum, sizeof (int), pBooleanId));
    }
    if (!pCharType) {
        SetType(pCharType, new TType(fcScalar, sizeof (char), pCharId));
    }

    // link each predefined type id's node to it's type object
    SetType(pIntegerId->pType, pIntegerType);
    SetType(pRealId->pType, pRealType);
    SetType(pBooleanId->pType, pBooleanType);
    SetType(pCharId->pType, pCharType);

    pBooleanType->enumeration.max = 1;
    pBooleanType->enumeration.pConstIds = pFalseId;

    pFalseId->defn.constant.value.integer = 0;
    pTrueId->defn.constant.value.integer = 1;

    SetType(pTrueId->pType, pBooleanType);
    SetType(pFalseId->pType, pBooleanType);

    pFalseId->next = pTrueId;

    SetType(pDummyType, new TType(fcNone, 1, nullptr));
}

void RemovePredefinedTypes(void){
    RemoveType(pIntegerType);
    RemoveType(pRealType);
    RemoveType(pBooleanType);
    RemoveType(pCharType);
    RemoveType(pDummyType);
}

void RemoveType(TType *&pType);

TType *SetType(TType *&pTargetType, TType *pSourceType) {
    if (!pTargetType) RemoveType(pTargetType);

    ++pSourceType->refCount;

    pTargetType = pSourceType;

    return pSourceType;
}

void RemoveType(TType *&pType){
    if(pType && (--pType->refCount == 0)){
        delete pType;
        pType = nullptr;
    }
}
