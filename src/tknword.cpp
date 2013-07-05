//fig 3-17
//  *************************************************************
//  *                                                           *
//  *   T O K E N S   (Words)                                   *
//  *                                                           *
//  *   Extract word tokens from the source file.               *
//  *                                                           *
//  *   CLASSES: TWordToken                                     *
//  *                                                           *
//  *   FILE:    prog3-2/tknword.cpp                            *
//  *                                                           *
//  *   MODULE:  Scanner                                        *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <utility>
#include <cstring>
#include <cstdio>
#include "misc.h"
#include "token.h"

//              *************************
//              *                       *
//              *  Reserved Word Table  *
//              *                       *
//              *************************

const int minResWordLen = 2; // min and max reserved
const int maxResWordLen = 12; //   word lengths

//--------------------------------------------------------------
//  Reserved word lists (according to word length)
//--------------------------------------------------------------
std::pair<std::string, TTokenCode> map_data[] = {
    std::make_pair("if", tcIf),
    std::make_pair("return", tcReturn),
    std::make_pair("continue", tcContinue),
    std::make_pair("friend", tcFriend),
    //std::make_pair("true", tcTrue),
    std::make_pair("goto", tcGoto),
    std::make_pair("try", tcTry),
    std::make_pair("delete", tcDelete),
    std::make_pair("short", tcShort),
    std::make_pair("typeid", tcTypeId),
    //std::make_pair("bool", tcBool),
    std::make_pair("do", tcDo),
    //std::make_pair("int", tcInt),
    std::make_pair("signed", tcSigned),
    std::make_pair("typename", tcTypeName),
    std::make_pair("break", tcBreak),
    //std::make_pair("double", tcDouble),
    std::make_pair("long", tcLong),
    std::make_pair("sizeof", tcSizeOf),
    std::make_pair("union", tcUnion),
    std::make_pair("case", tcCase),
    std::make_pair("static", tcStatic),
    std::make_pair("unsigned", tcUnsigned),
    std::make_pair("catch", tcCatch),
    std::make_pair("else", tcElse),
    std::make_pair("namespace", tcNameSpace),
    std::make_pair("using", tcUsing),
    ///std::make_pair("char", tcCharDef),
    std::make_pair("enum", tcEnum),
    std::make_pair("new", tcNew),
    std::make_pair("virtual", tcVirtual),
    std::make_pair("char16_t", tcChar16_t),
    std::make_pair("explicit", tcExplicit),
    std::make_pair("noexcept", tcNoExcept),
    std::make_pair("char32_t", tcChar32_t),
    std::make_pair("export", tcExport),
    std::make_pair("nullptr", tcNullptr),
    std::make_pair("switch", tcSwitch),
    std::make_pair("struct", tcStruct),
    std::make_pair("void", tcVoid),
    // std::make_pair("class", tcClass),
    std::make_pair("extern", tcExtern),
    std::make_pair("operator", tcOperator),
    std::make_pair("template", tcTemplate),
    std::make_pair("const", tcConst),
    //std::make_pair("false", tcFalse),
    std::make_pair("private", tcPrivate),
    std::make_pair("this", tcThis),
    std::make_pair("while", tcWhile),
    //std::make_pair("float", tcFloat),
    std::make_pair("protected", tcProtected),
    std::make_pair("thread_local", tcThreadLocal),
    std::make_pair("for", tcFor),
    std::make_pair("public", tcPublic),
    std::make_pair("throw", tcThrow),
    std::make_pair("default", tcDefault),
    std::make_pair("typedef", tcTypeDef),
    std::make_pair("mutable", tcMutable),
    std::make_pair("include", tcInclude),
    //std::make_pair("string", tcStringDef),
    //std::make_pair("record", tcRecord),
    //std::make_pair("array", tcArray),
    //std::make_pair("of", tcOf),
    //std::make_pair("var", tcVar)
};


token_map TResWord(map_data,
        map_data + sizeof map_data / sizeof map_data[0]);

//              *****************
//              *               *
//              *  Word Tokens  *
//              *               *
//              *****************

//--------------------------------------------------------------
//  Get     Extract a word token from the source and downshift
//          its characters.  Check if it's a reserved word.
//
//      pBuffer : ptr to text input buffer
//--------------------------------------------------------------

void TWordToken::Get(TTextInBuffer &buffer) {
    extern char_code_map charCodeMap;

    char ch = buffer.Char(); // char fetched from input
    char *ps = string;

    //--Get the word.
    do {
        *ps++ = ch;
        ch = buffer.GetChar();
    } while ((charCodeMap[ch] == ccLetter)
            || (charCodeMap[ch] == ccDigit));

    *ps = '\0';

    CheckForReservedWord();
}

//--------------------------------------------------------------
//  CheckForReservedWord    Is the word token a reserved word?
//                          If yes, set the its token code to
//                          the appropriate code.  If not, set
//                          the token code to tcIdentifier.
//--------------------------------------------------------------

void TWordToken::CheckForReservedWord(void) {
    std::string __s(string);
    code = tcIdentifier; // first assume it's an identifier

    //--Is it the right length?
    if ((__s.length() >= minResWordLen) &&
            (__s.length() <= maxResWordLen)) {
        //--Yes.  Use the word length to pick the appropriate list
        //--from the reserved word table and check to see if the word
        //--is in there.
        TTokenCode __c = TResWord[__s];
        if (__c > 0) code = __c;

    }

}

//--------------------------------------------------------------
//  Print       Print the token to the list file.
//--------------------------------------------------------------

void TWordToken::Print(void) const {
    if (code == tcIdentifier) {
        sprintf(list.text, "\t%-18s %-s", ">> identifier:", string);
    } else {
        sprintf(list.text, "\t%-18s %-s", ">> reserved word:", string);
    }

    list.PutLine();
}
//endfig

