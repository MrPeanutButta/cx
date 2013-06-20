/* 
 * File:   icode.h
 * Author: aaro3965
 *
 * Created on June 19, 2013, 7:02 PM
 */

#ifndef ICODE_H
#define	ICODE_H

#include <fstream>
#include "token.h"
#include "scanner.h"

using namespace std;

class TSymtabNode;

class TIcode : public TScanner{
    fstream file;
    const char *const pFileName;
    int size;
    
    char **symtabStrings;
    int symtabCount;
    
public:
    enum TMode {input, output};
    
    TIcode(const char *pIformFileName, TMode mode);
    ~TIcode();
    
    void GoTo(int location){ file.seekg(location, ios::beg); }
    int Size(void) const { return size;}
    
    void Put(int value);
    void Put(TTokenCode tc, short index = 1);
    void Put(const TSymtabNode *pNode);
    
    virtual TToken *Get(void);
    int GetInteger(void);
    void GetSymtabStrings(void);
    
};

#endif	/* ICODE_H */

