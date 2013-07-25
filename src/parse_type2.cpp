#include "common.h"
#include "parser.h"

TType *TParser::ParseArrayType(TSymtabNode *pRoutineId, TSymtabNode *pArrayNode) {
    TType *pArrayType = new TType(fcArray, 0, nullptr);
    TType *pElmtType = pArrayType;

    //--Final element type.
    SetType(pElmtType->array.pElmtType, pArrayNode->pType);

    CondGetTokenAppend(tcLeftSubscript, errMissingLeftSubscript);


    if (token == tcRightSubscript) {
        // xxx fixme, need a way to get out of assignment
        ParseAssignment(pArrayNode);
    } else {
        int min_index = 0;
        int max_index = pToken->Value().__int;
        SetType(pElmtType->array.pIndexType, pIntegerType);
        pArrayType->array.elmtCount = max_index;
        pArrayType->array.minIndex = min_index;
        pArrayType->array.maxIndex = max_index - 1;

        ParseExpression();
    }

    if (pArrayType->form != fcNone) {
        pArrayType->size = ArraySize(pArrayType);
    }

    CondGetTokenAppend(tcRightSubscript, errMissingRightSubscript);

    //pArrayType->array.pElmtType->size = ArraySize(pArrayType);
    SetType(pArrayNode->pType, pArrayType);
    pArrayNode->defn.how = dcVariable;

    //add to routines variable list
    if (pRoutineId) {
        TSymtabNode *__array = pRoutineId->defn.routine.locals.pTypeIds;
        if (!__array) {
            pRoutineId->defn.routine.locals.pTypeIds = pArrayNode;
        } else {
            while (__array->next)__array = __array->next;

            __array->next = pArrayNode;
        }
    }

    //--If the type object doesn't have a name yet,
    //--point it to the type id.
    if (!pArrayNode->pType->pTypeId) {
        pArrayNode->pType->pTypeId = pArrayNode;
    }

    return pArrayType;
}

int TParser::ArraySize(TType * pArrayType) {
    if (pArrayType->array.pElmtType->size == 0) {
        pArrayType->array.pElmtType->size = ArraySize(pArrayType->array.pElmtType);
    }

    return (pArrayType->array.elmtCount * pArrayType->array.pElmtType->size);
}

TType * TParser::ParseComplexType(TSymtabNode *pRoutineId, TSymtabNode * pNode) {

    GetToken();

    //--<id>
    TType *pType = new TType(fcComplex, 0, nullptr);

    //TSymtabNode *next_type = pNode;

    //while (next_type->next)
    //next_type = next_type->next;

    TSymtabNode *pId = EnterNewLocal(pToken->String());
    icode.Put(pId);

    //next_type->next = pId;

    //--Link the routine's local type id nodes together.
    if (!pRoutineId->defn.routine.locals.pTypeIds) {
        pRoutineId->defn.routine.locals.pTypeIds = pId;
    } else {
        TSymtabNode *__var = pRoutineId->defn.routine.locals.pTypeIds;

        while (__var->next)
            __var = __var->next;

        __var->next = pId;
    }

    SetType(pId->pType, pType);
    pId->defn.how = dcType;

    ParseMemberDecls(pRoutineId, pId->pType, 0);

    return pId->pType;
}

void TParser::ParseMemberDecls(TSymtabNode *pRoutineId, TType *pComplexType, int offset) {
    // copy of base class would go here

    // if no '{' this must be a forward

    GetToken();

    if (token == tcSemicolon) {
        GetTokenAppend();
        return;
    }

    // if '{' this is a class body

    CondGetTokenAppend(tcLBracket, errMissingLeftBracket);

    // scope
    TTokenCode scope;
    TSymtabNode *member = nullptr;

    // pointer to scoped table
    TSymtab *member_table = nullptr;

    TSymtabNode *pNode = nullptr;

    TSymtabNode *pLastId = nullptr; // ptrs to symtab nodes
    TSymtabNode *pFirstId = nullptr; // ptr to last node of previous sublist

    //    pComplexType->complex.MemberTable.insert(pair<TTokenCode, TSymtab *>(tcPublic, new TSymtab));
    //   pComplexType->complex.MemberTable.insert(pair<TTokenCode, TSymtab *>(tcPrivate, new TSymtab));
    //   pComplexType->complex.MemberTable.insert(pair<TTokenCode, TSymtab *>(tcProtected, new TSymtab));

    //default to public
    scope = tcPublic;

    do {

        // check scope and fast forward list to the end
        switch (token) {
            case tcPublic:
                scope = tcPublic;
                GetToken();
                CondGetToken(tcColon, errMissingColon);
                ///pFirstId = pComplexType->complex.MemberTable[scope]->Root();

                offset = 0;
                while (pFirstId) {
                    offset += pFirstId->pType->size;
                    pFirstId = pFirstId->next;
                }

                break;
            case tcPrivate:
                scope = tcPrivate;
                GetToken();
                CondGetToken(tcColon, errMissingColon);
                //pFirstId = pComplexType->complex.MemberTable[scope]->Root();

                offset = 0;
                while (pFirstId) {
                    offset += pFirstId->pType->size;
                    pFirstId = pFirstId->next;
                }

                break;
            case tcProtected:
                scope = tcProtected;
                GetToken();
                CondGetToken(tcColon, errMissingColon);
                // pFirstId = pComplexType->complex.MemberTable[scope]->Root();

                offset = 0;
                while (pFirstId) {
                    offset += pFirstId->pType->size;
                    pFirstId = pFirstId->next;
                }

                break;
        }

        //member_table = pComplexType->complex.MemberTable[scope];

        // find our declared type
        pNode = Find(pToken->String());

        // if complex then this is an object
        if (pNode->pType->form == fcComplex) {
            ParseComplexType(pRoutineId, pNode);
            // predefined type name found
        } else if ((pNode->defn.how == dcType) && (pNode->pType->form != fcComplex)) {
            do {
                GetToken();

                // enter new local
                member = member_table->EnterNew(pToken->String());
                member->defn.how = dcMember;

                // set type
                SetType(member->pType, pNode->pType);

                //--Record fields
                member->defn.data.offset = offset;
                offset += pNode->pType->size;

                GetTokenAppend();

                // check for array type
                // check for array type
                if (token == tcLeftSubscript) {
                    //ParseArrayType(member);
                    member->defn.how = dcVariable;
                } else if (token == tcLParen) {
                    ParseFunctionHeader(member);
                } else if (token != tcComma) {
                    // check for assignment
                    //ParseAssignment(pNewId);
                    member->defn.how = dcVariable;
                }

                if (!pFirstId) pFirstId = pLastId = member;
                else {
                    pLastId->next = member;
                    pLastId = member;
                }

            } while (token == tcComma);

            CondGetToken(tcSemicolon, errMissingSemicolon);
        }



    } while (token != tcRBracket);

    // connect all symtabs for use within the class
    pComplexType->complex.pSymtabClassScope = new TSymtab;
    // pComplexType->complex.pSymtabClassScope->ConnectTables(pComplexType->complex.MemberTable);

    CondGetTokenAppend(tcRBracket, errMissingRightBracket);
    CondGetTokenAppend(tcSemicolon, errMissingSemicolon);
}