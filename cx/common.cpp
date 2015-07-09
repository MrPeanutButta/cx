/**
 * common.cpp
 *
 * Data and routines common to the front and back ends.
 *
 * NOTE:
 *      token lists below are subject to change drastically
 *      once the parser and scanner are complete. -aaron
 */

#include "common.h"

namespace cx{
	namespace common{


		/** Global symbols
		 *
		 * NOTE:
		 *      when writing the Cx stdlib, this global symtab should be visible
		 *      to the dynamic library to allow a library to add it's own symbols. - aaron
		 */
		symbols::symbol_table cx_global_symtab;
		symbols::symbol_table *p_symtab_list = nullptr;

		/// Tokens for resyncing the parser

}