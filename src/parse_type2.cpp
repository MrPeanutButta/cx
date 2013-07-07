#include "common.h"
#include "parser.h"

TType *TParser::ParseArrayType(TSymtabNode *pArrayNode) {
    TType *pArrayType = pArrayNode->pType;
    //TType *pElmtType = pArrayNode->pType;
    //bool indexFlag(false);

    //GetToken();
    CondGetToken(tcLeftSubscript, errMissingLeftSubscript);

    pArrayNode->pType->form = fcArray;
    pArrayNode->pType->array.pElmtType = pArrayNode->pType;

    SetType(pArrayType->array.pElmtType, pArrayType->pTypeId->pType);
    SetType(pArrayType->array.pIndexType, pIntegerType);

    if (token == tcRightSubscript) {
        //CondGetToken(tcRightSubscript, errMissingRightSubscript);
        ParseAssignment(pArrayNode);
    } else {
        int max_index = pToken->Value().__int;
        CondGetToken(tcNumber, errInvalidNumber);
    }

    if (pArrayType->form != fcNone) {
        pArrayType->size = ArraySize(pArrayType);
    }

    return pArrayType;
}

void TParser::ParseIndexType(TSymtabNode *pArrayNode) {
    if (TokenIn(token, tlIndexStart)) {
        TType *pArrayType = pArrayNode->pType;
        TType *pIndexType = pIntegerType; //ParseTypeSpec(pArrayNode);

        SetType(pArrayType->array.pIndexType, pIndexType);

        switch (pIndexType->form) {
            case fcSubrange:
                pArrayType->array.elmtCount = pIndexType->subrange.max -
                        pIndexType->subrange.min + 1;
                pArrayType->array.minIndex = pIndexType->subrange.min;
                pArrayType->array.maxIndex = pIndexType->subrange.max;
                return;

            case fcEnum:
                pArrayType->array.elmtCount = pIndexType->enumeration.max + 1;
                pArrayType->array.minIndex = 0;
                pArrayType->array.maxIndex = pIndexType->enumeration.max;

                return;

            default:
                SetType(pArrayType->array.pIndexType, pIndexType);
                pArrayType->array.elmtCount = 0;
                pArrayType->array.minIndex = pArrayType->array.maxIndex = 0;
                //Error(errInvalidIndexType);
        }
    }
}

int TParser::ArraySize(TType* pArrayType) {
    if (pArrayType->array.pElmtType->size == 0) {
        pArrayType->array.pElmtType->size = ArraySize(pArrayType->array.pElmtType);
    }

    return (pArrayType->array.elmtCount * pArrayType->array.pElmtType->size);
}

TType *TParser::ParseComplexType(TSymtabNode *pRoutineId, TSymtabNode *pNode) {

    GetToken();

    //--<id>
    TType *pType = new TType(fcComplex, 0, nullptr);

    //TSymtabNode *next_type = pNode;

    //while (next_type->next)
    //next_type = next_type->next;

    TSymtabNode *pId = EnterNewLocal(pToken->String());

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
        GetToken();
        return;
    }

    // if '{' this is a class body

    CondGetToken(tcLBracket, errMissingLeftBracket);

    // scope
    TTokenCode scope;
    TSymtabNode *member = nullptr;

    // pointer to scoped table
    TSymtab *member_table = nullptr;
    
    TSymtabNode *pNode = nullptr;

    TSymtabNode *pLastId = nullptr; // ptrs to symtab nodes
    TSymtabNode *pFirstId = nullptr; // ptr to last node of previous sublist

    pComplexType->complex.MemberTable.insert(pair<TTokenCode, TSymtab *>(tcPublic, new TSymtab));
    pComplexType->complex.MemberTable.insert(pair<TTokenCode, TSymtab *>(tcPrivate, new TSymtab));
    pComplexType->complex.MemberTable.insert(pair<TTokenCode, TSymtab *>(tcProtected, new TSymtab));

    //default to public
    scope = tcPublic;

    do {

        // check scope and fast forward list to the end
        switch (token) {
            case tcPublic:
                scope = tcPublic;
                GetToken();
                CondGetToken(tcColon, errMissingColon);
                pFirstId = pComplexType->complex.MemberTable[scope]->Root();

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
                pFirstId = pComplexType->complex.MemberTable[scope]->Root();

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
                pFirstId = pComplexType->complex.MemberTable[scope]->Root();

                offset = 0;
                while (pFirstId) {
                    offset += pFirstId->pType->size;
                    pFirstId = pFirstId->next;
                }

                break;
        }

        member_table = pComplexType->complex.MemberTable[scope];

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

                GetToken();

                // check for array type
                // check for array type
                if (token == tcLeftSubscript) {
                    ParseArrayType(member);
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
    pComplexType->complex.pSymtabClassScope->ConnectTables(pComplexType->complex.MemberTable);
    
    CondGetToken(tcRBracket, errMissingRightBracket);
    CondGetToken(tcSemicolon, errMissingSemicolon);
}