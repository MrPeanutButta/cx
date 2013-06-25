/*
 * File:   icode.h
 * Author: aaro3965
 *
 * Created on June 19, 2013, 7:02 PM
 */

#ifndef ICODE_H
#define	ICODE_H

#include <fstream>
#include "misc.h"
#include "symtable.h"
#include "token.h"
#include "scanner.h"

extern int cntSymtabs;
extern TSymtab *pSymtabList;
extern TSymtab **vpSymtabs;

using namespace std;

const TTokenCode mcLineMarker = ((TTokenCode) 127);

class TSymtabNode;

class TIcode : public TScanner {

    enum {
        codeSegmentSize = 4096
    };

    char *pCode; // ptr to code segment
    char *cursor; // ptr to code location
    TSymtabNode *pNode; // ptr to extracted symbol table node

    void CheckBounds(int size);
    TSymtabNode *GetSymtabNode(void);

public:

    TIcode() {
        pCode = cursor = new char[codeSegmentSize];
    }
    TIcode(const TIcode &icode); // cpy ctor

    ~TIcode() {
        delete [] pCode;
    }

    void Put(TTokenCode tc);
    void Put(const TSymtabNode *pNode);
    void InsertLineMarker(void);

    void Reset(void) {
        cursor = pCode;
    }

    void Goto(int location) {
        cursor = pCode + location;
    }

    int CurrentLocation(void) const {
        return cursor - pCode;
    }

    TSymtabNode *SymtabNode(void) const {
        return pNode;
    }

    virtual TToken *Get(void);

};

#endif	/* ICODE_H */

