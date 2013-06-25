/*
 * File:   exec.h
 * Author: aaro3965
 *
 * Created on June 20, 2013, 12:18 AM
 */

#ifndef EXEC_H
#define	EXEC_H

#include <cstring>
#include "misc.h"
#include "error.h"
#include "symtable.h"
#include "backend.h"

extern TSymtab globalSymtab;

class TRuntimeStack {

    enum {
        stackSize = 32
    };

    float stack[stackSize];
    float *tos;

public:

    TRuntimeStack() {
        tos = &stack[-1];
        memset(&stack, 0, stackSize);
    }

    float Pop(void) {
        return *tos--;
    }

    void Push(float value) {
        if (tos < &stack[stackSize - 1]) *++tos = value;
        else RuntimeError(rteStackOverflow);
    }
};

class TExecutor : public TBackend {
    long stmtCount;
    TRuntimeStack runStack;

    // pointers to input/output Nodes
    TSymtabNode * const pInputNode;
    TSymtabNode * const pOutputNode;

    // statements
    void ExecuteStatement(void);
    void ExecuteAssignment(void);
    void ExecuteStatementList(TTokenCode terminator);
    void ExecuteDO(void);
    void ExecuteCompound(void);

    // expressions
    void ExecuteExpression(void);
    void ExecuteSuffix(TSymtabNode *pNode);
    void ExecuteSimpleExpression(void);
    void ExecuteTerm(void);
    void ExecuteFactor(void);

public:

    TExecutor()
    : pInputNode(globalSymtab.Search("input")),
    pOutputNode(globalSymtab.Search("output")),
    stmtCount(0) {
    }

    virtual void Go(void);
};

#endif	/* EXEC_H */

