#include "symtable.h"
#include "icode.h"

const char *symbolStrings[] = {
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    //operators and punctuation
    //bitwise
    "^", "&", "|", "~", "~=", "^=", "&=", "|=", "<<",
    "<<=", ">>", ">>=", "-", "-=", "+", "+=", "=", "--",
    "++", "/", "/=", "*", "*=", "<", ">", "==", "<=", ">=",
    "!=", "%", "%=", "[", "]", "?", "#", ".*", "(", ")", "}",
    "{", ":", ";", ",", "...", ".", "::", "->", "->*", "||",
    "&&", "!", "\'", "\"",

    "if", "return", "continue", "friend", "true", "goto", "try",
    "delete", "short", "typeid", "bool", "do", "int", "singed",
    "typename", "break", "double", "long", "sizeof", "union",
    "case", "static", "unsigned", "catch", "else", "namespace",
    "using", "char", "enum", "new", "virtual", "char16_t", "explicit",
    "noexcept", "char32_t", "export", "nullptr", "switch", "struct",
    "void", "class", "extern", "operator", "template", "const", "false",
    "private", "this", "while", "float", "protected", "threadlocal",
    "for", "public", "throw", "default", "typedef", "mutable", "include",
    "string"
};

TIcode::TIcode(const char* pIformFileName, TMode mode)
: pFileName(pIformFileName) {
    if (mode == input) {
        file.open(pIformFileName, ios::in | ios::binary);
        if (!file.good()) AbortTranslation(abortIFormFileOpenFailed);
    } else {
        file.open(pIformFileName, ios::out | ios::binary);
        if (!file.good()) AbortTranslation(abortIFormFileOpenFailed);
        size = 0;
    }

    symtabStrings = nullptr;
}

TIcode::~TIcode() {
    if (symtabStrings != nullptr) {
        for (int i = 0; i < symtabCount; ++i) delete [] symtabStrings[i];
        delete [] symtabStrings;
    }

    file.close();
}

void TIcode::Put(int value) {
    file.write((const char *) &value, sizeof (int));
    size += sizeof (int);
}

void TIcode::Put(TTokenCode tc, short index) {
    char code = tc;

    file.write((const char *) &code, sizeof (char));
    size += sizeof (char);

    if (index != -1) {
        file.write((const char *) &index, sizeof (short));
        size += sizeof (short);
    }
}

void TIcode::Put(const TSymtabNode* pNode) {
    if (!pNode) return;

    Put(pNode->LeftSubtree());

    char length = strlen(pNode->String()) + 1;
    short index = pNode->NodeIndex();

    file.write((const  char *) &index, sizeof (short));
    file.write((const  char *) &length, sizeof (char));
    file.write((const  char *) pNode->String(), length);
    size += sizeof (short) + sizeof (char) +length;

    Put(pNode->RightSubtree());
}

TToken *TIcode::Get(void) {
    TToken *pToken;
    char code;
    TTokenCode token;

    file.read(( char *) &code, sizeof (char));
    token = (TTokenCode) code;

    switch (token) {
        case tcNumber: pToken = &numberToken;
            break;
        case tcString: pToken = &stringToken;
            break;

        case tcIdentifier:
        {
            pToken = &wordToken;
            pToken->code = tcIdentifier;
            break;
        }
        default:
        {
            if (token < tcIf) {
                pToken = &specialToken;
                pToken->code = token;
            } else {
                pToken = &wordToken;
                pToken->code = token;
            }
            break;
        }
    }

    switch (token) {
        case tcIdentifier:
        case tcNumber:
        case tcString:
        {
            short index;
            file.read(( char *) &index, sizeof (short));
            strcpy(pToken->string, symbolStrings[index]);
            break;
        }
        default:
        {
            strcpy(pToken->string, symbolStrings[(int)code]);
            break;
        }
    }

    return pToken;
}

int TIcode::GetInteger(void) {
    int value;
    file.read(( char *) &value, sizeof (int));
    return value;
}

void TIcode::GetSymtabStrings(void){
    int count;
    short index;
    char length;
    
    file.read(( char *) &count, sizeof(int));
    symtabStrings = new char *[count];
    symtabCount = count;
    
    do{
        file.read(( char *) &index, sizeof(short));
        file.read(( char *) &length, sizeof(char));
        
        symtabStrings[index] = new char[length];
        file.read(( char *) symtabStrings[index], length);
    }while(--count > 0);  
}

