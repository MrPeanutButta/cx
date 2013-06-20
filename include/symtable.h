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

class TLineNumList;

class TSymtabNode {
    TSymtabNode *left, *right;
    char *pString;
    short xSymtab;
    short xNode;
    TLineNumList *pLineNumList;

    friend class TSymtab;

public:

    float value;

    TSymtabNode(const char *pStr);
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

    void Print(void) const;
};

class TSymtab {
    TSymtabNode *root;
    short cntNodes;
    short xSymtab;

public:

    TSymtab() : cntNodes(0), xSymtab(0) {
        root = nullptr;
    }

    ~TSymtab() {
        delete root;
    }

    TSymtabNode *Search(const char *pString) const;
    TSymtabNode *Enter(const char *pString);

    TSymtabNode *Root(void) const {
        return root;
    }

    int NodeCount(void)const {
        return cntNodes;
    }

    void Print(void) const {
        root->Print();
    }

};

class TLineNumNode{
    TLineNumNode *next;
    const int number;
    
    friend class TLineNumList;
    
public:
    TLineNumNode() :
    number(currentLineNumber) { next = nullptr; }
};

class TLineNumList{
    TLineNumNode *head, *tail;
    
public:
    TLineNumList() :
    head(new TLineNumNode), tail(new TLineNumNode){
    }
    
    virtual ~TLineNumList();
    
    void Update(void);
    void Print(int newLineFlag, int indent) const;
};

#endif	/* SYMTABLE_H */

