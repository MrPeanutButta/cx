/*
 * File:   symtable.h
 * Author: aaro3965
 *
 * Created on June 19, 2013, 12:17 AM
 */

#ifndef SYMTABLE_H
#define	SYMTABLE_H

#include <map>
#include <cstring>
#include "misc.h"

using namespace std;

extern bool xrefFlag;
extern int currentLineNumber;
extern int currentNestingLevel;
extern int asmLabelIndex;

class TSymtab;
class TSymtabNode;
class TLineNumList;
class TIcode;
class TType;

// for public, private and protected scopes
typedef map<TTokenCode, TSymtab *> ScopedSymtab;

enum TDefnCode {
    dcUndefined, dcConstant, dcType, dcVariable, dcMember,
    dcValueParm, dcVarParm,
    dcProgram, dcFunction
};

enum TRoutineCode {
    rcDeclared, rcForward,
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
            int returnMarker;
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
    TSymtabNode *prev;
    
    TType *pType;

    TDefn defn;
    int level;
    int labelIndex;

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

    void RenameNode(const char *pStr){
        if(pString != nullptr){
            delete pString;
            pString = nullptr;
        }

        pString = new char[strlen(pStr)];
        strcpy(pString, pStr);
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

    void ConnectTables(ScopedSymtab &classSymtab){

        /*root = classSymtab[tcPublic]->root;
        root->left = classSymtab[tcProtected]->root;
        root->right = classSymtab[tcPrivate]->root;*/
    }

    TSymtabNode *Get(short xNode) const {
        if(vpNodes == nullptr) return nullptr;

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

//fig 8-5
//--------------------------------------------------------------
//  TSymtabStack      Symbol table stack class.
//--------------------------------------------------------------

class TSymtabStack {
    enum {maxNestingLevel = 8};

    TSymtab *pSymtabs[maxNestingLevel]; // stack of symbol table ptrs

    void InitializeMain(void);

public:
    TSymtabStack(void);
   ~TSymtabStack(void);

    TSymtabNode *SearchLocal(const char *pString)
    {
	return pSymtabs[currentNestingLevel]->Search(pString);
    }

    TSymtabNode *EnterLocal(const char *pString,
			    TDefnCode dc = dcUndefined)
    {
	return pSymtabs[currentNestingLevel]->Enter(pString, dc);
    }

    TSymtabNode *EnterNewLocal(const char *pString,
			       TDefnCode dc = dcUndefined)
    {
	return pSymtabs[currentNestingLevel]->EnterNew(pString, dc);
    }

    TSymtab *GetCurrentSymtab(void) const
    {
	return pSymtabs[currentNestingLevel];
    }

    void SetCurrentSymtab(TSymtab *pSymtab)
    {
	pSymtabs[currentNestingLevel] = pSymtab;
    }

    void SetScope(int scopeLevel) {
        currentNestingLevel = scopeLevel;
    }

    TSymtabNode *SearchAvailableScopes(const char *pString) const;
    TSymtabNode *SearchAll (const char *pString) const;
    TSymtabNode *Find      (const char *pString) const;
    void         EnterScope(void);
    TSymtab     *ExitScope (void);
};
//endfig

#endif	/* SYMTABLE_H */

