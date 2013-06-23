#include <cstring>
#include "common.h"
#include "error.h"
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

TIcode::TIcode(const TIcode &icode) {
    int length = int(icode.cursor - icode.pCode); // len of icode

    pCode = cursor = new char[length];
    memcpy(pCode, icode.pCode, length);
}

void TIcode::CheckBounds(int size) {
    if (cursor + size >= &pCode[codeSegmentSize]) {
        Error(errCodeSegmentOverflow);
        AbortTranslation(abortCodeSegmentOverflow);
    }
}

void TIcode::Put(TTokenCode tc) {
    if (errorCount > 0) return;

    char code = tc;
    CheckBounds(sizeof (char));

    memcpy((void *) cursor, (const void *) &code, sizeof (char));
    cursor += sizeof (char);
}

void TIcode::Put(const TSymtabNode* pNode) {
    if (errorCount > 0) return;

    short xSymtab = pNode->SymtabIndex();
    short xNode = pNode->NodeIndex();

    CheckBounds(2 * sizeof (short));

    memcpy((void *) cursor,
            (const void *) &xSymtab, sizeof (short));

    memcpy((void *) (cursor + sizeof (short)),
            (const void *) &xNode, sizeof (short));

    cursor += (2 * sizeof (short));
}

TToken *TIcode::Get(void) {
    TToken *pToken;
    char code;
    TTokenCode token;

    do {
        memcpy((void *) &code, (const void *) cursor, sizeof (char));
        cursor += sizeof (char);
        token = (TTokenCode) code;

        if (token == mcLineMarker) {
            short number;
            memcpy((void *) &number, (const void *) cursor, sizeof (short));
            currentLineNumber = number;
            cursor += sizeof (short);
        }

    } while (token == mcLineMarker);

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

            if (token < tcIf) {
                pToken = &specialToken;
                pToken->code = token;
            } else {
                pToken = &wordToken;
                pToken->code = token;
            }
            break;
    }

    switch (token) {
        case tcIdentifier:
        case tcNumber:
        case tcString:
            pNode = GetSymtabNode();
            strcpy(pToken->string, pNode->String());
            break;
        case tcEndOfFile:
            break;
        default:
            pNode = nullptr;
            strcpy(pToken->string, symbolStrings[(int) code]);
            break;
    }

    return pToken;
}

TSymtabNode *TIcode::GetSymtabNode(void) {
    extern TSymtab **vpSymtabs;
    short xSymtab, xNode;

    memcpy(&xSymtab, cursor, sizeof (short));
    memcpy(&xNode, cursor + sizeof (short),
            sizeof (short));

    cursor += (2 * sizeof (short));

    return vpSymtabs[xSymtab]->Get(xNode);
}

void TIcode::InsertLineMarker(void) {
    if (errorCount > 0) return;

    char lastCode;
    cursor -= sizeof (char);

    memcpy(&lastCode, cursor, sizeof (char));

    char code = mcLineMarker;
    short number = currentLineNumber;

    CheckBounds(sizeof (char) + sizeof (short));

    memcpy(cursor, &code, sizeof (char));
    cursor += sizeof (char);

    memcpy(cursor, &number, sizeof (short));
    cursor += sizeof (short);

    memcpy(cursor, &lastCode, sizeof (char));
    cursor += sizeof (char);
}