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

#include <iostream>
#include <cstdio>
#include "cxvm.h"
#include "symtab.h"
#include "error.h"

namespace cx{
	namespace vm_settings {
		// VM settings
		// Turn on to view Cx debugging
		bool dev_debug_flag = false;
		// Verbose garbage collection
		bool verbose_gc = false;
	}

	const wchar_t *opcode_string[] = {
		L"aaload"            ,
		L"aastore"           ,
		L"aconst_null"       ,
		L"aload"             ,
		L"anewarray"         ,
		L"arraylength"       ,
		L"astore"            ,
		L"athrow"            ,
		L"b2i"               ,
		L"baload"            ,
		L"bastore"           ,
		L"beq"               ,
		L"bipush"            ,
		L"c2i"               ,
		L"call"              ,
		L"caload"            ,
		L"castore"           ,
		L"checkcast"         ,
		L"d2f"               ,
		L"d2i"               ,
		L"d2l"               ,
		L"dadd"              ,
		L"daload"            ,
		L"dastore"           ,
		L"dcmp"              ,
		L"dconst"            ,
		L"ddiv"              ,
		L"del"				 ,
		L"deq"               ,
		L"dgt"               ,
		L"dgt_eq"            ,
		L"dinc"              ,
		L"dload"             ,
		L"dlt"               ,
		L"dlt_eq"            ,
		L"dmul"              ,
		L"dneg"              ,
		L"dnot_eq"           ,
		L"dpos"              ,
		L"drem"              ,
		L"dstore"            ,
		L"dsub"              ,
		L"dup"               ,
		L"dup2"              ,
		L"dup2_x1"           ,
		L"dup2_x2"           ,
		L"dup_x1"            ,
		L"dup_x2"            ,
		L"getfield"          ,
		L"getstatic"         ,
		L"goto"              ,
		L"i2b"               ,
		L"i2c"               ,
		L"i2d"               ,
		L"iadd"              ,
		L"iaload"            ,
		L"iand"              ,
		L"iastore"           ,
		L"icmp"              ,
		L"iconst"            ,
		L"idiv"              ,
		L"ieq"               ,
		L"if_false"          ,
		L"ifne"              ,
		L"iflt"              ,
		L"ifge"              ,
		L"ifgt"              ,
		L"ifle"              ,
		L"if_acmpeq"         ,
		L"if_acmpne"         ,
		L"if_icmpeq"         ,
		L"if_icmpne"         ,
		L"if_icmplt"         ,
		L"if_icmpge"         ,
		L"if_icmpgt"         ,
		L"if_icmple"         ,
		L"ifnonnull"         ,
		L"ifnull"            ,
		L"igt"               ,
		L"igt_eq"            ,
		L"iinc"              ,
		L"iload"             ,
		L"ilt"               ,
		L"ilt_eq"            ,
		L"imul"              ,
		L"ineg"              ,
		L"inot"              ,
		L"inot_eq"           ,
		L"instanceof"        ,
		L"invokedynamic"     ,
		L"invokefunct"       ,
		L"invokeinterface"  ,
		L"invokespecial"    ,
		L"invokestatic"     ,
		L"invokevirtual"    ,
		L"ior"              ,
		L"ipos"             ,
		L"irem"             ,
		L"ishl"             ,
		L"ishr"             ,
		L"istore"           ,
		L"isub"             ,
		L"ixor"             ,
		L"jsr"              ,
		L"jsr_w"            ,
		L"ldc"              ,
		L"ldc2_w"           ,
		L"ldc_w"            ,
		L"lookupswitch"     ,
		L"logic_or"         ,
		L"logic_and"        ,
		L"logic_not"        ,
		L"monitorenter"     ,
		L"monitorexit"      ,
		L"multianewarray"   ,
		L"new"              ,
		L"newarray"         ,
		L"nop"              ,
		L"pload"            ,
		L"postop"           ,
		L"pop"              ,
		L"pop2"             ,
		L"preop"            ,
		L"putfield"         ,
		L"putstatic"        ,
		L"ret"              ,
		L"swap"             ,
		L"tableswitch"      ,
		L"zeq"              ,
		L"break_marker"
	};

	// Type sizes
	const size_t type_size[] = {
		sizeof(cx_bool),
		sizeof(cx_char),
		sizeof(cx_byte),
		sizeof(cx_int),
		sizeof(cx_real)
	};

	// Stack Ops
#define _POPS (--vpu.stack_ptr)
#define _PUSHS (vpu.stack_ptr++)

	// Value object
#define _VALUE ((symbol_table_node *) this->vpu.inst_ptr->arg0.a_)->runstack_item 

	// Symbol node
#define _NODE ((symbol_table_node *) this->vpu.inst_ptr->arg0.a_)

	// Type
#define _TYPE ((cx_type *)this->vpu.inst_ptr->arg0.a_)

	// Top of stack
#define _TOS vpu.stack_ptr[-1]
	// Memory address to uintptr_t
#define _ADDRTOINT(addr) (uintptr_t)*&addr	
	// Simple bounds checks
#define _BOUNDS_CHECK(index){ \
	if ((index > _NODE->p_type->array.max_index) || \
	(index < 0)) {\
		std::string msg = "index out of bounds: ";\
		msg += std::string(_NODE->node_name.begin(), _NODE->node_name.end()) + "[" + std::to_string(index) + "]";\
		throw std::exception(msg.c_str());\
	}\
}
	// Load Array or reference to stack
#define _ALOAD(t_, type) {      \
		cx_int index = _POPS->i_; \
		void *mem = _POPS->a_; \
		_BOUNDS_CHECK(index) \
		type v_ = *((type *)((char *)mem + (index * sizeof(type))));\
		_PUSHS->t_ = v_;\
}
	// Store to memory
#define _ASTORE(t_, type) {     \
	type v_ = _POPS->t_;\
	cx_int index = _POPS->i_;\
	void *mem = _VALUE->a_;       \
	_BOUNDS_CHECK(index); \
	*((type *)((char *)mem + (index * sizeof(type)))) = v_;\
}
	// Binary Operators
#define _BIN_OP(t_, type, op)  { \
	type b = _POPS->t_; \
	type a = _POPS->t_; \
	_PUSHS->t_ = (a op b); \
}
	// Unary Operators
#define _UNA_OP(t_, type, op)  { \
	type a = _POPS->t_; \
	_PUSHS->t_ = op a; \
}
	// Relational Operators (bool)
#define _REL_OP(t_, type, op)  { \
	type b = _POPS->t_; \
	type a = _POPS->t_; \
	_PUSHS->i_ = (a op b); \
}

	// Pointer to the runtime stack
	cxvm::cxvm() { this->vpu.stack_ptr = this->stack; }
	cxvm::~cxvm(void){}
	value *cxvm::push(void) { return _PUSHS; }
	value *cxvm::pop(void) { return _POPS; }

	// Does a look-up in the current heap and returns a reference to it.
	heap::mem_mapping &cxvm::get_managed_reference(uintptr_t address) {
		if (vm_settings::verbose_gc) {
			std::puts("[GC] Reference copied\n");
			// TODO: output more p_type information?
		}

		return heap_.at(address);
	}

	// Copies a new reference into the current heap.
	void cxvm::copy_reference(uintptr_t &reference, heap::mem_mapping &mal_map) {
		this->heap_.insert(std::make_pair(reference, mal_map));
	}

	// Set basic function elements
	void cxvm::enter_function(symbol_table_node *p_function_id){
		this->p_my_function_id = p_function_id;

		this->vpu.code_ptr = &this->p_my_function_id->defined.routine.program_code;
		this->vpu.inst_ptr = this->vpu.code_ptr->begin();

		// load variables
		if (!p_my_function_id->defined.routine.p_variable_ids.empty()){
			for (auto &local : p_my_function_id->defined.routine.p_variable_ids){
				local->runstack_item = _PUSHS;
			}
		}
	}

	void cxvm::nano_sleep(int nano_secs = 5) {
		std::this_thread::sleep_for(
			std::chrono::nanoseconds(nano_secs)
			);
	}

	void cxvm::go(void) {
		using namespace heap;

		try {
			for (vpu.inst_ptr = vpu.code_ptr->begin();
			vpu.inst_ptr < vpu.code_ptr->end();
				vpu.inst_ptr++) {

				switch (vpu.inst_ptr->op) {
				case opcode::AALOAD: _PUSHS->a_ = _VALUE->a_; continue;
				case opcode::AASTORE: _VALUE->a_ = _POPS->a_; continue;
				case opcode::ACONST_NULL: _PUSHS->a_ = nullptr; continue;
				case opcode::ALOAD: _PUSHS->a_ = _VALUE->a_;  continue;
/*				case opcode::ANEWARRAY: {
					size_t size = (size_t)_POPS->i_ * sizeof(void *);

					void **mem = (void **)malloc(size);
					assert(mem != nullptr);

					mem_mapping *mem_map = &heap_[_ADDRTOINT(mem)]; // point to, only 1 hash calculation

					/* Compile with -D INSTRUCTION_TEST if testing.
					* If undefined, RAM gets released and tests allocating RAM
					* will fail.   */

					// assign mem to smart pointer, release using free()
/*					mem_map->shared_ref = heap::managedmem((uintptr_t *)mem, free);
					mem_map->size = size; // size
					mem_map->typecode = T_REFERENCE; // type
					mem_map->typeform = F_ARRAY;
					_PUSHS->a_ = (void *)mem;
				} continue;
				case opcode::ARRAYLENGTH: {
					void *mem = _POPS->a_;
					assert(mem != nullptr);
					_PUSHS->i_ = heap_[_ADDRTOINT(mem)].count();
				} continue;*/
				case opcode::ASTORE: {
					_VALUE->a_ = _POPS->a_;
					uintptr_t reference = _ADDRTOINT(_VALUE->a_);
					// Do a look up on the heap and increment reference count.
					symbol_table_node *p_node = _NODE;
					p_node->p_type = this->heap_.at(reference).p_type;
				}continue;
				case opcode::ATHROW: { // Throws a string message
					char *message = (char *)_POPS->a_;
					assert(message != nullptr);

					throw std::string(message);
				} continue;
				case opcode::B2I:		_PUSHS->i_ = static_cast<cx_int> (_POPS->b_); continue;
				case opcode::BALOAD:	_ALOAD(b_, cx_byte); continue;
				case opcode::BASTORE:	_ASTORE(b_, cx_byte); continue;
				case opcode::BEQ:		_REL_OP(b_, cx_byte, == ); continue;
				case opcode::C2I:		_PUSHS->i_ = static_cast<cx_int> (_POPS->c_); continue;
				case opcode::CALL: {
					symbol_table_node *p_function_id = (symbol_table_node *)vpu.inst_ptr->arg0.a_;

					std::shared_ptr<cxvm> cx = std::make_shared<cxvm>();
					p_function_id->runstack_item = cx->push();
					p_function_id->runstack_item->a_ = nullptr;

					// Load parameters from the stack
					std::vector<std::shared_ptr<symbol_table_node>>::reverse_iterator parameter = p_function_id->defined.routine.p_parameter_ids.rbegin();
					for (; parameter != p_function_id->defined.routine.p_parameter_ids.rend(); ++parameter) {
						value *p_param = _POPS;
						parameter->get()->runstack_item = cx->push();

						switch (parameter->get()->p_type->typecode) {
						case type_code::T_BOOLEAN:
							parameter->get()->runstack_item->z_ = p_param->z_;
							break;
						case type_code::T_BYTE:
							parameter->get()->runstack_item->b_ = p_param->b_;
							break;
						case type_code::T_CHAR:
							parameter->get()->runstack_item->c_ = p_param->c_;
							break;
						case type_code::T_DOUBLE:
							parameter->get()->runstack_item->d_ = p_param->d_;
							break;
						case type_code::T_INT:
							parameter->get()->runstack_item->i_ = p_param->i_;
							break;
							// Reference needs to be copied into the callee heap
						case type_code::T_REFERENCE: {
							parameter->get()->runstack_item->a_ = p_param->a_;
							uintptr_t reference = _ADDRTOINT(p_param->a_);
							heap::mem_mapping *mem_map = &this->heap_.at(reference);
							cx->copy_reference(reference, *mem_map);
							parameter->get()->p_type = mem_map->p_type;
						}break;
						}
					}

					// Enter function info
					cx->enter_function(p_function_id);
					// Run function
					cx->go();

					// Push functions return value
					switch (p_function_id->p_type->typecode) {
					case type_code::T_BOOLEAN:
						_PUSHS->z_ = p_function_id->runstack_item->z_;

						if (vm_settings::dev_debug_flag) {
							std::wcout << p_function_id->node_name << L" returned " << p_function_id->runstack_item->z_ << std::endl;
						}
						break;
					case type_code::T_BYTE:
						_PUSHS->b_ = p_function_id->runstack_item->b_;

						if (vm_settings::dev_debug_flag) {
							std::wcout << p_function_id->node_name << L" returned " << p_function_id->runstack_item->b_ << std::endl;
						}
						break;
					case type_code::T_CHAR:
						_PUSHS->c_ = p_function_id->runstack_item->c_;

						if (vm_settings::dev_debug_flag) {
							std::wcout << p_function_id->node_name << L" returned " << p_function_id->runstack_item->c_ << std::endl;
						}
						break;
					case type_code::T_DOUBLE:
						_PUSHS->d_ = p_function_id->runstack_item->d_;

						if (vm_settings::dev_debug_flag) {
							std::wcout << p_function_id->node_name << L" returned " << p_function_id->runstack_item->d_ << std::endl;
						}
						break;
					case type_code::T_INT:
						_PUSHS->i_ = p_function_id->runstack_item->i_;

						if (vm_settings::dev_debug_flag) {
							std::wcout << p_function_id->node_name << L" returned " << p_function_id->runstack_item->i_ << std::endl;
						}
						break;
						// Need to copy returned reference into caller heap
					case type_code::T_REFERENCE: {
						uintptr_t reference = _ADDRTOINT(p_function_id->runstack_item->a_);
						this->heap_[reference] = std::move(cx->get_managed_reference(reference));
						p_function_id->p_type = this->heap_.at(reference).p_type;
						_PUSHS->a_ = p_function_id->runstack_item->a_;
					}break;
					case type_code::T_VOID: break;
					}
				} continue;
				case opcode::CALOAD: _ALOAD(c_, cx_char); continue;
				case opcode::CASTORE: _ASTORE(c_, cx_char); continue;
				case opcode::CHECKCAST: continue;

					/** Duplicate the top operand stack value
					 * Duplicate the top value on the operand stack and push
					 * the duplicated value onto the operand stack. */
/*				case opcode::DUP: {
					value *val = (value *)(vpu.stack_ptr - 1);
					assert(val != nullptr);

					// Allow the compiler to build copy CTOR
					value *new_value_copy = new value(*val);

					assert(new_value_copy != nullptr);
					assert(new_value_copy->a_ == val->a_);

					heap::mem_mapping *mem_map = &heap_[_ADDRTOINT(new_value_copy)]; // point to, only 1 hash calculation

					/* Compile with -D INSTRUCTION_TEST if testing.
					* If undefined, RAM gets released and tests that allocate RAM
					* will fail.   */

					// Assign mem to smart pointer, release using delete
/*					mem_map->shared_ref = std::move(heap::managedmem((uintptr_t *)new_value_copy));
					mem_map->size = sizeof(value); // size
					mem_map->typecode = T_REFERENCE; // type
					mem_map->typeform = F_SCALAR;

					// Push new copy
					_PUSHS->a_ = (void *)new_value_copy;

				} continue;*/
				case opcode::DUP2:		continue;
				case opcode::DUP2_X1:	continue;
				case opcode::DUP2_X2:	continue;
				case opcode::DUP_X1:	continue;
				case opcode::DUP_X2:	continue;
				case opcode::D2I:		_PUSHS->i_ = static_cast<cx_int> (_POPS->d_); continue;
				case opcode::DADD:		_BIN_OP(d_, cx_real, +); continue;
				case opcode::DALOAD:	_ALOAD(d_, cx_real); continue;
				case opcode::DASTORE:	_ASTORE(d_, cx_real); continue;
				case opcode::DCONST:	_PUSHS->d_ = vpu.inst_ptr->arg0.d_; continue;
				case opcode::DDIV:		_BIN_OP(d_, cx_real, / ); continue;
				case opcode::DEL: {
					uintptr_t reference = _ADDRTOINT(_VALUE->a_);
					if (this->heap_.erase(reference) == 0) {
						std::string node_name = std::string(_NODE->node_name.begin(), _NODE->node_name.end());
						std::string msg = "Double delete on reference or [ " + node_name + " ] not allocated on heap.";
						throw std::exception(msg.c_str());
					}
				}continue;
				case opcode::DEQ:		_REL_OP(d_, cx_real, == ); continue;
				case opcode::DGT:		_REL_OP(d_, cx_real, > ); continue;
				case opcode::DGT_EQ:	_REL_OP(d_, cx_real, >= ); continue;
				case opcode::DINC:		_VALUE->d_ += vpu.inst_ptr->arg1.d_; continue;
				case opcode::DLOAD:		_PUSHS->d_ = _VALUE->d_; continue;
				case opcode::DLT:		_REL_OP(d_, cx_real, < ); continue;
				case opcode::DLT_EQ:	_REL_OP(d_, cx_real, <= ); continue;
				case opcode::DMUL:		_BIN_OP(d_, cx_real, *); continue;
				case opcode::DNEG:		_PUSHS->d_ = -abs(_POPS->d_); continue;
				case opcode::DNOT_EQ:	_REL_OP(d_, cx_real, != ); continue;
				case opcode::DPOS:		_PUSHS->d_ = abs(_POPS->d_); continue;
				case opcode::DREM: {
					cx_real b = _POPS->d_;
					cx_real a = _POPS->d_;
					_PUSHS->d_ = fmod(a, b);
				}continue;
				case opcode::DSTORE:	_VALUE->d_ = _POPS->d_; continue;
				case opcode::DSUB:		_BIN_OP(d_, cx_real, -); continue;
				case opcode::GETFIELD: continue;
				case opcode::GETSTATIC: continue;
				case opcode::GOTO: {
					cx_int location = vpu.inst_ptr->arg0.i_;
					if (location <= 0) {
						vpu.inst_ptr = vpu.code_ptr->begin();
					}
					else {
						vpu.inst_ptr = vpu.code_ptr->begin() + (int)(location - 1);
					}
				} continue;
				case opcode::I2B:		_PUSHS->b_ = static_cast<cx_byte> (_POPS->i_); continue;
				case opcode::I2C:		_PUSHS->c_ = static_cast<cx_char> (_POPS->i_); continue;
				case opcode::I2D:		_PUSHS->d_ = static_cast<cx_real> (_POPS->i_); continue;
				case opcode::IADD:		_BIN_OP(i_, cx_int, +); continue;
				case opcode::IALOAD:	_ALOAD(i_, cx_int); continue;
				case opcode::ILT:		_REL_OP(i_, cx_int, < ); continue;
					// Bitwise AND
				case opcode::IAND:		_BIN_OP(i_, cx_int, &); continue;
				case opcode::IASTORE:	_ASTORE(i_, cx_int); continue;
				case opcode::ICMP:
					continue;
				case opcode::ICONST:	_PUSHS->i_ = vpu.inst_ptr->arg0.i_; continue;
				case opcode::IDIV:		_BIN_OP(i_, cx_int, / ); continue;
				case opcode::IEQ:		_REL_OP(i_, cx_int, == ); continue;
				case opcode::IF_FALSE:
				{
					if (!_POPS->z_) {
						cx_int location = vpu.inst_ptr->arg0.i_;
						vpu.inst_ptr = vpu.code_ptr->begin() + (int)(location - 1);
					}
				}continue;
				/*case opcode::IFNE: _IF(!= ); continue;
				case opcode::IFLT: _IF(< ); continue;
				case opcode::IFGE: _IF(>= ); continue;
				case opcode::IFGT: _IF(> ); continue;
				case opcode::IFLE: _IF(<= ); continue;*/

/*				case opcode::IF_ACMPEQ: {
					void *value2 = _POPS->a_;
					void *value1 = _POPS->a_;

					if (!memcmp(value1, value2, heap_[_ADDRTOINT(value1)].size)) _JMP(i_);
				} continue;

				case opcode::IF_ACMPNE: {
					void *value2 = _POPS->a_;
					void *value1 = _POPS->a_;

					if (memcmp(value1, value2, heap_[_ADDRTOINT(value1)].size)) _JMP(i_);
				} continue;
*/
				/*case opcode::IF_ICMPEQ: _IFICMP(== ); continue;
				case opcode::IF_ICMPNE: _IFICMP(!= ); continue;
				case opcode::IF_ICMPLT: _IFICMP(< ); continue;
				case opcode::IF_ICMPGE: _IFICMP(>= ); continue;
				case opcode::IF_ICMPGT: _IFICMP(> ); continue;
				case opcode::IF_ICMPLE: _IFICMP(<= ); continue;
				case opcode::IFNONNULL: if (_POPS->a_ != nullptr) _JMP(i_); continue;
				case opcode::IFNULL: if (_POPS->a_ == nullptr) _JMP(i_); continue;*/
				case opcode::IGT:		_REL_OP(i_, cx_int, > ); continue;
				case opcode::IGT_EQ:	_REL_OP(i_, cx_int, >= ); continue;
				case opcode::IINC:		_VALUE->i_ += vpu.inst_ptr->arg1.i_; continue;
				case opcode::ILOAD:		_PUSHS->i_ = _VALUE->i_; continue;
				case opcode::ILT_EQ:	_REL_OP(i_, cx_int, <= ); continue;
				case opcode::IMUL:		_BIN_OP(i_, cx_int, *); continue;
				case opcode::INEG:		_PUSHS->i_ = -abs(_POPS->i_); continue;
					// Unary complement (bit inversion)
				case opcode::INOT: 		_UNA_OP(i_, cx_int, ~); continue;
				case opcode::INOT_EQ:	_REL_OP(i_, cx_int, != ); continue;
				case opcode::INSTANCEOF: continue;
				case opcode::INVOKEDYNAMIC: continue;
				case opcode::INVOKEFUNCT: continue;
				case opcode::INVOKEINTERFACE: continue;
				case opcode::INVOKESPECIAL: continue;
				case opcode::INVOKESTATIC: continue;
				case opcode::INVOKEVIRTUAL: continue;
					// Bitwise inclusive OR
				case opcode::IOR:		_BIN_OP(i_, cx_int, | ); continue;
				case opcode::IPOS: 		_PUSHS->i_ = abs(_POPS->i_); continue;
				case opcode::IREM: 		_BIN_OP(i_, cx_int, %); continue;
				case opcode::ISHL: 		_BIN_OP(i_, cx_int, << ); continue;
				case opcode::ISHR: 		_BIN_OP(i_, cx_int, >> ); continue;
				case opcode::ISTORE:	_VALUE->i_ = _POPS->i_; continue;
				case opcode::ISUB:		_BIN_OP(i_, cx_int, -); continue;
					// Bitwise exclusive OR
				case opcode::IXOR: 		_BIN_OP(i_, cx_int, ^); continue;
				case opcode::JSR:
				case opcode::JSR_W: continue;
				case opcode::LDC:
				case opcode::LDC2_W:
				case opcode::LDC_W: continue;
				case opcode::LOOKUPSWITCH: continue;
				case opcode::LOGIC_OR:	_BIN_OP(z_, cx_bool, || ); continue;
				case opcode::LOGIC_AND:	_BIN_OP(z_, cx_bool, &&); continue;
				case opcode::LOGIC_NOT: _PUSHS->z_ = !_POPS->i_; continue;
				case opcode::MONITORENTER:
				case opcode::MONITOREXIT: continue;
				case opcode::MULTIANEWARRAY: continue;
				case opcode::NEW: continue;

					/** newarray: allocate new array
					 * @param: vpu.stack_ptr[-1].l_ - number of elements
					 * @param: vpu.inst_ptr->arg0.b_ - type code
					 * @return: new array allocation managed by GC */
				case opcode::NEWARRAY: {
					size_t element_count = static_cast<size_t>(_POPS->i_);
					cx_type *p_type = (cx_type *)vpu.inst_ptr->arg0.a_;
					size_t size = p_type->size;
					void *mem = malloc(size);
					
					if (mem == nullptr) {
						std::string msg = "[ malloc ] ";
						msg += std::strerror(errno);
						throw std::exception(msg.c_str());
					}

					std::memset(mem, 0, size);
					heap::mem_mapping *mem_map = &heap_[_ADDRTOINT(mem)]; // Point to, only 1 hash calculation
					// Assign mem to smart pointer, release using free()
					mem_map->shared_ref = heap::managedmem((uintptr_t *)mem, free);		
					mem_map->p_type = std::make_shared<cx_type>(F_ARRAY, T_REFERENCE);
					mem_map->p_type->array.element_count = element_count;
					mem_map->p_type->array.min_index = 0;
					mem_map->p_type->array.max_index = static_cast<int>(element_count) - 1;
					mem_map->p_type->size = size;
					mem_map->p_type->array.p_element_type = p_type->array.p_element_type;
					mem_map->p_type->array.p_index_type = p_type->array.p_index_type;
				
					_PUSHS->a_ = mem;
				} continue;
				case opcode::NOP: continue;
				case opcode::PLOAD: _PUSHS->a_ = _VALUE->a_; continue;
				case opcode::POP: _POPS; continue;
				case opcode::POP2: _POPS; _POPS; continue;
				case opcode::PUTFIELD: continue;
				case opcode::PUTSTATIC: continue;
				case opcode::RETURN: return; break;
				case opcode::SWAP: continue;
				case opcode::TABLESWITCH: continue;
				case opcode::ZEQ: _REL_OP(z_, cx_bool, == ); continue;
				} //switch
			} // for
		}

		catch (std::exception ex) {
			std::wcerr << L"[ function  ]--> " << this->p_my_function_id->node_name << std::endl;
			std::wcerr << L"[ paramters ]-->";
			if (this->p_my_function_id->defined.routine.p_parameter_ids.empty()) {
				std::cerr << " [ void ]";
			}
			else {
				//for (auto &param : this->p_my_function_id->defined.routine.p_parameter_ids) {
				// TODO: std::wcerr << L" [ " << param->p_type->p_type_id->node_name << L" ] " << param->node_name;
				//}
			}

			std::wcerr << std::endl;
			throw ex;
		}

		return;
	}
}
