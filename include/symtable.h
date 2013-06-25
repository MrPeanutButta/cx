/*
 * File:   symtable.h
 * Author: aaro3965
 *
 * Created on June 19, 2013, 12:17 AM
 */

#ifndef SYMTABLE_H
#define	SYMTABLE_H

#include <cstring>
#include "misc.h"

extern bool xrefFlag;
extern int currentLineNumber;
extern int asmLabelIndex;

class TSymtab;
class TSymtabNode;
class TLineNumList;
class TIcode;
class TType;

enum TDefnCode {
    dcUndefined, dcConstant, dcType, dcVariable, dcField,
    dcValueParm, dcVarParm,
    dcProgram, dcProcedure, dcFunction
};

enum TRoutineCode {
    rcDeclared, rcForward,
    rcRead, rcReadln, rcWrite, rcWriteln,
    rcAbs, rcArctan, rcChr, rcCos, rcEof, rcEoln,
    rcExp, rcLn, rcOdd, rcOrd, rcPred, rcRound,
    rcSin, rcSqr, rcSqrt, rcSucc, rcTrunc
};

struct TLocalIds {
    TSymtabNode *pParmsIds;
    TSymtabNode *pConstantIds;
    TSymtabNode *pTypeIds;
    TSymtabNode *pVariableIds;
    TSymtabNode *pRoutineIds;
};

class TDefn {
public:

    TDefnCode how;

    union {

        struct {
            TDataValue value;
        } constant;

        struct {
            TRoutineCode which;
            int parmCount;
            int totalParmSize;
            int totalLocalSize;
            TLocalIds locals;
            TSymtab *pSymtab;
            TIcode *pIcode;
        } routine;

        struct {
            int offset;
        } data;
    };

    TDefn(TDefnCode dc) {
        how = dc;
    }
    ~TDefn();
};

class TSymtabNode {
    TSymtabNode *left, *right;
    char *pString;
    short xSymtab;
    short xNode;
    TLineNumList *pLineNumList;

    friend class TSymtab;

public:

    TSymtabNode *next;
    TType *pType;

    TDefn defn;
    int level;
    int labelIndex;

    //legacy
    float value;

    TSymtabNode(const char *pStr, TDefnCode dc = dcUndefined);
    ~TSymtabNode();

    TSymtabNode *LeftSubtree(void) const {
        return left;
    }

    TSymtabNode *RightSubtree(void) const {
        return right;
    }

    char *String(void) const {
        return pString;
    }

    short SymtabIndex(void) const {
        return xSymtab;
    }

    short NodeIndex(void) const {
        return xNode;
    }

    void Convert(TSymtabNode *vpNodes[]);

    void Print(void) const;
    void PrintIdentifier(void) const;
    void PrintConstant(void) const;
    void PrintVarOrField(void) const;
    void PrintType(void) const;
};

class TSymtab {
    TSymtabNode *root;
    TSymtabNode **vpNodes;
    short cntNodes;
    short xSymtab;
    TSymtab *next;

public:

    TSymtab() : cntNodes(0), xSymtab(0) {
        extern int cntSymtabs;
        extern TSymtab *pSymtabList;

        root = nullptr;
        vpNodes = nullptr;
        xSymtab = cntSymtabs++;

        next = pSymtabList;
        pSymtabList = this;
    }

    ~TSymtab() {
        delete root;
        delete [] vpNodes;
    }

    TSymtabNode *Search(const char *pString) const;
    TSymtabNode *Enter(const char *pString, TDefnCode dc = dcUndefined);
    TSymtabNode *EnterNew(const char *pString, TDefnCode dc = dcUndefined);

    TSymtabNode *Root(void) const {
        return root;
    }

    TSymtabNode *Get(short xNode) const {
        return vpNodes[xNode];
    }

    TSymtab *Next(void) const {
        return next;
    }

    TSymtabNode **NodeVector(void) const {
        return vpNodes;
    }

    int NodeCount(void)const {
        return cntNodes;
    }

    void Print(void) const {
        root->Print();
    }

    void Convert(TSymtab *vpSymtabs[]);

};

class TLineNumNode {
    TLineNumNode *next;
    const int number;

    friend class TLineNumList;

public:

    TLineNumNode() :
    number(currentLineNumber) {
        next = nullptr;
    }
};

class TLineNumList {
    TLineNumNode *head, *tail;

public:

    TLineNumList() :
    head(new TLineNumNode), tail(new TLineNumNode) {
    }

    virtual ~TLineNumList();

    void Update(void);
    void Print(int newLineFlag, int indent) const;
};

#endif	/* SYMTABLE_H */

