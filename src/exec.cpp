#include <iostream>
#include "exec.h"

using namespace std;

void TExecutor::Go(void) {
    //icode.Reset();
    GetToken();

    do {
        ExecuteStatement();

        while (token == tcSemicolon)GetToken();

    } while (token != tcEndOfFile);

    cout << "\nsuccess: " << stmtCount << " statements executed.\n";
}
