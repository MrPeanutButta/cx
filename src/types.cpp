
#include <cstdio>
#include "buffer.h"
#include "error.h"
#include "types.h"

const char *formStrings[] = {
    "*error*", "scalar", "enum", "subrange", "array", "complex", "pointer"
};

TSymtabNode *pMain = nullptr;
TType *pIntegerType = nullptr;
TType *pFloatType = nullptr;
TType *pDoubleType = nullptr;
TType *pBooleanType = nullptr;
TType *pCharType = nullptr;
TType *pClassType = nullptr;
TType *pComplexType = nullptr;

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
        case fcComplex:
            // delete complex.pSymtabPublic;
            break;

        default:
            break;
    }
}

void TType::PrintTypeSpec(TVerbosityCode vc) {
    sprintf(list.text, "%s, size %d bytes. type id: ", formStrings[form], size);

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
        case fcComplex: PrintRecordType(vc);
            break;
    }
}

void TType::PrintEnumType(TVerbosityCode vc) const {
    if (vc == vcTerse) return;

    list.PutLine("---enum constant identifiers (value = name)---");

    for (TSymtabNode *pConstId = enumeration.pConstIds;
            pConstId; pConstId = pConstId->next) {
        sprintf(list.text, "%d = %s",
                pConstId->defn.constant.value.__int,
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

void TType::PrintRecordType(TVerbosityCode vc) {
    if (vc == vcTerse) return;

    list.PutLine("member identifiers (offset : name)---");
    list.PutLine();

//    if (complex.MemberTable.empty()) {
//        list.PutLine("empty type");
//        return;
//    }

//    for (TSymtabNode *pFieldId = complex.pSymtabClassScope->Root();
//            pFieldId; pFieldId = pFieldId->next) {
//        sprintf(list.text, "\t%d : %s",
//                pFieldId->defn.data.offset,
//                pFieldId->String());
//        list.PutLine();
//        pFieldId->PrintVarOrField();
//    }
//
//    list.PutLine("public:\n");
//
//    for (TSymtabNode *pFieldId = complex.MemberTable[tcPublic]->Root();
//            pFieldId; pFieldId = pFieldId->next) {
//        sprintf(list.text, "\t%d : %s",
//                pFieldId->defn.data.offset,
//                pFieldId->String());
//        list.PutLine();
//        pFieldId->PrintVarOrField();
//    }
//
//    list.PutLine();
//    list.PutLine("private:\n");
//
//    if (!complex.MemberTable[tcPrivate]) {
//        list.PutLine("empty scope");
//    } else {
//
//        for (TSymtabNode *pFieldId = complex.MemberTable[tcPrivate]->Root();
//                pFieldId; pFieldId = pFieldId->next) {
//            sprintf(list.text, "\t%d : %s",
//                    pFieldId->defn.data.offset,
//                    pFieldId->String());
//            list.PutLine();
//            pFieldId->PrintVarOrField();
//        }
//    }
//
//    list.PutLine();
//    list.PutLine("protected:\n");
//
//    if (!complex.MemberTable[tcProtected]) {
//        list.PutLine("empty scope");
//    } else {
//        for (TSymtabNode *pFieldId = complex.MemberTable[tcProtected]->Root();
//                pFieldId; pFieldId = pFieldId->next) {
//            sprintf(list.text, "\t%d : %s",
//                    pFieldId->defn.data.offset,
//                    pFieldId->String());
//            list.PutLine();
//            pFieldId->PrintVarOrField();
//        }
//    }
}

void InitializePredefinedTypes(TSymtab *pSymtab) {

    pMain = pSymtab->Enter("main", dcFunction);
    pMain->defn.routine.which = rcForward;

    TSymtabNode *pIntegerId = pSymtab->Enter("int", dcType);
    TSymtabNode *pFloatId = pSymtab->Enter("float", dcType);

    TSymtabNode *pComplexId = pSymtab->Enter("class", dcType);

    TSymtabNode *pBooleanId = pSymtab->Enter("bool", dcType);
    TSymtabNode *pCharId = pSymtab->Enter("char", dcType);
    TSymtabNode *pFalseId = pSymtab->Enter("false", dcConstant);
    TSymtabNode *pTrueId = pSymtab->Enter("true", dcConstant);

    if (!pIntegerType) {
        SetType(pIntegerType, new TType(fcScalar, sizeof (int), pIntegerId));
    }
    if (!pFloatType) {
        SetType(pFloatType, new TType(fcScalar, sizeof (float), pFloatId));
    }

    if (!pBooleanType) {
        SetType(pBooleanType, new TType(fcEnum, sizeof (int), pBooleanId));
    }
    if (!pCharType) {
        SetType(pCharType, new TType(fcScalar, sizeof (char), pCharId));
    }
    if (!pComplexType) {
        SetType(pComplexType, new TType(fcComplex, sizeof (TType), pComplexId));
    }

    SetType(pMain->pType, pIntegerType);

    // link each predefined type id's node to it's type object
    SetType(pIntegerId->pType, pIntegerType);

    SetType(pFloatId->pType, pFloatType);

    SetType(pBooleanId->pType, pBooleanType);
    SetType(pCharId->pType, pCharType);

    SetType(pComplexId->pType, pComplexType);

    pBooleanType->enumeration.max = 1;
    pBooleanType->enumeration.pConstIds = pFalseId;

    pFalseId->defn.constant.value.__bool = false;
    pTrueId->defn.constant.value.__bool = true;

    SetType(pTrueId->pType, pBooleanType);
    SetType(pFalseId->pType, pBooleanType);

    pFalseId->next = pTrueId;

    SetType(pDummyType, new TType(fcNone, 1, nullptr));
}

void RemovePredefinedTypes(void) {
    RemoveType(pComplexType);
    RemoveType(pIntegerType);
    RemoveType(pFloatType);
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

void RemoveType(TType *&pType) {
    if (pType && (--pType->refCount == 0)) {
        delete pType;
        pType = nullptr;
    }
}

void CheckRelOpOperands(const TType *pType1, const TType *pType2) {
    pType1 = pType1->Base();
    pType2 = pType2->Base();

    if ((pType1 == pType2) &&
            ((pType1->form == fcScalar) || (pType1->form == fcEnum))) {
        return;
    }

    if (((pType1 == pIntegerType) && (pType2 == pFloatType))
            || ((pType2 == pIntegerType) && (pType2 == pFloatType))) {
        return;
    }

    if ((pType1->form == fcArray)
            && (pType2->form == fcArray)
            && (pType1->array.pElmtType == pCharType)
            && (pType2->array.pElmtType == pCharType)
            && (pType1->array.elmtCount == pType2->array.elmtCount)) {
        return;
    }

    Error(errIncompatibleTypes);
}

void CheckIntegerOrReal(const TType *pType1, const TType *pType2) {
    pType1 = pType1->Base();

    if ((pType1 != pIntegerType) && (pType1 != pFloatType)) {
        Error(errIncompatibleTypes);
    }

    if (pType2) {
        pType2 = pType2->Base();

        if ((pType2 != pIntegerType) && (pType2 != pFloatType)) {
            Error(errIncompatibleTypes);
        }
    }
}

void CheckBoolean(const TType *pType1, const TType *pType2) {
    if ((pType1->Base() != pBooleanType)
            || (pType2 && (pType2->Base() != pBooleanType))) {
        Error(errIncompatibleTypes);
    }
}

void CheckAssignmentTypeCompatible(const TType *pTargetType,
        const TType *pValueType, TErrorCode ec) {

    pTargetType = pTargetType->Base();
    pValueType = pValueType->Base();

    if (pTargetType == pValueType) return;

    if ((pTargetType == pFloatType)
            && (pValueType == pIntegerType)) return;

    if ((pTargetType == pFloatType)
            && (pValueType == pDoubleType)) return;

    if ((pTargetType == pDoubleType)
            && (pValueType == pIntegerType)) return;

    if ((pTargetType == pDoubleType)
            && (pValueType == pFloatType)) return;

    if ((pTargetType == pIntegerType)
            && (pValueType == pFloatType)) return;

    if ((pTargetType == pIntegerType)
            && (pValueType == pDoubleType)) return;

    if ((pTargetType->form == fcArray)
            && (pValueType->form == fcArray)
            && (pTargetType->array.pElmtType == pCharType)
            && (pValueType->array.pElmtType == pCharType)) {
        //&& (pTargetType->array.elmtCount == pValueType->array.elmtCount)) {
        return;
    }

    Error(ec);
}

bool IntegerOperands(const TType *pType1, const TType *pType2) {
    pType1 = pType1->Base();
    pType2 = pType2->Base();

    return (pType1 == pIntegerType) && (pType2 == pIntegerType);
}

bool RealOperands(const TType *pType1, const TType *pType2) {
    pType1 = pType1->Base();
    pType2 = pType2->Base();

    return (pType1 == pFloatType) && (pType2 == pFloatType)
            || (pType1 == pFloatType) && (pType2 == pIntegerType)
            || (pType2 == pFloatType) && (pType1 == pIntegerType)
            || (pType1 == pFloatType) && (pType2 == pDoubleType)
            || (pType2 == pFloatType) && (pType1 == pDoubleType)
            || (pType1 == pDoubleType) && (pType2 == pIntegerType)
            || (pType2 == pDoubleType) && (pType1 == pIntegerType)
            || (pType1 == pDoubleType) && (pType2 == pFloatType)
            || (pType2 == pDoubleType) && (pType1 == pFloatType);
}