/*
 * File:   complist.h
 * Author: aaron
 *
 * Created on June 13, 2013, 11:31 PM
 */

#ifndef COMPLIST_H
#define	COMPLIST_H

#include "buffer.h"

const int maxCompactTextLength = 72;

class TCompactListBuffer : public TTextOutBuffer {
private:
    int textLength;
    char *pText;

public:
    TCompactListBuffer() :
    textLength(0){
        pText = text;
        *pText = '\0';
    }

    ~TCompactListBuffer() {

    };

    void PutBlank(void);
    void Put(const char *pString);
    void PutLine(void);

};

#endif	/* COMPLIST_H */

