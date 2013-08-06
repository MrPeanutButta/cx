/*
 * File:   types.h
 * Author: aaro3965
 *
 * Created on June 24, 2013, 12:28 AM
 */

#ifndef TYPES_H
#define	TYPES_H

#include <cstdio>
#include "error.h"
#include "symtable.h"

using namespace std;

extern cx_type *pIntegerType, *pFloatType, *pBooleanType, *pCharType,
        *pDummyType, *p_complex_type, *pFileType;

extern cx_symtab_node *pMain;

enum TFormCode {
    fcNone, fcScalar, fcEnum, fcSubrange, fcArray, fcComplex, fcStream
};

extern const char *formStrings[];

class cx_type {
    int refCount;
public:
    TFormCode form;
    int size;
    cx_symtab_node *pTypeId;

    union {

        struct {
            cx_symtab_node *pConstIds;
            int max;
        } enumeration;

        struct {
            cx_type *pBaseType;
            int min, max;
        } subrange;

        struct {
            cx_type *pIndexType;
            cx_type *pElmtType;
            // first address at intdex 0
            void * start_address;
            void * end_address;
            int minIndex, maxIndex;
            int elmtCount;
        } array;

        struct {
            /* used only for internal to class.
             * connects all scopes to a single table */
            cx_symtab *pSymtabClassScope;

            // seperate public, private and protected tables
            //cx_scoped_symtab MemberTable;
        } complex;

        struct {
            // file stream
            FILE *pFileStream;
        } stream;
    };

    //struct {
    /* used only for internal to class.
     * connects all scopes to a single table */
    //cx_symtab *pSymtabClassScope;

    // seperate public, private and protected tables
    //cx_scoped_symtab MemberTable;
    //} complex;

    cx_type(TFormCode fc, int s, cx_symtab_node *p_id);
    cx_type(int length);

    ~cx_type();

    bool IsScalar(void) const {
        return (form != fcArray) &&
                (form != fcComplex) &&
                (form != fcStream);
    }

    cx_type *Base(void) const {
        return form == fcSubrange ? subrange.pBaseType : (cx_type *) this;
    }

    enum TVerbosityCode {
        vcVerbose, vcTerse
    };

    void PrintTypeSpec(TVerbosityCode vc);
    void PrintEnumType(TVerbosityCode vc) const;
    void PrintSubrangeType(TVerbosityCode vc) const;
    void PrintArrayType(TVerbosityCode vc) const;
    void PrintRecordType(TVerbosityCode vc);

    friend cx_type *SetType(cx_type *&p_target_type, cx_type *pSourceType);
    friend void RemoveType(cx_type *&p_type);

    friend void CheckRelOpOperands(const cx_type *pType1,
            const cx_type *pType2);
    friend void CheckIntegerOrReal(const cx_type *pType1,
            const cx_type *Type2 = nullptr);
    friend void CheckBoolean(const cx_type *pType1,
            const cx_type *pType2 = nullptr);
    friend void CheckAssignmentTypeCompatible(const cx_type *p_target_type,
            const cx_type *pValueType,
            cx_error_code ec);
    friend bool IntegerOperands(const cx_type *pType1,
            const cx_type *pType2);
    friend bool RealOperands(const cx_type *pType1,
            const cx_type *pType2);
};

void initialize_builtin_types(cx_symtab *p_symtab);
void remove_builtin_types(void);



#endif	/* TYPES_H */

