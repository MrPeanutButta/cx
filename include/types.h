/*
 * File:   types.h
 * Author: aaro3965
 *
 * Created on June 24, 2013, 12:28 AM
 */

#ifndef TYPES_H
#define	TYPES_H

#include "error.h"
#include "symtable.h"

using namespace std;

extern TType *pIntegerType, *pFloatType, *pBooleanType, *pCharType,
        *pDummyType, *pComplexType;

extern TSymtabNode *pMain;

enum TFormCode {
    fcNone, fcScalar, fcEnum, fcSubrange, fcArray, fcComplex
};

extern const char *formStrings[];

class TType {
    int refCount;
public:
    TFormCode form;
    int size;
    TSymtabNode *pTypeId;

    union {

        struct {
            TSymtabNode *pConstIds;
            int max;
        } enumeration;

        struct {
            TType *pBaseType;
            int min, max;
        } subrange;

        struct {
            TType *pIndexType;
            TType *pElmtType;
            // first address at intdex 0
            void * start_address;
            void * end_address;
            int minIndex, maxIndex;
            int elmtCount;
        } array;

        struct {
            /* used only for internal to class.
             * connects all scopes to a single table */
            TSymtab *pSymtabClassScope;

            // seperate public, private and protected tables
            //ScopedSymtab MemberTable;
        } complex;
    };

    //struct {
    /* used only for internal to class.
     * connects all scopes to a single table */
    //TSymtab *pSymtabClassScope;

    // seperate public, private and protected tables
    //ScopedSymtab MemberTable;
    //} complex;

    TType(TFormCode fc, int s, TSymtabNode *pId);
    TType(int length);

    ~TType();

    bool IsScalar(void) const {
        return (form != fcArray) &&
                (form != fcComplex);
    }

    TType *Base(void) const {
        return form == fcSubrange ? subrange.pBaseType : (TType *) this;
    }

    enum TVerbosityCode {
        vcVerbose, vcTerse
    };

    void PrintTypeSpec(TVerbosityCode vc);
    void PrintEnumType(TVerbosityCode vc) const;
    void PrintSubrangeType(TVerbosityCode vc) const;
    void PrintArrayType(TVerbosityCode vc) const;
    void PrintRecordType(TVerbosityCode vc);

    friend TType *SetType(TType *&pTargetType, TType *pSourceType);
    friend void RemoveType(TType *&pType);

    friend void CheckRelOpOperands(const TType *pType1,
            const TType *pType2);
    friend void CheckIntegerOrReal(const TType *pType1,
            const TType *Type2 = nullptr);
    friend void CheckBoolean(const TType *pType1,
            const TType *pType2 = nullptr);
    friend void CheckAssignmentTypeCompatible(const TType *pTargetType,
            const TType *pValueType,
            TErrorCode ec);
    friend bool IntegerOperands(const TType *pType1,
            const TType *pType2);
    friend bool RealOperands(const TType *pType1,
            const TType *pType2);
};

void InitializePredefinedTypes(TSymtab *pSymtab);
void RemovePredefinedTypes(void);



#endif	/* TYPES_H */

