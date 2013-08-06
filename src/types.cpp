
#include <cstdio>
#include "buffer.h"
#include "error.h"
#include "types.h"

const char *formStrings[] = {
    "*error*", "scalar", "enum", "subrange", "array", "complex", "pointer"
};

// Pointers to predefined types.
cx_symtab_node *pMain = nullptr;
cx_symtab_node *pStdIn = nullptr;
cx_symtab_node *pStdOut = nullptr;
cx_symtab_node *pStdErr = nullptr;

cx_type *pIntegerType = nullptr;
cx_type *pFloatType = nullptr;
cx_type *pDoubleType = nullptr;
cx_type *pBooleanType = nullptr;
cx_type *pCharType = nullptr;
cx_type *pClassType = nullptr;
cx_type *p_complex_type = nullptr;
cx_type *pFileType = nullptr;

cx_type *pDummyType = nullptr;

/** Constructors    General.
 * 
 * @param fc  : form code.
 * @param s   : byte size of type.
 * @param p_id : ptr to symbol table node of type identifier.
 */
cx_type::cx_type(TFormCode fc, int s, cx_symtab_node* p_id)
: form(fc), size(s), pTypeId(p_id), refCount(0) {

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

cx_type::cx_type(int length)
: size(length), form(fcArray), refCount(0) {
    pTypeId = nullptr;

    array.pIndexType = array.pElmtType = nullptr;
    SetType(array.pIndexType, new cx_type(fcSubrange, sizeof (int), nullptr));
    SetType(array.pElmtType, pCharType);
    array.elmtCount = length;

    SetType(array.pIndexType->subrange.pBaseType, pIntegerType);
    array.pIndexType->subrange.min = 1;
    array.pIndexType->subrange.max = length;
}

/** Destructor      Delete the allocated objects according to
 *                  the form code.  Note that the objects
 *                  pointed to by enumeration.pConstIds and by
 *                  subrange.pBaseType are deleted along with
 *                  the symbol tables that contain their
 *                  identifiers. 
 */
cx_type::~cx_type() {
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

/** PrintTypeSpec       print information about a type
 *                      specification for the cross-reference.
 * 
 * @param vc : vcVerbose or vcTerse to control the output.
 */
void cx_type::PrintTypeSpec(TVerbosityCode vc) {
    sprintf(list.text, "%s, size %d bytes. type id: ", formStrings[form], size);

    if (pTypeId) strcat(list.text, pTypeId->string__());
    else {
        strcat(list.text, "<unnamed>");
        vc = vcVerbose;
    }

    list.put_line();

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

/** PrintEnumType       print information about an enumeration
 *                      type for the cross-reference.
 * 
 * @param vc : vcVerbose or vcTerse to control the output.
 */
void cx_type::PrintEnumType(TVerbosityCode vc) const {
    if (vc == vcTerse) return;

    list.put_line("---enum constant identifiers (value = name)---");

    for (cx_symtab_node *p_const_id = enumeration.pConstIds;
            p_const_id; p_const_id = p_const_id->next__) {
        sprintf(list.text, "%d = %s",
                p_const_id->defn.constant.value.int__,
                p_const_id->string__());

        list.put_line();
    }
}

/** PrintSubrangeType   print information about a subrange
 *                      type for the cross-reference.
 * 
 * @param vc : vcVerbose or vcTerse to control the output.
 */
void cx_type::PrintSubrangeType(TVerbosityCode vc) const {
    if (vc == vcTerse) return;

    sprintf(list.text, "min value = %d, max value = %d",
            subrange.min, subrange.max);

    list.put_line();

    if (subrange.pBaseType) {
        list.put_line("---base type---");
        subrange.pBaseType->PrintTypeSpec(vcTerse);
    }
}

/** PrintArrayType      print information about an array
 *                      type for the cross-reference.
 * 
 * @param vc : vcVerbose or vcTerse to control the output.
 */
void cx_type::PrintArrayType(TVerbosityCode vc) const {
    if (vc == vcTerse) return;

    sprintf(list.text, "%d elements", array.elmtCount);
    list.put_line();

    if (array.pIndexType) {
        list.put_line("---index type---");
        array.pIndexType->PrintTypeSpec(vcTerse);
    }

    if (array.pElmtType) {
        list.put_line("---element type---");
        array.pElmtType->PrintTypeSpec(vcTerse);
    }
}

/** PrintRecordType     print information about a record
 *                      type for the cross-reference.
 * 
 * @param vc : vcVerbose or vcTerse to control the output.
 */
void cx_type::PrintRecordType(TVerbosityCode vc) {
    if (vc == vcTerse) return;

    list.put_line("member identifiers (offset : name)---");
    list.put_line();

    //    if (complex.MemberTable.empty()) {
    //        list.put_line("empty type");
    //        return;
    //    }

    //    for (cx_symtab_node *pFieldId = complex.pSymtabClassScope->root();
    //            pFieldId; pFieldId = pFieldId->next__) {
    //        sprintf(list.text, "\t%d : %s",
    //                pFieldId->defn.data.offset,
    //                pFieldId->string__());
    //        list.put_line();
    //        pFieldId->print_var_or_field();
    //    }
    //
    //    list.put_line("public:\n");
    //
    //    for (cx_symtab_node *pFieldId = complex.MemberTable[tc_PUBLIC]->root();
    //            pFieldId; pFieldId = pFieldId->next__) {
    //        sprintf(list.text, "\t%d : %s",
    //                pFieldId->defn.data.offset,
    //                pFieldId->string__());
    //        list.put_line();
    //        pFieldId->print_var_or_field();
    //    }
    //
    //    list.put_line();
    //    list.put_line("private:\n");
    //
    //    if (!complex.MemberTable[tc_PRIVATE]) {
    //        list.put_line("empty scope");
    //    } else {
    //
    //        for (cx_symtab_node *pFieldId = complex.MemberTable[tc_PRIVATE]->root();
    //                pFieldId; pFieldId = pFieldId->next__) {
    //            sprintf(list.text, "\t%d : %s",
    //                    pFieldId->defn.data.offset,
    //                    pFieldId->string__());
    //            list.put_line();
    //            pFieldId->print_var_or_field();
    //        }
    //    }
    //
    //    list.put_line();
    //    list.put_line("protected:\n");
    //
    //    if (!complex.MemberTable[tc_PROTECTED]) {
    //        list.put_line("empty scope");
    //    } else {
    //        for (cx_symtab_node *pFieldId = complex.MemberTable[tc_PROTECTED]->root();
    //                pFieldId; pFieldId = pFieldId->next__) {
    //            sprintf(list.text, "\t%d : %s",
    //                    pFieldId->defn.data.offset,
    //                    pFieldId->string__());
    //            list.put_line();
    //            pFieldId->print_var_or_field();
    //        }
    //    }
}

/** initialize_builtin_types   Initialize the predefined
 *                              types by entering their
 *                              identifiers into the symbol
 *                              table.
 * 
 * @param p_symtab : ptr to symbol table.
 */
void initialize_builtin_types(cx_symtab *p_symtab) {

    pMain = p_symtab->enter("main", dc_function);
    pMain->defn.routine.which = rc_forward;

    cx_symtab_node *pIntegerId = p_symtab->enter("int", dc_type);
    cx_symtab_node *pFloatId = p_symtab->enter("float", dc_type);

    cx_symtab_node *pComplexId = p_symtab->enter("class", dc_type);

    cx_symtab_node *pBooleanId = p_symtab->enter("bool", dc_type);
    cx_symtab_node *pCharId = p_symtab->enter("char", dc_type);
    cx_symtab_node *pFalseId = p_symtab->enter("false", dc_constant);
    cx_symtab_node *pTrueId = p_symtab->enter("true", dc_constant);

    cx_symtab_node *pFileId = p_symtab->enter("file", dc_type);

    if (!pIntegerType) {
        SetType(pIntegerType, new cx_type(fcScalar, sizeof (int), pIntegerId));
    }
    if (!pFloatType) {
        SetType(pFloatType, new cx_type(fcScalar, sizeof (float), pFloatId));
    }

    if (!pBooleanType) {
        SetType(pBooleanType, new cx_type(fcEnum, sizeof (int), pBooleanId));
    }
    if (!pCharType) {
        SetType(pCharType, new cx_type(fcScalar, sizeof (char), pCharId));
    }
    if (!p_complex_type) {
        SetType(p_complex_type, new cx_type(fcComplex, sizeof (cx_type), pComplexId));
    }

    if (!pFileType) {
        SetType(pFileType, new cx_type(fcStream, sizeof (FILE), pFileId));
    }

    SetType(pMain->p_type, pIntegerType);

    // link each predefined type id's node to it's type object
    SetType(pIntegerId->p_type, pIntegerType);

    SetType(pFloatId->p_type, pFloatType);

    SetType(pBooleanId->p_type, pBooleanType);
    SetType(pCharId->p_type, pCharType);

    SetType(pComplexId->p_type, p_complex_type);

    pBooleanType->enumeration.max = 1;
    pBooleanType->enumeration.pConstIds = pFalseId;

    pFalseId->defn.constant.value.int__ = 0;
    pTrueId->defn.constant.value.int__ = 1;

    SetType(pTrueId->p_type, pBooleanType);
    SetType(pFalseId->p_type, pBooleanType);

    pFalseId->next__ = pTrueId;

    pStdOut = p_symtab->enter("__cx_stdout__", ::dc_variable);
    SetType(pStdOut->p_type, pFileType);
    pStdOut->p_type->stream.pFileStream = stdout;

    pStdIn = p_symtab->enter("__cx_stdin__", ::dc_variable);
    SetType(pStdIn->p_type, pFileType);
    pStdIn->p_type->stream.pFileStream = stdin;

    pStdErr = p_symtab->enter("__cx_stderr__", ::dc_variable);
    SetType(pStdErr->p_type, pFileType);
    pStdErr->p_type->stream.pFileStream = stderr;

    SetType(pDummyType, new cx_type(fcNone, 1, nullptr));
}

/** remove_builtin_types       Remove the predefined types.
 */
void remove_builtin_types(void) {
    RemoveType(p_complex_type);
    RemoveType(pIntegerType);
    RemoveType(pFloatType);
    RemoveType(pBooleanType);
    RemoveType(pCharType);
    RemoveType(pDummyType);
    RemoveType(pFileType);
}

void RemoveType(cx_type *&p_type);

/** SetType     Set the target type.  Increment the reference
 *              count of the source type.
 * 
 * @param p_target_type : ref to ptr to target type object.
 * @param pSourceType : ptr to source type object.
 * @return ptr to source type object.
 */
cx_type *SetType(cx_type *&p_target_type, cx_type *pSourceType) {
    if (!p_target_type) RemoveType(p_target_type);

    ++pSourceType->refCount;

    p_target_type = pSourceType;

    return pSourceType;
}

/** RemoveType  Decrement a type object's reference count, and
 *              delete the object and set its pointer to NULL
 *              if the count becomes 0.
 * 
 * @param p_type : ref to ptr to type object.
 */
void RemoveType(cx_type *&p_type) {
    if (p_type && (--p_type->refCount == 0)) {
        delete p_type;
        p_type = nullptr;
    }
}

/************************
 *                      *
 *  type Compatibility  *
 *                      *
 ************************/

/** CheckRelOpOperands  Check that the types of the two operands
 *                      of a relational operator are compatible.
 *                      Flag an incompatible type error if not.
 * 
 * @param pType1 : ptr to the first  operand's type object.
 * @param pType2 : ptr to the second operand's type object.
 */
void CheckRelOpOperands(const cx_type *pType1, const cx_type *pType2) {
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

    cx_error(err_incompatible_types);
}

/** CheckIntegerOrReal  Check that the type of each operand is
 *                      either integer or real.  Flag an
 *                      incompatible type error if not.
 * 
 * @param pType1 : ptr to the first  operand's type object.
 * @param pType2 : ptr to the second operand's type object or NULL.
 */
void CheckIntegerOrReal(const cx_type *pType1, const cx_type *pType2) {
    pType1 = pType1->Base();

    if ((pType1 != pIntegerType) && (pType1 != pFloatType)) {
        cx_error(err_incompatible_types);
    }

    if (pType2) {
        pType2 = pType2->Base();

        if ((pType2 != pIntegerType) && (pType2 != pFloatType)) {
            cx_error(err_incompatible_types);
        }
    }
}

/** CheckBoolean        Check that the type of each operand is
 *                      boolean.  Flag an incompatible type
 *                      error if not.
 * 
 * @param pType1 : ptr to the first  operand's type object.
 * @param pType2 : ptr to the second operand's type object or NULL.
 */
void CheckBoolean(const cx_type *pType1, const cx_type *pType2) {
    if ((pType1->Base() != pBooleanType)
            || (pType2 && (pType2->Base() != pBooleanType))) {
        cx_error(err_incompatible_types);
    }
}

/** CheckAssignmentTypeCompatible   Check that a value's type is
 *                                  assignment compatible with
 *                                  the target's type.  Flag an
 *                                  error if not.
 * 
 * @param p_target_type : ptr to the target's type object.
 * @param pValueType  : ptr to the value's  type object.
 * @param ec          : error code.
 */
void CheckAssignmentTypeCompatible(const cx_type *p_target_type,
        const cx_type *pValueType, cx_error_code ec) {

    p_target_type = p_target_type->Base();
    pValueType = pValueType->Base();

    if (p_target_type == pValueType) return;

    if ((p_target_type == pFloatType)
            && (pValueType == pIntegerType)) return;

    if ((p_target_type == pFloatType)
            && (pValueType == pDoubleType)) return;

    if ((p_target_type == pDoubleType)
            && (pValueType == pIntegerType)) return;

    if ((p_target_type == pDoubleType)
            && (pValueType == pFloatType)) return;

    if ((p_target_type == pIntegerType)
            && (pValueType == pFloatType)) return;

    if ((p_target_type == pIntegerType)
            && (pValueType == pDoubleType)) return;

    if ((p_target_type->form == fcArray)
            && (pValueType->form == fcArray)
            && (p_target_type->array.pElmtType == pCharType)
            && (pValueType->array.pElmtType == pCharType)) {
        //&& (p_target_type->array.elmtCount == pValueType->array.elmtCount)) {
        return;
    }

    cx_error(ec);
}

/** IntegerOperands     Check that the types of both operands
 *                      are integer.
 * 
 * @param pType1 : ptr to the first  operand's type object.
 * @param pType2 : ptr to the second operand's type object.
 * @return true if yes, false if no.
 */
bool IntegerOperands(const cx_type *pType1, const cx_type *pType2) {
    pType1 = pType1->Base();
    pType2 = pType2->Base();

    return (pType1 == pIntegerType) && (pType2 == pIntegerType);
}

/** RealOperands        Check that the types of both operands
 *                      are real, or that one is real and the
 *                      other is integer.
 * 
 * @param pType1 : ptr to the first  operand's type object.
 * @param pType2 : ptr to the second operand's type object.
 * @return true if yes, false if no.
 */
bool RealOperands(const cx_type *pType1, const cx_type *pType2) {
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