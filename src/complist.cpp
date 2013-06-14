#include <cstring>
#include <iostream>
#include "complist.h"

using namespace std;

void TCompactListBuffer::PutBlank(void) {
    if (++textLength >= maxCompactTextLength - 1) PutLine();
    else *pText++ = ' ';
}

void TCompactListBuffer::Put(const char* pString) {

    int tokenLength(strlen(pString));

    if (textLength + tokenLength >= maxCompactTextLength - 1) {
        PutLine();
    }

    strcpy(pText, pString);
    pText += tokenLength;
    textLength += tokenLength;

}

void TCompactListBuffer::PutLine(void) {
    if (textLength > 0) {
        *pText = '\0';
        cout << text << endl;

        pText = text;
        textLength = 0;
    }
}

