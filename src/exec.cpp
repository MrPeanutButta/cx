//fig 9-7
//  *************************************************************
//  *                                                           *
//  *   E X E C U T O R                                         *
//  *                                                           *
//  *   Execute the intermediate code.                          *
//  *                                                           *
//  *   CLASSES: TRuntimeStack, TExecutor                       *
//  *                                                           *
//  *   FILE:    prog9-1/exec.cpp                               *
//  *                                                           *
//  *   MODULE:  Executor                                       *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#include <iostream>
#include "exec.h"

using namespace std;

//              *******************
//              *                 *
//              *  Runtime Stack  *
//              *                 *
//              *******************

//--------------------------------------------------------------
//  Constructor
//--------------------------------------------------------------

TRuntimeStack::TRuntimeStack(void) {

	memset(&stack, 0, sizeof(stack));

    tos = &stack[-1]; // point to just below bottom of stack
    pFrameBase = &stack[ 0]; // point to bottom of stack

    //--Initialize the program's stack frame at the bottom.
    Push(0); // function return value
    Push(0); // static  link
    Push(0); // dynamic link
    Push(0); // return address icode pointer
    Push(0); // return address icode location
}

//--------------------------------------------------------------
//  PushFrameHeader     Push the callee subroutine's stack frame
//                      header onto the runtime stack.  (Leave
//                      it inactive.)
//
//      oldLevel : nesting level of the caller routine
//      newLevel : nesting level of the callee subroutine's
//                 formal parameters and local variables
//      pIcode   : ptr to caller's intermediate code
//
//  Return: ptr to the base of the callee's stack frame
//--------------------------------------------------------------

TStackItem *TRuntimeStack::PushFrameHeader(int oldLevel, int newLevel,
        TIcode *pIcode) {
    TFrameHeader *pHeader = (TFrameHeader *) pFrameBase;
    TStackItem *pNewFrameBase = tos + 1; // point to item just above
    //   current TOS item

    Push(0); // function return value (placeholder)

    //--Compute the static link.
    if (newLevel == oldLevel + 1) {

        //--Callee nested within caller:
        //--Push address of caller's stack frame.
        Push(pHeader);
    } else if (newLevel == oldLevel) {

        //--Callee at same level as caller:
        //--Push address of common parent's stack frame.
        Push(pHeader->staticLink.__addr);
    } else /* newLevel < oldLevel */ {

        //--Callee nested less deeply than caller:
        //--Push address of nearest commmon ancestor's stack frame.
        int delta = oldLevel - newLevel;

        while (delta-- >= 0) {
            pHeader = (TFrameHeader *) pHeader->staticLink.__addr;
        }
        Push(pHeader);
    }

    Push(pFrameBase); // dynamic link
    Push(pIcode); // return address icode pointer
    Push(0); // return address icode location (placeholder)

    return pNewFrameBase;
}

//--------------------------------------------------------------
//  ActivateFrame       Activate the newly-allocated stack frame
//                      by pointing the frame base pointer to it
//                      and setting the return address location.
//
//      pNewFrameBase : ptr to the new stack frame base
//      location      : return address location
//--------------------------------------------------------------

void TRuntimeStack::ActivateFrame(TStackItem *pNewFrameBase,
        int location) {
    pFrameBase = pNewFrameBase;
    ((TFrameHeader *) pFrameBase)->returnAddress
            .location.__int = location;
}

//--------------------------------------------------------------
//  PopFrame    Pop the current frame from the runtime stack.
//              If it's for a function, leave the return value
//              on the top of the stack.
//
//      pRoutineId : ptr to subroutine name's symbol table node
//      pIcode     : ref to ptr to caller's intermediate code
//--------------------------------------------------------------

void TRuntimeStack::PopFrame(const TSymtabNode *pRoutineId,
        TIcode *&pIcode) {
    TFrameHeader *pHeader = (TFrameHeader *) pFrameBase;

    //--Don't do anything if it's the bottommost stack frame.
    if (pFrameBase != &stack[0]) {

        //--Return to the caller's intermediate code.
        pIcode = (TIcode *) pHeader->returnAddress.icode.__addr;
        pIcode->GoTo(pHeader->returnAddress.location.__int);

        //--Cut the stack back.  Leave a function value on top.
        tos = (TStackItem *) pFrameBase;
        if (pRoutineId->defn.how != dcFunction) --tos;
        pFrameBase = (TStackItem *) pHeader->dynamicLink.__addr;
    }
}

//--------------------------------------------------------------
//  AllocateValue       Allocate a runtime stack item for the
//                      value of a formal parameter or a local
//                      variable.
//
//      pId : ptr to symbol table node of variable or parm
//--------------------------------------------------------------

void TRuntimeStack::AllocateValue(TSymtabNode *pId) {
    TType *pType = pId->pType->Base(); // ptr to type object of value

    if ((pType->form != fcArray) && (pType->form != fcComplex)) {
        if (pType == pIntegerType) Push(0);
        else if (pType == pFloatType) Push(0.0f);
        else if (pType == pBooleanType) Push(0);
        else if (pType == pCharType) Push(0);
        else if (pType->form == fcEnum) Push(0);
    } else {

        //--Array or record
        void *addr = new char[pType->size];
        Push(addr);
        pId->pType->array.start_address = addr;
    }

    /* save runstack address.
	 * this negates the need to calculate the
	 * variables offset. */
    pId->runstackItem = TOS();
}

//--------------------------------------------------------------
//  DeallocateValue     Deallocate the data area of an array or
//                      record value of a formal value parameter
//                      or a local variable.
//
//      pId : ptr to symbol table node of variable or parm
//--------------------------------------------------------------

void TRuntimeStack::DeallocateValue(const TSymtabNode *pId) {
    if ((!pId->pType->IsScalar()) && (pId->defn.how != dcReference)) {
		TStackItem *pValue = pId->runstackItem;

		if(pValue->__addr != nullptr) delete[] pValue->__addr;
    }
}

//--------------------------------------------------------------
//  GetValueAddress     Get the address of the runtime stack
//                      item that contains the value of a formal
//                      parameter or a local variable.  If
//                      nonlocal, follow the static links to the
//                      appropriate stack frame.
//
//      pId : ptr to symbol table node of variable or parm
//
//  Return:  ptr to the runtime stack item containing the
//           variable, parameter, or function return value
//--------------------------------------------------------------

TStackItem *TRuntimeStack::GetValueAddress(const TSymtabNode *pId) {
    int functionFlag = pId->defn.how == dcFunction; // true if function
    //   else false
    TFrameHeader *pHeader = (TFrameHeader *) pFrameBase;

    //--Compute the difference between the current nesting level
    //--and the level of the variable or parameter.  Treat a function
    //--value as if it were a local variable of the function.  (Local
    //--variables are one level higher than the function name.)
    int delta = currentNestingLevel - pId->level;
    if (functionFlag) --delta;

    //--Chase static links delta times.
    while (delta-- > 0) {
        pHeader = (TFrameHeader *) pHeader->staticLink.__addr;
    }

    return functionFlag ? &pHeader->functionValue
            : pId->runstackItem; /*((TStackItem *) pHeader)
            + frameHeaderSize + pId->defn.data.offset;*/
}

//              **************
//              *            *
//              *  Executor  *
//              *            *
//              **************

//--------------------------------------------------------------
//  Go                  Start the executor.
//--------------------------------------------------------------

void TExecutor::Go(TSymtabNode *pProgramId) {
    //--Initialize standard input and output.
    eofFlag = cin.eof();
    cout.setf(ios::fixed, ios::floatfield);
    cout << endl;

    //--Execute the program.
    breakLoop = false;

    InitializeGlobal(pProgramId);
    //ExecuteRoutine(pProgramId);
    ExitRoutine(pProgramId);

    //--Print the executor's summary.
    cout << endl;
    cout << "Successful completion.  " << stmtCount
            << " statements executed." << endl;
}

//--------------------------------------------------------------
//  RangeCheck      Range check an assignment to a subrange.
//
//      pTargetType : ptr to target type object
//      value       : integer value to assign
//--------------------------------------------------------------

void TExecutor::RangeCheck(const TType *pTargetType, int value) {

    if ((pTargetType->form == fcSubrange)
            && ((value < pTargetType->subrange.min)
            || (value > pTargetType->subrange.max))) {
        RuntimeError(rteValueOutOfRange);
    }
}
//endfig

void TExecutor::InitializeGlobal(TSymtabNode* pProgramId) {

    //--Set up a new stack frame for the callee.
    TStackItem *pNewFrameBase = runStack.PushFrameHeader
            (0, 0, pProgramId->defn.routine.pIcode);

    //--Activate the new stack frame ...
    currentNestingLevel = 0;
    runStack.ActivateFrame(pNewFrameBase, pProgramId->defn.routine.returnMarker);

	EnterRoutine(pProgramId);
	GetToken();
	ExecuteStatement(pProgramId);
}
