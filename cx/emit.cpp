/*
The MIT License (MIT)

Copyright (c) 2015 Aaron Hebert <aaron.hebert@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "parser.h"

namespace cx {

	void parser::emit_const(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id) {
		opcode op;

		switch (p_id->p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_INT:
			op = ICONST;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant_value.i_ });
			break;
		case T_CHAR:
			op = ICONST;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant_value.c_ });
			break;
		case T_DOUBLE:
			op = DCONST;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant_value.d_ });
			break;
		case T_REFERENCE:
			op = ACONST_NULL;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant_value.a_ });
			break;
		default:
			break;
		}
	}

	void parser::emit_not_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = INOT_EQ;
			break;
		case T_DOUBLE:
			op = DNOT_EQ;
			break;
		case T_REFERENCE: // TODO fix
						  //op = ANOT_EQ;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_lt_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ILT_EQ;
			break;
		case T_DOUBLE:
			op = DLT_EQ;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}


	void parser::emit_eq_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BYTE:
			op = opcode::BEQ;
			break;
		case T_BOOLEAN:
			op = opcode::ZEQ;
			break;
		case T_CHAR:
		case T_INT:
			op = opcode::IEQ;
			break;
		case T_DOUBLE:
			op = opcode::DEQ;
			break;
		case T_REFERENCE: // TODO fix
						  //op = ANOT_EQ;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_gt(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IGT;
			break;
		case T_DOUBLE:
			op = DGT;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_gt_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IGT_EQ;
			break;
		case T_DOUBLE:
			op = DGT_EQ;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_lt(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ILT;
			break;
		case T_DOUBLE:
			op = DLT;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_add(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IADD;
			break;
		case T_DOUBLE:
			op = DADD;
			break;
		case T_REFERENCE: // TODO fix
			op = ASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_div(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IDIV;
			break;
		case T_DOUBLE:
			op = DDIV;
			break;
		case T_REFERENCE: // TODO fix
			op = ASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_sub(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ISUB;
			break;
		case T_DOUBLE:
			op = DSUB;
			break;
		case T_REFERENCE: // TODO fix
			op = ASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_mod(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IREM;
			break;
		case T_DOUBLE:
			op = DREM;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_mul(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IMUL;
			break;
		case T_DOUBLE:
			op = DMUL;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_lnot(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {
		cx::opcode op = LOGIC_NOT;

		switch (p_type->typecode)
		{
		case T_DOUBLE:
			p_function_id->defined.routine.program_code.push_back(D2I);
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit(symbol_table_node_ptr &p_function_id, cx::opcode op1) {
		p_function_id->defined.routine.program_code.push_back(op1);
	}

	void parser::emit(symbol_table_node_ptr &p_function_id, cx::opcode op1, value arg1) {
		p_function_id->defined.routine.program_code.push_back({ op1, arg1 });
	}

	void parser::emit(symbol_table_node_ptr &p_function_id, cx::opcode op1, value arg1, value arg2) {
		p_function_id->defined.routine.program_code.push_back({ op1, arg1, arg2 });
	}

	void parser::emit_store_no_load(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id) {
		opcode op = opcode::NOP;

		switch (p_id->p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ISTORE;
			break;
		case T_DOUBLE:
			op = DSTORE;
			break;
		case T_REFERENCE:
			op = ASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back({ op, p_id.get() });
	}

	void parser::emit_store(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id) {
		opcode op = opcode::NOP;

		switch (p_id->p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ISTORE;
			break;
		case T_DOUBLE:
			op = DSTORE;
			break;
		case T_REFERENCE:
			op = ASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back({ op, p_id.get() });
	}

	void parser::emit_load(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id) {
		opcode op = opcode::NOP;

		switch (p_id->p_type->typecode)
		{
		case T_BOOLEAN:

		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ILOAD;
			break;
		case T_DOUBLE:
			op = DLOAD;
			break;
		case T_REFERENCE:
			op = ALOAD;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back({ op, p_id.get() });
	}

	void parser::emit_ax_load(symbol_table_node_ptr &p_function_id,
		symbol_table_node_ptr &p_id,
		type_ptr &p_type) {
		opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BYTE:
			op = BALOAD;
			break;
		case T_CHAR:
			op = CALOAD;
			break;
		case T_BOOLEAN:
		case T_INT:
			op = IALOAD;
			break;
		case T_DOUBLE:
			op = DALOAD;
			break;
		case T_REFERENCE:
			op = AALOAD;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back({ op, p_id.get(), p_type.get() });
	}

	void parser::emit_ax_store(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id) {
		opcode op = opcode::NOP;

		switch (p_id->p_type->base_type()->typecode)
		{
		case T_BYTE:
			op = BASTORE;
			break;
		case T_CHAR:
			op = CASTORE;
			break;
		case T_BOOLEAN:
		case T_INT:
			op = IASTORE;
			break;
		case T_DOUBLE:
			op = DASTORE;
			break;
		case T_REFERENCE:
			op = AASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back({ op, p_id.get() });
	}

	void parser::emit_inc(symbol_table_node_ptr &p_function_id, type_ptr &p_type, value v_) {

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			this->emit(p_function_id, IINC, { v_.i_ });
			break;
		case T_DOUBLE:
			this->emit(p_function_id, DCONST, { v_.d_ });
			this->emit(p_function_id, DADD);
			break;
		default:
			break;
		}
	}
}