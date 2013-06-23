#include <iostream>
#include "exec.h"

using namespace std;

void TExecutor::ExecuteStatement(void){
    if(token != tcLBracket) ++stmtCount;
    
    switch (token) {
        case tcIdentifier: ExecuteAssignment();
            break;
        case tcDo: ExecuteDO();
            break;
        case tcWhile: //ParseWHILE();
            break;
        case tcIf: //ParseIF();
            break;
        case tcFor: //ParseFOR();
            break;
        case tcSwitch: //ParseSWITCH();
            break;
        case tcCase:
        case tcDefault://ParseCaseLabel();
            break;
        case tcBreak: //GetTokenAppend();
            break;
        case tcLBracket: ExecuteCompound();
            break;
        case tcReturn: //ParseRETURN();
            break;
    }
}

void TExecutor::ExecuteStatementList(TTokenCode terminator){
    do{
        ExecuteStatement();
        while(token == tcSemicolon) GetToken();
    }while(token != terminator);
}

void TExecutor::ExecuteAssignment(void){
    TSymtabNode *pTargetNode = pNode;
    
    GetToken(); // assignment operator
    //GetToken(); // first token expression
    
    switch (token) {
        case tcEqual:
            GetToken();
            ExecuteExpression();
            pTargetNode->value = runStack.Pop();
            break;
        case tcPlusPlus:
            ++pTargetNode->value;
            runStack.Push(pTargetNode->value);
            GetToken();
            break;
        case tcMinusMinus:
            --pTargetNode->value;
            runStack.Push(pTargetNode->value);
            GetToken();
            break;
        case tcPlusEqual:
            GetToken();
            ExecuteExpression();
            pTargetNode->value += runStack.Pop();
            break;
        case tcMinusEqual:
            GetToken();
            ExecuteExpression();
            pTargetNode->value -= runStack.Pop();
            break;
        case tcStarEqual:
            GetToken();
            ExecuteExpression();
            pTargetNode->value *= runStack.Pop();
            break;
        case tcForwardSlashEqual:
            GetToken();
            ExecuteExpression();
            pTargetNode->value /= runStack.Pop();
            break;
        case tcModEqual:
        {
            GetToken();
            ExecuteExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 %= __2);
        }
            break;
        case tcBitLeftShiftEqual:
        {
            GetToken();
            ExecuteExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 <<= __2);
        }
            break;
        case tcBitRightShiftEqual:
        {
            GetToken();
            ExecuteExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 >>= __2);
        }
            break;
        case tcBitANDEqual:
        {
            GetToken();
            ExecuteExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 &= __2);
        }
            break;
        case tcBitXOREqual:
        {
            GetToken();
            ExecuteExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 ^= __2);
        }
            break;
        case tcBitOREqual:
        {
            GetToken();
            ExecuteExpression();
            int __1 = (int) pTargetNode->value;
            int __2 = (int) runStack.Pop();

            pTargetNode->value = (__1 |= __2);
        }
            break;
        default:
            Error(errInvalidAssignment);
            break;
    }
    
    if(pTargetNode == pOutputNode){
        cout << " output == " << pTargetNode->value << endl;
    }
    
}

void TExecutor::ExecuteDO(void){
    
    int atLoopStart = CurrentLocation(); // location of loop start in icode
    
    do{
        GetToken();     // do
        
        ExecuteStatementList(tcWhile);
        
        GetToken();     //while
        ExecuteExpression();     // (condition)
        
        if(runStack.Pop() == 1.0)Goto(atLoopStart);
    }while (CurrentLocation() == atLoopStart);
}

void TExecutor::ExecuteCompound(void){
    GetToken();
    
    ExecuteStatementList(tcRBracket);
    
    GetToken();
}
