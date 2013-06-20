#include <cstdio>
#include <iostream>
#include "buffer.h"
#include "symtable.h"

bool xrefFlag(false);

TSymtabNode::TSymtabNode(const char* pStr):
 pString(new char[strlen(pStr) + 1]), xNode(0), value(0.0){
    left = right = nullptr;
    pLineNumList = nullptr;
    
    strcpy(pString, pStr);
    
    if(xrefFlag)pLineNumList = new TLineNumList;
}

TSymtabNode::~TSymtabNode(){
    delete left;
    delete right;
    delete [] pString;
    delete pLineNumList;
}

void TSymtabNode::Print(void) const {
    const int maxNamePrintWidth(16);
    
    if(left != nullptr) left->Print();
    
    sprintf(::list.text, "%*s", maxNamePrintWidth, pString);
    
    if(pLineNumList != nullptr){
        pLineNumList->Print(strlen(pString) > maxNamePrintWidth,
                maxNamePrintWidth);
    }else list.PutLine();
    
    if(right != nullptr) right->Print();
    
}

TSymtabNode *TSymtab::Search(const char* pString) const{
    TSymtabNode *pNode = root;
    int comp(-1);
    
    while(pNode){
        comp = strcmp(pString, pNode->pString);
        if(comp ==0) break;
        
        pNode = comp < 0 ? pNode->left : pNode->right;
    }
    
    if(xrefFlag && (comp == 0)) pNode->pLineNumList->Update();
    
    return pNode;
}

TSymtabNode *TSymtab::Enter(const char* pString){
    TSymtabNode *pNode;
    TSymtabNode **ppNode = &root;
    
    while((pNode = *ppNode) != nullptr){
        int comp = strcmp(pString, pNode->pString);
        if(comp == 0) return pNode;
        
        ppNode = comp < 0 ? &(pNode->left) : &(pNode->right);
    }
    
    pNode = new TSymtabNode(pString);
    pNode->xSymtab = xSymtab;
    pNode->xNode = cntNodes++;
    *ppNode = pNode;
    
    return pNode;
}

TLineNumList::~TLineNumList(){
    while(head != nullptr){
        TLineNumNode *pNode = head;
        head = head->next;
        delete pNode;
        pNode = nullptr;
    }
}

void TLineNumList::Update(void){
    if(tail && (tail->number == currentLineNumber)) return;
    
    tail->next = new TLineNumNode;
    tail = tail->next;
}

void TLineNumList::Print(int newLineFlag, int indent) const{
    const int maxLineNumberPrintWidth(4);
    const int maxLineNumbersPerLine(10);
    
    int n(0);
    TLineNumNode *pNode;
    char *plt = &list.text[strlen(list.text)];
    
    n = newLineFlag ? 0 : maxLineNumbersPerLine;
    
    for(pNode = head; pNode; pNode = pNode->next){
        if(n ==0){
            list.PutLine();
            sprintf(list.text, "%s", indent, " ");
            plt = &list.text[indent];
            n = maxLineNumbersPerLine;
        }
        
        sprintf(plt, "%*d", maxLineNumberPrintWidth, pNode->number);
        plt += maxLineNumberPrintWidth;
        --n;
    }
    
    list.PutLine();
    
}