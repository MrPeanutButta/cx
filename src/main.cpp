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
//#include <chrono>
#include "error.h"
#include "buffer.h"
#include "parser.h"
#include "symtable.h"
#include "common.h"
#include "icode.h"

bool debugFlag = true;

//--------------------------------------------------------------
//  main
//--------------------------------------------------------------

int main(int argc, char *argv[]) {

    extern bool xrefFlag;

    //--Check the command line arguments.
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <source file>" << endl;
        AbortTranslation(abortInvalidCommandLineArgs);
    }


    listFlag = debugFlag;
    errorArrowFlag = debugFlag;
    xrefFlag = false;

    //using namespace std::chrono;

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser *parser = new TParser(new TSourceBuffer(argv[1]));

    //high_resolution_clock::time_point t1 = high_resolution_clock::now();
    TSymtabNode *pProgramId = parser->Parse();
    //high_resolution_clock::time_point t2 = high_resolution_clock::now();
    //duration<double> time_span = duration_cast < duration<double >> (t2 - t1);
    //std::cout << "finished parsing in: " << time_span.count() << "(secs)" << std::endl;

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
            if (pSt != nullptr) {
                if (pSt->Root() != nullptr)pSt->Convert(vpSymtabs);
            }
        }

        std::cin.get();

        TBackend *pBackend = new TExecutor;

        //t1 = high_resolution_clock::now();
        pBackend->Go(pProgramId);
        //t2 = high_resolution_clock::now();
        //time_span = duration_cast < duration<double >> (t2 - t1);
        //std::cout << "finished executing in: " << time_span.count() << "(secs)" << std::endl;

        delete[] vpSymtabs;
        delete pBackend;
    }

    return 0;
}