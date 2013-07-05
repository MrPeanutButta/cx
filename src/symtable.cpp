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
        case dcMember: PrintVarOrField();
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

//              ************************
//              *		       *
//              *  Symbol Table Stack  *
//              *		       *
//              ************************

//fig 8-6
//--------------------------------------------------------------
//  Constructor	    Initialize the global (level 0) symbol
//		    table, and set the others to NULL.
//--------------------------------------------------------------

TSymtabStack::TSymtabStack(void)
{
    extern TSymtab globalSymtab;
    void InitializeStandardRoutines(TSymtab *pSymtab);

    currentNestingLevel = 0;
    for (int i = 1; i < maxNestingLevel; ++i) pSymtabs[i] = NULL;

    //--Initialize the global nesting level.
    pSymtabs[0] = &globalSymtab;
    InitializePredefinedTypes (pSymtabs[0]);
    //InitializeStandardRoutines(pSymtabs[0]);
}

//--------------------------------------------------------------
//  Destructor	    Remove the predefined types.
//--------------------------------------------------------------

TSymtabStack::~TSymtabStack(void)
{
    RemovePredefinedTypes();      
}

//--------------------------------------------------------------
//  SearchAll   Search the symbol table stack for the given
//              name string.
//
//      pString : ptr to name string to find
//
//  Return: ptr to symbol table node if found, else NULL
//--------------------------------------------------------------

TSymtabNode *TSymtabStack::SearchAll(const char *pString) const
{
    for (int i = currentNestingLevel; i >= 0; --i) {
	TSymtabNode *pNode = pSymtabs[i]->Search(pString);
	if (pNode) return pNode;
    }

    return NULL;
}

//--------------------------------------------------------------
//  Find        Search the symbol table stack for the given
//              name string.  If the name is not already in
//              there, flag the undefined identifier error,
//		and then enter the name into the local symbol
//		table.
//
//      pString : ptr to name string to find
//
//  Return: ptr to symbol table node
//--------------------------------------------------------------

TSymtabNode *TSymtabStack::Find(const char *pString) const
{
    TSymtabNode *pNode = SearchAll(pString);

    if (!pNode) {
	Error(errUndefinedIdentifier);
	pNode = pSymtabs[currentNestingLevel]->Enter(pString);
    }

    return pNode;
}

//--------------------------------------------------------------
//  EnterScope	Enter a new nested scope.  Increment the nesting
//		level.  Push new scope's symbol table onto the
//		stack.
//
//      pSymtab : ptr to scope's symbol table
//--------------------------------------------------------------

void TSymtabStack::EnterScope(void)
{
    if (++currentNestingLevel > maxNestingLevel) {
	Error(errNestingTooDeep);
	AbortTranslation(abortNestingTooDeep);
    }

    SetCurrentSymtab(new TSymtab);
}

//--------------------------------------------------------------
//  ExitScope	Exit the current scope and return to the
//		enclosing scope.  Decrement the nesting level.
//		Pop the closed scope's symbol table off the
//		stack and return a pointer to it.
//
//  Return: ptr to closed scope's symbol table
//--------------------------------------------------------------

TSymtab *TSymtabStack::ExitScope(void)
{
    return pSymtabs[currentNestingLevel--];
}
//endfig