//extern TSymtab globalSymtab;
//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R   (Header)                              *
//  *                                                           *
//  *   CLASSES: TStackItem, TRuntimeStack, TExecutor           *
//  *                                                           *
//  *   FILE:    prog10-1/exec.h                                *
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef exec_h
#define exec_h

#include "error.h"
#include "symtable.h"
#include "types.h"
#include "icode.h"
#include "backend.h"

//--------------------------------------------------------------
//  TStackItem          Item pushed onto the runtime stack.
//--------------------------------------------------------------

union TStackItem {
    int    __int;
    float  __float;
    double __double;
    char   __char;
    void  *__addr;
};

//--------------------------------------------------------------
//  TRuntimeStack       Runtime stack class.
//--------------------------------------------------------------

class TRuntimeStack {
    enum {
	stackSize       = 128,
	frameHeaderSize =   5,
    };

    //--Stack frame header
    struct TFrameHeader {
	TStackItem functionValue;
	TStackItem staticLink;
	TStackItem dynamicLink;

	struct {
	    TStackItem icode;
	    TStackItem location;
	} returnAddress;
    };

    TStackItem  stack[stackSize];  // stack items
    TStackItem *tos;               // ptr to the top of the stack
    TStackItem *pFrameBase;        // ptr to current stack frame base

public:
    TRuntimeStack(void);

    void Push(int value)
    {
	if (tos < &stack[stackSize-1]) (++tos)->__int = value;
	else RuntimeError(rteStackOverflow);
    }

    void Push(float value)
    {
	if (tos < &stack[stackSize-1]) (++tos)->__float = value;
	else RuntimeError(rteStackOverflow);
    }

    void Push(char value)
    {
	if (tos < &stack[stackSize-1]) (++tos)->__char = value;
	else RuntimeError(rteStackOverflow);
    }

    void Push(void *addr)
    {
	if (tos < &stack[stackSize-1]) (++tos)->__addr = addr;
	else RuntimeError(rteStackOverflow);
    }

    TStackItem *PushFrameHeader(int oldLevel, int newLevel,
				TIcode *pIcode);
    void ActivateFrame(TStackItem *pNewFrameBase, int location);
    void PopFrame     (const TSymtabNode *pRoutineId, TIcode *&pIcode);

    TStackItem *Pop(void)       { return tos--; }
    TStackItem *TOS(void) const { return tos;   }

    void AllocateValue  (const TSymtabNode *pId);
    void DeallocateValue(const TSymtabNode *pId);

    TStackItem *GetValueAddress(const TSymtabNode *pId);
};

//--------------------------------------------------------------
//  TExecutor           Executor subclass of TBackend.
//--------------------------------------------------------------

class TExecutor : public TBackend {
    long          stmtCount;  // count of executed statements
    TRuntimeStack runStack;   // ptr to runtime stack

    int eofFlag;  // true if at end of file, else false

    //--Trace flags
    int traceRoutineFlag;    // true to trace routine entry/exit
    int traceStatementFlag;  // true to trace statements
    int traceStoreFlag;      // true to trace data stores
    int traceFetchFlag;      // true to trace data fetches

    //--Routines
    void   ExecuteRoutine(const TSymtabNode *pRoutineId);
    void   EnterRoutine  (const TSymtabNode *pRoutineId);
    void   ExitRoutine   (const TSymtabNode *pRoutineId);
    TType *ExecuteSubroutineCall        (const TSymtabNode *pRoutineId);
    TType *ExecuteDeclaredSubroutineCall(const TSymtabNode *pRoutineId);
    TType *ExecuteStandardSubroutineCall(const TSymtabNode *pRoutineId);
    void   ExecuteActualParameters      (const TSymtabNode *pRoutineId);

    //--Statements
    void ExecuteStatement(void);
    void ExecuteStatementList(TTokenCode terminator);
    void ExecuteAssignment(const TSymtabNode *pTargetId);
    void ExecuteDO(void);
    void ExecuteWHILE(void);
    void ExecuteIF(void);
    void ExecuteFOR(void);
    void ExecuteSWITCH(void);
    void ExecuteCompound(void);

    //--Expressions
    TType *ExecuteExpression(void);
    TType *ExecuteSimpleExpression(void);
    TType *ExecuteTerm(void);
    TType *ExecuteFactor(void);
    TType *ExecuteConstant  (const TSymtabNode *pId);
    TType *ExecuteVariable  (const TSymtabNode *pId, int addressFlag);
    TType *ExecuteSubscripts(const TType *pType);
    TType *ExecuteField(void);
    //TType *ExecuteSuffix(void)

    //--Tracing
    void TraceRoutineEntry(const TSymtabNode *pRoutineId);
    void TraceRoutineExit (const TSymtabNode *pRoutineId);
    void TraceStatement(void);
    void TraceDataStore(const TSymtabNode *pTargetId,
			const void        *pDataValue,
			const TType       *pDataType);
    void TraceDataFetch(const TSymtabNode *pId,
			const void        *pDataValue,
			const TType       *pDataType);
    void TraceDataValue(const void        *pDataValue,
			const TType       *pDataType);

    void RangeCheck(const TType *pTargetType, int value);

    void Push(int    value) { runStack.Push(value); }
    void Push(float  value) { runStack.Push(value); }
    void Push(char   value) { runStack.Push(value); }
    void Push(void  *addr)  { runStack.Push(addr);  }

    TStackItem *Pop(void)       { return runStack.Pop(); }
    TStackItem *TOS(void) const { return runStack.TOS(); }

public:
    TExecutor(void)
    {
	stmtCount = 0;

	traceRoutineFlag   = true;
	traceStatementFlag = true;
	traceStoreFlag     = true;
	traceFetchFlag     = true;
    }

    virtual void Go(const TSymtabNode *pProgramId);
};

#endif