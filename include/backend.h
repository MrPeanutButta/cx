/* 
 * File:   backend.h
 * Author: aaro3965
 *
 * Created on June 21, 2013, 1:04 PM
 */

#ifndef BACKEND_H
#define	BACKEND_H

#include "misc.h"
#include "symtable.h"
#include "token.h"
#include "icode.h"

extern TIcode icode;

class TBackend {
protected:
    TToken *pToken;
    TTokenCode token;
    TSymtabNode *pNode;

    void GetToken(void) {
        pToken = icode.Get();
        token = pToken->Code();
        pNode = icode.SymtabNode();
    }

    void Goto(int location) {
        icode.Goto(location);
    }

    int CurrentLocation(void) const {
        return icode.CurrentLocation();
    }
    
public:
    virtual ~TBackend(){}
    virtual void Go(void) = 0;
};

#endif	/* BACKEND_H */

