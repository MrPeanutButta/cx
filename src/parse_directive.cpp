/** Parse Directives
 * parse_directive.cpp
 * 
 */

#include <cstdio>
#include "common.h"
#include "buffer.h"
#include "error.h"
#include "parser.h"

/** ParseDirective      Opens an external script module
 *                      for parsing.
 * 
 *      #include <identifier>
 * 
 * @param pRoutineId : ptr to the routine owning this directive call.
 */
void TParser::ParseDirective(TSymtabNode *pRoutineId) {

    switch (token) {
        case tcInclude:
        {
            GetTokenAppend();
            TParser *parser = new TParser(new TSourceBuffer(pToken->String()));

            TSymtabNode *pModule = parser->Parse();
            delete parser;
            GetTokenAppend();
        }
            break;
    }

}
