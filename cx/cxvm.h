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

#ifndef CXVM_H
#define	CXVM_H

/// Only defined for debugging
#ifdef _DEBUG
#include <iostream>
#include <iomanip>
#endif

#include <cmath>
#include <cassert>
#include <cstring>
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <map>
#include <unordered_map>
#include <chrono>
#include "types.h"
#include "symtab.h"

namespace cx {
	namespace heap {
		// Memory allocation representation class
		typedef std::shared_ptr<uintptr_t> managedmem;

		class mem_mapping {
		private:
		public:
			mem_mapping() : is_rvalue(false) {}
			managedmem shared_ref;	// VM memory allocations
			size_t size;			// sizeof
			type_code typecode;
			type_form typeform;
			bool is_rvalue;			// Evaluates to a temporary value that does not persist beyond the expression that defines it.
			size_t count(void);		// memory size / element size
		};

		typedef void* address;
		typedef std::map<uintptr_t, mem_mapping> malloc_map;

		extern malloc_map heap_;		// HEAP: For storing raw memory allocations
	}

	extern const wchar_t* opcode_string[];

	// Op codes
	enum opcode {
		AALOAD,
		AASTORE,
		ACONST_NULL,
		ALOAD,
		ANEWARRAY,
		ARRAYLENGTH,
		ASTORE,
		ATHROW,
		BALOAD,
		BASTORE,
		BEQ,
		BGE,
		BGT,
		BIPUSH,
		BLE,
		BLT,
		BNE,
		C2I,
		CALL,
		CALOAD,
		CASTORE,
		CHECKCAST,
		D2F,
		D2I,
		D2L,
		DADD,
		DALOAD,
		DASTORE,
		DCMP,
		DCONST,
		DDIV,
		DEQ_EQ,
		DGT,
		DGT_EQ,
		DINC,
		DLOAD,
		DLT,
		DLT_EQ,
		DMUL,
		DNEG,
		DNOT_EQ,
		DPOS,
		DREM,
		DSTORE,
		DSUB,
		DUP,
		DUP2,
		DUP2_X1,
		DUP2_X2,
		DUP_X1,
		DUP_X2,
		GETFIELD,
		GETSTATIC,
		GOTO,
		I2B,
		I2C,
		I2D,
		IADD,
		IALOAD,
		IAND,
		IASTORE,
		ICMP,
		ICONST,
		IDIV,
		IEQ_EQ,
		IF_FALSE,
		IFNE,
		IFLT,
		IFGE,
		IFGT,
		IFLE,
		IF_ACMPEQ,
		IF_ACMPNE,
		IF_ICMPEQ,
		IF_ICMPNE,
		IF_ICMPLT,
		IF_ICMPGE,
		IF_ICMPGT,
		IF_ICMPLE,
		IFNONNULL,
		IFNULL,
		IGT,
		IGT_EQ,
		IINC,
		ILOAD,
		ILT,
		ILT_EQ,
		IMUL,
		INEG,
		INOT,
		INOT_EQ,
		INSTANCEOF,
		INVOKEDYNAMIC,
		INVOKEFUNCT,
		INVOKEINTERFACE,
		INVOKESPECIAL,
		INVOKESTATIC,
		INVOKEVIRTUAL,
		IOR,
		IPOS,
		IREM,
		ISHL,
		ISHR,
		ISTORE,
		ISUB,
		IXOR,
		JSR,
		JSR_W,
		LDC,
		LDC2_W,
		LDC_W,
		LOOKUPSWITCH,
		LOGIC_OR,
		LOGIC_AND,
		LOGIC_NOT,
		MONITORENTER,
		MONITOREXIT,
		MULTIANEWARRAY,
		NEW,
		NEWARRAY,
		NOP,
		PLOAD,
		POSTOP,
		POP,
		POP2,
		PREOP,
		PUTFIELD,
		PUTSTATIC,
		RETURN,
		SWAP,
		TABLESWITCH,
		BREAK_MARKER
	};

	// Instruction
	typedef struct inst {
		opcode op;
		value arg0;
		value arg1;

		inst() = delete;
		inst(opcode op_) : op(op_){}
		inst(opcode op_, value arg0_) : op(op_), arg0(arg0_) {}
		inst(opcode op_, value arg0_, value arg1_) : op(op_), arg0(arg0_), arg1(arg1_) {}
	} inst;

	// Program instructions
	typedef std::vector<inst> program;
	// Instruction pointer
	typedef std::vector<inst>::const_iterator instr_ptr;

	// Virtual CPU
	struct _vcpu {
		value *stack_ptr;	// Pointer to the current position in stack
		instr_ptr inst_ptr; // Instruction pointer
		const program *code_ptr;
	};

	enum {
		_STACK_SIZE = 255
	};

	class cxvm {
	private:
		_vcpu vpu;					// VPU: Virtual Proc Unit
		value stack[_STACK_SIZE];	// STACK:
		heap::malloc_map heap_;			// HEAP: For storing raw memory allocations

		//std::mutex vm_lock;				// VM lock during execution
		const symbol_table_node *p_my_function_id;
		void nano_sleep(int nano_secs);	// Thread sleep while waiting for VM lock

	public:
		
		value *push(void);
		value *pop(void);
		void enter_function(symbol_table_node *p_function_id);
		void go(void);
		cxvm();
		~cxvm(void);

		void lock(void);
		void unlock(void);
	};
}

#endif	/* CXVM_H */

