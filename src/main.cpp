//fig 3-23
//  *************************************************************
//  *                                                           *
//  *   Program 3-2:  Pascal Tokenizer                          *
//  *                                                           *
//  *   List the source file.  After each line, list the Pascal *
//  *   tokens that were extracted from that line.              *
//  *                                                           *
//  *   FILE: prog3-2/tokeniz2.cpp                              *
//  *                                                           *
//  *   USAGE:  tokeniz2 <source file>                          *
//  *                                                           *
//  *               <source file>  name of source file to       *
//  *                              tokenize                     *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <iostream>
#include "error.h"
#include "buffer.h"
#include "parser.h"
#include "symtable.h"
#include "common.h"
#include "icode.h"

TIcode *pIcode;

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

int main(int argc, char *argv[]) {

    extern bool xrefFlag;

    //--Check the command line arguments.
   /* if (argc != 2) {
        cerr << "Usage: token <source file>" << endl;
        AbortTranslation(abortInvalidCommandLineArgs);
    }*/

    listFlag = true;
    errorArrowFlag = false;
    xrefFlag = true;

    //pIcode = new TIcode(argv[2], TIcode::output);
    //--Create the parser for the source file,
    //--and then parse the file.
    TParser parser(new TSourceBuffer(argv[1]));
    parser.Parse();

    if(xrefFlag){
        list.PutLine();
        list.PutLine("--x ref---");
        list.PutLine();
        globalSymtab.Print();
    }

    //delete pIcode;

    return 0;
}
//endfig
