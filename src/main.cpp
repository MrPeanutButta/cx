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

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

int main(int argc, char *argv[]) {

    extern bool xrefFlag;

    //--Check the command line arguments.
    if (argc < 2) {
        cerr << "Usage: Cx <source file>" << endl;
        AbortTranslation(abortInvalidCommandLineArgs);
    }

    listFlag = true;
    errorArrowFlag = true;
    xrefFlag = false;

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser *parser = new TParser(new TSourceBuffer(argv[1]));

    TSymtabNode *pProgramId = parser->Parse();
    delete parser;

    if (xrefFlag) {
        list.PutLine();
        list.PutLine("--x ref---");
        list.PutLine();
        globalSymtab.Print();
    }

    if (errorCount == 0) {
        vpSymtabs = new TSymtab *[cntSymtabs];
        for (TSymtab *pSt = pSymtabList; pSt; pSt = pSt->Next()) {
            pSt->Convert(vpSymtabs);
        }

        TBackend *pBackend = new TExecutor;
        pBackend->Go(pProgramId);

        delete[] vpSymtabs;
        delete pBackend;
    }

    return 0;
}