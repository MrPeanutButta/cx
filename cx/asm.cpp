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

	const std::pair<std::wstring, cx::opcode> opcode_map_data[] = {
		std::make_pair(L"aaload",          cx::opcode::AALOAD),
		std::make_pair(L"aastore",         cx::opcode::AASTORE),
		std::make_pair(L"aconst_null",     cx::opcode::ACONST_NULL),
		std::make_pair(L"aload",           cx::opcode::ALOAD),
		std::make_pair(L"anewarray",       cx::opcode::ANEWARRAY),
		std::make_pair(L"arraylength",     cx::opcode::ARRAYLENGTH),
		std::make_pair(L"astore",          cx::opcode::ASTORE),
		std::make_pair(L"vm_throw",        cx::opcode::VM_THROW),
		std::make_pair(L"baload",          cx::opcode::BALOAD),
		std::make_pair(L"bastore",         cx::opcode::BASTORE),
		std::make_pair(L"beq",             cx::opcode::BEQ),
		std::make_pair(L"bipush",          cx::opcode::BIPUSH),
		std::make_pair(L"c2i",             cx::opcode::C2I),
		std::make_pair(L"call",            cx::opcode::CALL),
		std::make_pair(L"caload",          cx::opcode::CALOAD),
		std::make_pair(L"castore",         cx::opcode::CASTORE),
		std::make_pair(L"checkcast",       cx::opcode::CHECKCAST),
		std::make_pair(L"d2f",             cx::opcode::D2F),
		std::make_pair(L"d2i",             cx::opcode::D2I),
		std::make_pair(L"d2l",             cx::opcode::D2L),
		std::make_pair(L"dadd",            cx::opcode::DADD),
		std::make_pair(L"daload",          cx::opcode::DALOAD),
		std::make_pair(L"dastore",         cx::opcode::DASTORE),
		std::make_pair(L"dcmp",            cx::opcode::DCMP),
		std::make_pair(L"dconst",          cx::opcode::DCONST),
		std::make_pair(L"ddiv",            cx::opcode::DDIV),
		std::make_pair(L"deq_eq",          cx::opcode::DEQ),
		std::make_pair(L"dgt",             cx::opcode::DGT),
		std::make_pair(L"dgt_eq",          cx::opcode::DGT_EQ),
		std::make_pair(L"dinc",            cx::opcode::DINC),
		std::make_pair(L"dload",           cx::opcode::DLOAD),
		std::make_pair(L"dlt",             cx::opcode::DLT),
		std::make_pair(L"dlt_eq",          cx::opcode::DLT_EQ),
		std::make_pair(L"dmul",            cx::opcode::DMUL),
		std::make_pair(L"dneg",            cx::opcode::DNEG),
		std::make_pair(L"dnot_eq",         cx::opcode::DNOT_EQ),
		std::make_pair(L"drem",            cx::opcode::DREM),
		std::make_pair(L"dstore",          cx::opcode::DSTORE),
		std::make_pair(L"dsub",            cx::opcode::DSUB),
		std::make_pair(L"dup",             cx::opcode::DUP),
		std::make_pair(L"dup2",            cx::opcode::DUP2),
		std::make_pair(L"dup2_x1",         cx::opcode::DUP2_X1),
		std::make_pair(L"dup2_x2",         cx::opcode::DUP2_X2),
		std::make_pair(L"dup_x1",          cx::opcode::DUP_X1),
		std::make_pair(L"dup_x2",          cx::opcode::DUP_X2),
		std::make_pair(L"getfield",        cx::opcode::GETFIELD),
		std::make_pair(L"getstatic",       cx::opcode::GETSTATIC),
		std::make_pair(L"goto",            cx::opcode::GOTO),
		std::make_pair(L"i2b",             cx::opcode::I2B),
		std::make_pair(L"i2c",             cx::opcode::I2C),
		std::make_pair(L"i2d",             cx::opcode::I2D),
		std::make_pair(L"iadd",            cx::opcode::IADD),
		std::make_pair(L"iaload",          cx::opcode::IALOAD),
		std::make_pair(L"iand",            cx::opcode::IAND),
		std::make_pair(L"iastore",         cx::opcode::IASTORE),
		std::make_pair(L"icmp",            cx::opcode::ICMP),
		std::make_pair(L"iconst",          cx::opcode::ICONST),
		std::make_pair(L"idiv",            cx::opcode::IDIV),
		std::make_pair(L"ieq_eq",          cx::opcode::IEQ),
		std::make_pair(L"if_false",        cx::opcode::IF_FALSE),
		std::make_pair(L"ifne",            cx::opcode::IFNE),
		std::make_pair(L"iflt",            cx::opcode::IFLT),
		std::make_pair(L"ifge",            cx::opcode::IFGE),
		std::make_pair(L"ifgt",            cx::opcode::IFGT),
		std::make_pair(L"ifle",            cx::opcode::IFLE),
		std::make_pair(L"if_acmpeq",       cx::opcode::IF_ACMPEQ),
		std::make_pair(L"if_acmpne",       cx::opcode::IF_ACMPNE),
		std::make_pair(L"if_icmpeq",       cx::opcode::IF_ICMPEQ),
		std::make_pair(L"if_icmpne",       cx::opcode::IF_ICMPNE),
		std::make_pair(L"if_icmplt",       cx::opcode::IF_ICMPLT),
		std::make_pair(L"if_icmpge",       cx::opcode::IF_ICMPGE),
		std::make_pair(L"if_icmpgt",       cx::opcode::IF_ICMPGT),
		std::make_pair(L"if_icmple",       cx::opcode::IF_ICMPLE),
		std::make_pair(L"ifnonnull",       cx::opcode::IFNONNULL),
		std::make_pair(L"ifnull",          cx::opcode::IFNULL),
		std::make_pair(L"igt",             cx::opcode::IGT),
		std::make_pair(L"igt_eq",          cx::opcode::IGT_EQ),
		std::make_pair(L"iinc",            cx::opcode::IINC),
		std::make_pair(L"iload",           cx::opcode::ILOAD),
		std::make_pair(L"ilt",             cx::opcode::ILT),
		std::make_pair(L"ilt_eq",          cx::opcode::ILT_EQ),
		std::make_pair(L"imul",            cx::opcode::IMUL),
		std::make_pair(L"ineg",            cx::opcode::INEG),
		std::make_pair(L"inot",            cx::opcode::INOT),
		std::make_pair(L"inot_eq",         cx::opcode::INOT_EQ),
		std::make_pair(L"instanceof",      cx::opcode::INSTANCEOF),
		std::make_pair(L"invokedynamic",   cx::opcode::INVOKEDYNAMIC),
		std::make_pair(L"invokefunct",     cx::opcode::INVOKEFUNCT),
		std::make_pair(L"invokeinterface", cx::opcode::INVOKEINTERFACE),
		std::make_pair(L"invokespecial",   cx::opcode::INVOKESPECIAL),
		std::make_pair(L"invokestatic",    cx::opcode::INVOKESTATIC),
		std::make_pair(L"invokevirtual",   cx::opcode::INVOKEVIRTUAL),
		std::make_pair(L"ior",             cx::opcode::IOR),
		std::make_pair(L"irem",            cx::opcode::IREM),
		std::make_pair(L"ishl",            cx::opcode::ISHL),
		std::make_pair(L"ishr",            cx::opcode::ISHR),
		std::make_pair(L"istore",          cx::opcode::ISTORE),
		std::make_pair(L"isub",            cx::opcode::ISUB),
		std::make_pair(L"ixor",            cx::opcode::IXOR),
		std::make_pair(L"jsr",             cx::opcode::JSR),
		std::make_pair(L"jsr_w",           cx::opcode::JSR_W),
		std::make_pair(L"ldc",             cx::opcode::LDC),
		std::make_pair(L"ldc2_w",          cx::opcode::LDC2_W),
		std::make_pair(L"ldc_w",           cx::opcode::LDC_W),
		std::make_pair(L"lookupswitch",    cx::opcode::LOOKUPSWITCH),
		std::make_pair(L"logic_or",        cx::opcode::LOGIC_OR),
		std::make_pair(L"logic_and",       cx::opcode::LOGIC_AND),
		std::make_pair(L"logic_not",       cx::opcode::LOGIC_NOT),
		std::make_pair(L"monitorenter",    cx::opcode::MONITORENTER),
		std::make_pair(L"monitorexit",     cx::opcode::MONITOREXIT),
		std::make_pair(L"multianewarray",  cx::opcode::MULTIANEWARRAY),
		std::make_pair(L"new",             cx::opcode::NEW),
		std::make_pair(L"newarray",        cx::opcode::NEWARRAY),
		std::make_pair(L"nop",             cx::opcode::NOP),
		std::make_pair(L"pload",           cx::opcode::PLOAD),
		std::make_pair(L"postop",          cx::opcode::POSTOP),
		std::make_pair(L"pop",             cx::opcode::POP),
		std::make_pair(L"pop2",            cx::opcode::POP2),
		std::make_pair(L"preop",           cx::opcode::PREOP),
		std::make_pair(L"putfield",        cx::opcode::PUTFIELD),
		std::make_pair(L"putstatic",       cx::opcode::PUTSTATIC),
		std::make_pair(L"return",          cx::opcode::RETURN),
		std::make_pair(L"ret",			   cx::opcode::RETURN),
		std::make_pair(L"swap",            cx::opcode::SWAP),
		std::make_pair(L"tableswitch",     cx::opcode::TABLESWITCH),
		std::make_pair(L"zeq",			   cx::opcode::ZEQ)
	};

	void parser::parse_ASM(symbol_table_node_ptr &p_function_id) {
		symtab_stack.enter_scope();

		get_token();

		if (p_function_id != nullptr) {}

		typedef std::map<std::wstring, cx::opcode> opcode_map;
		opcode_map cxvm_opcodes(opcode_map_data, opcode_map_data + sizeof opcode_map_data / sizeof opcode_map_data[0]);

		bool is_block = (token == TC_LEFT_BRACKET);
		if (is_block) get_token();

		do {

			opcode op = opcode::NOP;

			if (token != TC_IDENTIFIER) {
				cx_error(ERR_INVALID_OPCODE);
			}
			else {
				op = cxvm_opcodes[this->p_token->string];
			}

			switch (op) {
			case opcode::AALOAD: get_token(); break;
			case opcode::AASTORE: get_token(); break;
			case opcode::ACONST_NULL: get_token(); break;
			case opcode::ALOAD: get_token(); break;
			case opcode::ANEWARRAY: get_token(); break;
			case opcode::ARRAYLENGTH: get_token(); break;
			case opcode::ASTORE: get_token(); break;
			case opcode::VM_THROW: get_token(); break;
			case opcode::BALOAD: get_token(); break;
			case opcode::BASTORE: get_token(); break;
			case opcode::BEQ: get_token(); break;
			case opcode::BIPUSH: get_token(); break;
			case opcode::C2I: get_token(); break;
			case opcode::CALL: get_token(); break;
			case opcode::CALOAD: get_token(); break;
			case opcode::CASTORE: get_token(); break;
			case opcode::CHECKCAST: get_token(); break;
			case opcode::D2F: get_token(); break;
			case opcode::D2I: get_token(); break;
			case opcode::D2L: get_token(); break;
			case opcode::DADD: get_token(); break;
			case opcode::DALOAD: get_token(); break;
			case opcode::DASTORE: get_token(); break;
			case opcode::DCMP: get_token(); break;
			case opcode::DCONST: get_token(); break;
			case opcode::DDIV: get_token(); break;
			case opcode::DEQ: get_token(); break;
			case opcode::DGT: get_token(); break;
			case opcode::DGT_EQ: get_token(); break;
			case opcode::DINC: get_token(); break;
			case opcode::DLOAD: get_token(); break;
			case opcode::DLT: get_token(); break;
			case opcode::DLT_EQ: get_token(); break;
			case opcode::DMUL: get_token(); break;
			case opcode::DNEG: get_token(); break;
			case opcode::DNOT_EQ: get_token(); break;
			case opcode::DREM: get_token(); break;
			case opcode::DSTORE: get_token(); break;
			case opcode::DSUB: get_token(); break;
			case opcode::DUP: get_token(); break;
			case opcode::DUP2: get_token(); break;
			case opcode::DUP2_X1: get_token(); break;
			case opcode::DUP2_X2: get_token(); break;
			case opcode::DUP_X1: get_token(); break;
			case opcode::DUP_X2: get_token(); break;
			case opcode::GETFIELD: get_token(); break;
			case opcode::GETSTATIC: get_token(); break;
			case opcode::GOTO: get_token(); break;
			case opcode::I2B: get_token(); break;
			case opcode::I2C: get_token(); break;
			case opcode::I2D: get_token(); break;
			case opcode::IADD:
				this->emit(p_function_id, opcode::IADD);
				break;
			case opcode::IALOAD: get_token(); break;
			case opcode::IAND: get_token(); break;
			case opcode::IASTORE: get_token(); break;
			case opcode::ICMP: get_token(); break;
			case opcode::ICONST: get_token();
				if (token != TC_NUMBER) cx_error(ERR_INVALID_NUMBER);
				this->emit(p_function_id, opcode::ICONST, { p_token->value().i_ });
				break;
			case opcode::IDIV: get_token(); break;
			case opcode::IEQ: get_token(); break;
			case opcode::IF_FALSE: get_token(); break;
			case opcode::IFNE: get_token(); break;
			case opcode::IFLT: get_token(); break;
			case opcode::IFGE: get_token(); break;
			case opcode::IFGT: get_token(); break;
			case opcode::IFLE: get_token(); break;
			case opcode::IF_ACMPEQ: get_token(); break;
			case opcode::IF_ACMPNE: get_token(); break;
			case opcode::IF_ICMPEQ: get_token(); break;
			case opcode::IF_ICMPNE: get_token(); break;
			case opcode::IF_ICMPLT: get_token(); break;
			case opcode::IF_ICMPGE: get_token(); break;
			case opcode::IF_ICMPGT: get_token(); break;
			case opcode::IF_ICMPLE: get_token(); break;
			case opcode::IFNONNULL: get_token(); break;
			case opcode::IFNULL: get_token(); break;
			case opcode::IGT: get_token(); break;
			case opcode::IGT_EQ: get_token(); break;
			case opcode::IINC: get_token(); break;
			case opcode::ILOAD: get_token(); {
				if (token != TC_IDENTIFIER) cx_error(ERR_MISSING_IDENTIFIER);
				symbol_table_node_ptr p_node = search_all(p_token->string);
				if (p_node == nullptr) cx_error(ERR_UNDEFINED_IDENTIFIER);
				this->emit(p_function_id, opcode::ILOAD, p_node.get());
			}
								break;
			case opcode::ILT: get_token(); break;
			case opcode::ILT_EQ: get_token(); break;
			case opcode::IMUL: get_token(); break;
			case opcode::INEG: get_token(); break;
			case opcode::INOT: get_token(); break;
			case opcode::INOT_EQ: get_token(); break;
			case opcode::INSTANCEOF: get_token(); break;
			case opcode::INVOKEDYNAMIC: get_token(); break;
			case opcode::INVOKEFUNCT: get_token(); break;
			case opcode::INVOKEINTERFACE: get_token(); break;
			case opcode::INVOKESPECIAL: get_token(); break;
			case opcode::INVOKESTATIC: get_token(); break;
			case opcode::INVOKEVIRTUAL: get_token(); break;
			case opcode::IOR: get_token(); break;
			case opcode::IREM: get_token(); break;
			case opcode::ISHL: get_token(); break;
			case opcode::ISHR: get_token(); break;
			case opcode::ISTORE: get_token(); {
				if (token != TC_IDENTIFIER) cx_error(ERR_MISSING_IDENTIFIER);
				symbol_table_node_ptr p_node = search_all(p_token->string);
				if (p_node == nullptr) cx_error(ERR_UNDEFINED_IDENTIFIER);
				this->emit(p_function_id, opcode::ISTORE, p_node.get());
			}
								 break;
			case opcode::ISUB: get_token(); break;
			case opcode::IXOR: get_token(); break;
			case opcode::JSR: get_token(); break;
			case opcode::JSR_W: get_token(); break;
			case opcode::LDC: get_token(); break;
			case opcode::LDC2_W: get_token(); break;
			case opcode::LDC_W: get_token(); break;
			case opcode::LOOKUPSWITCH: get_token(); break;
			case opcode::LOGIC_OR: get_token(); break;
			case opcode::LOGIC_AND: get_token(); break;
			case opcode::LOGIC_NOT: get_token(); break;
			case opcode::MONITORENTER: get_token(); break;
			case opcode::MONITOREXIT: get_token(); break;
			case opcode::MULTIANEWARRAY: get_token(); break;
			case opcode::NEW: get_token(); break;
			case opcode::NEWARRAY: get_token(); break;
			case opcode::NOP: get_token(); break;
			case opcode::PLOAD: get_token(); break;
			case opcode::POSTOP: get_token(); break;
			case opcode::POP: get_token(); break;
			case opcode::POP2: get_token(); break;
			case opcode::PREOP: get_token(); break;
			case opcode::PUTFIELD: get_token(); break;
			case opcode::PUTSTATIC: get_token(); break;
			case opcode::RETURN:
				this->emit(p_function_id, opcode::RETURN);
				break;
			case opcode::SWAP: get_token(); break;
			case opcode::TABLESWITCH: get_token(); break;
			default:
				cx_error(ERR_INVALID_OPCODE);
				break;
			}

			get_token();
		} while (is_block && token != TC_RIGHT_BRACKET);

		if (is_block) conditional_get_token(TC_RIGHT_BRACKET, ERR_MISSING_RIGHT_BRACKET);
		else conditional_get_token(TC_SEMICOLON, ERR_MISSING_SEMICOLON);

		symtab_stack.exit_scope();
	}
}