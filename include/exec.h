/* 
 * File:   exec.h
 * Author: aaro3965
 *
 * Created on June 20, 2013, 12:18 AM
 */

#ifndef EXEC_H
#define	EXEC_H

#include "common.h"

class TRuntimeStack{
    enum {stackSize = 32};
    
    float stack[stackSize];
    float *tos;
    
public:
    TRuntimeStack(){ tos = &stack[-1];}
    
    float Pop(void){return *tos--;}
    
    void Push(float value){
        if(tos < &stack[stackSize - 1]) *++tos = value;
        else AbortTranslation(abortStackOverflow);
    }
   
};

#endif	/* EXEC_H */

