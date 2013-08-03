/** main
 * main.cpp
 * 
 * main entry point for the Cx interpretor.
 */
#include <iostream>

#ifdef __CX_PROFILE_EXECUTION__
#include <chrono>
#endif

#include "error.h"
#include "buffer.h"
#include "parser.h"
#include "symtable.h"
#include "common.h"
#include "icode.h"

// turn on to view Cx debugging
#ifdef __CX_DEBUG__
bool debugFlag = true;
#else
bool debugFlag = false;
#endif 

/** main        main entry point
 * 
 * @param argc
 * @param argv
 * @return exit status.
 */
int main(int argc, char *argv[]) {

    //--Check the command line arguments.
    if (argc < 2) {
        cerr << "usage: " << argv[0] << " <source file>" << endl;
        AbortTranslation(abortInvalidCommandLineArgs);
    }

    listFlag = debugFlag;
    errorArrowFlag = debugFlag;

    //--Create the parser for the source file,
    //--and then parse the file.
    TParser *parser = new TParser(new TSourceBuffer(argv[1]));

    
#ifdef __CX_PROFILE_EXECUTION__
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif
    
    TSymtabNode *pProgramId = parser->Parse();
    
#ifdef __CX_PROFILE_EXECUTION__
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast < duration<double >> (t2 - t1);
    std::cout << "finished parsing in: " << time_span.count() << "(secs)" << std::endl;
#endif
    
    delete parser;

    if (errorCount == 0) {
        vpSymtabs = new TSymtab *[cntSymtabs];
        for (TSymtab *pSt = pSymtabList; pSt; pSt = pSt->Next()) {
            if (pSt != nullptr) {
                if (pSt->Root() != nullptr)pSt->Convert(vpSymtabs);
            }
        }

        TBackend *pBackend = new TExecutor;

#ifdef __CX_PROFILE_EXECUTION__
        std::cin.get();
        t1 = high_resolution_clock::now();
#endif
        
        pBackend->Go(pProgramId);
        
#ifdef __CX_PROFILE_EXECUTION__
        t2 = high_resolution_clock::now();
        time_span = duration_cast < duration<double >> (t2 - t1);
        std::cout << "finished executing in: " << time_span.count() << "(secs)" << std::endl;
#endif
        
        delete[] vpSymtabs;
        delete pBackend;
    }

    return 0;
}