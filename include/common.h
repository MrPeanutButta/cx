//fig 2-7
//  *************************************************************
//  *                                                           *
//  *   C O M M O N   (Header)                                  *
//  *                                                           *
//  *   FILE:    prog2-1/common.h                               *
//  *                                                           *
//  *   MODULE:  Common                                         *
//  *                                                           *
//  *   Copyright (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef common_h
#define common_h

#include "misc.h"
#include "symtable.h"
#include "icode.h"

extern int currentLineNumber;
extern int currentNestingLevel;

extern TSymtab globalSymtab;

// tokens that can start a statement
extern const TTokenCode tlStatementStart[];

// tokens that can follow a statement
extern const TTokenCode tlStatementFollow[];

extern const TTokenCode tlCaseLabelStart[];
extern const TTokenCode tlExpressionStart[];

extern const TTokenCode tlExpressionFollow[];

extern const TTokenCode tlRelOps[];

extern const TTokenCode tlUnaryOps[];

extern const TTokenCode tlAddOps[];

extern const TTokenCode tlMulOps[];

extern const TTokenCode tlProgramEnd[];

extern const TTokenCode tlEqualEqual[];
extern const TTokenCode tlDo[];
extern const TTokenCode tlLBracket [];
extern const TTokenCode tlColonp[];
extern const TTokenCode tlRBracket[];
extern const TTokenCode tlSemicolon[];
extern const TTokenCode tlRParen[];
extern const TTokenCode tlLParen[];


bool TokenIn(TTokenCode tc, const TTokenCode *pList);

#endif
//endfig

