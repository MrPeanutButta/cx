#include <cstdio>
#include <iostream>
#include "common.h"
#include "error.h"
#include "buffer.h"
#include "symtable.h"
#include "types.h"
#include "icode.h"

int asmLabelIndex(0);
bool xrefFlag(false);

TDefn::~TDefn() {
    switch (how) {
        case dcProgram:
        case dcProcedure:
        case dcFunction:

            if (routine.which == rcDeclared) {
                delete routine.pSymtab;
                delete routine.pIcode;
            }
            break;

        default: break;
    }
}

void TSymtab::Convert(TSymtab* vpSymtabs[]) {
    vpSymtabs[xSymtab] = this;

    vpNodes = new TSymtabNode *[cntNodes];
    root->Convert(vpNodes);
}

void TSymtabNode::Convert(TSymtabNode* vpNodes[]) {
    if (left) left->Convert(vpNodes);

    vpNodes[xNode] = this;

    if (right) right->Convert(vpNodes);
}

TSymtabNode::TSymtabNode(const char* pStr, TDefnCode dc) :
pString(new char[strlen(pStr) + 1]), xNode(0),
defn(dc), level(currentNestingLevel), labelIndex(++asmLabelIndex) {
    left = right = next = nullptr;
    pLineNumList = nullptr;

    strcpy(pString, pStr);

    if (xrefFlag)pLineNumList = new TLineNumList;
}

TSymtabNode::~TSymtabNode() {

    void RemoveType(TType *&Type);

    delete left;
    delete right;
    delete [] pString;
    delete pLineNumList;
    RemoveType(pType);
}

void TSymtabNode::Print(void) const {
    const int maxNamePrintWidth(16);

    if (left != nullptr) left->Print();

    sprintf(::list.text, "%*s", maxNamePrintWidth, pString);

    if (pLineNumList != nullptr) {
        pLineNumList->Print(strlen(pString) > maxNamePrintWidth,
                maxNamePrintWidth);
    } else list.PutLine();

    PrintIdentifier();

    if (right != nullptr) right->Print();

}

void TSymtabNode::PrintIdentifier(void) const {
    switch (defn.how) {
        case dcConstant: PrintConstant();
            break;
        case dcType: PrintType();
            break;
        case dcVariable:
        case dcField: PrintVarOrField();
            break;
    }
}

void TSymtabNode::PrintConstant(void) const {
    extern TListBuffer list;

    list.PutLine();
    list.PutLine("defined constant");

    if ((pType == pIntegerType) ||
            (pType->form == fcEnum)) {

        sprintf(list.text, "value = %d", defn.constant.value.__int);
    } else if (pType == pFloatType) {
        sprintf(list.text, "value = %g", defn.constant.value.__float);
    } else if (pType == pDoubleType) {
        sprintf(list.text, "value = %lf", defn.constant.value.__double);
    } else if (pType == pCharType) {
        sprintf(list.text, "value = '%c'", defn.constant.value.__char);
    } else if (pType->form == fcArray) {
        sprintf(list.text, "value = '%s'", defn.constant.value.pString);
    }

    list.PutLine();

    if (pType) pType->PrintTypeSpec(TType::vcTerse);
    list.PutLine();
}

void TSymtabNode::PrintVarOrField(void) const {
    extern TListBuffer list;

    list.PutLine();
    list.PutLine(defn.how == dcVariable ? "declared variable" : "declared record field");

    if (pType)pType->PrintTypeSpec(TType::vcTerse);
    if ((defn.how == dcVariable) || (this->next)) list.PutLine();
}

void TSymtabNode::PrintType(void) const {
    list.PutLine();
    list.PutLine("defined type");

    if (pType) pType->PrintTypeSpec(TType::vcVerbose);
    list.PutLine();
}

TSymtabNode *TSymtab::Search(const char* pString) const {
    TSymtabNode *pNode = root;
    int comp(-1);

    while (pNode) {
        comp = strcmp(pString, pNode->pString);
        if (comp == 0) break;

        pNode = comp < 0 ? pNode->left : pNode->right;
    }

    if (xrefFlag && (comp == 0)) pNode->pLineNumList->Update();

    return pNode;
}

TSymtabNode *TSymtab::Enter(const char* pString, TDefnCode dc) {
    TSymtabNode *pNode;
    TSymtabNode **ppNode = &root;

    while ((pNode = *ppNode) != nullptr) {
        int comp = strcmp(pString, pNode->pString);
        if (comp == 0) return pNode;

        ppNode = comp < 0 ? &(pNode->left) : &(pNode->right);
    }

    pNode = new TSymtabNode(pString, dc);
    pNode->xSymtab = xSymtab;
    pNode->xNode = cntNodes++;
    *ppNode = pNode;

    return pNode;
}

TSymtabNode *TSymtab::EnterNew(const char *pString, TDefnCode dc) {
    TSymtabNode *pNode = Search(pString);

    if (!pNode) pNode = Enter(pString, dc);
    else Error(errRedefinedIdentifier);

    return pNode;
}

TLineNumList::~TLineNumList() {
    while (head != nullptr) {
        TLineNumNode *pNode = head;
        head = head->next;
        delete pNode;
        pNode = nullptr;
    }
}

void TLineNumList::Update(void) {
    if (tail && (tail->number == currentLineNumber)) return;

    tail->next = new TLineNumNode;
    tail = tail->next;
}

void TLineNumList::Print(int newLineFlag, int indent) const {
    const int maxLineNumberPrintWidth(4);
    const int maxLineNumbersPerLine(10);

    int n(0);
    TLineNumNode *pNode;
    char *plt = &list.text[strlen(list.text)];

    n = newLineFlag ? 0 : maxLineNumbersPerLine;

    for (pNode = head; pNode; pNode = pNode->next) {
        if (n == 0) {
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