#include <cstdio>
#include "parser.h"

void TParser::ParseStatement(void) {
    if(token = tcIdentifier) ParseAssignment();
}

void TParser::ParseAssignment(void){
    TSymtabNode *pTargetNode = SearchAll(pToken->String());
    
    if(!pTargetNode) pTargetNode = EnterLocal(pToken->String());
    
    GetToken();
    
    // if identifier the unary assignment
    if(token == tcEqual)GetToken();
    else if( token == tcPlusPlus ) pTargetNode->value++;
    else Error(errMissingEqual);
    
    ParseExpression();
    
    pTargetNode->value = runStack.Pop();
    
    sprintf(::list.text, "\t> %s == %g", pTargetNode->String(), pTargetNode->value);
    
    list.PutLine();
}
