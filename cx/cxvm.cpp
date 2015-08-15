/*
*
*/

#include <iostream>
#include "cxvm.h"
#include "symtab.h"

namespace cx {

	const wchar_t *opcode_string[] = {
		L"AALOAD",
		L"AASTORE",
		L"ACONST_NULL",
		L"ALOAD",
		L"ANEWARRAY",
		L"ARRAYLENGTH",
		L"ASTORE",
		L"ATHROW",
		L"BALOAD",
		L"BASTORE",
		L"BEQ",
		L"BGE",
		L"BGT",
		L"BIPUSH",
		L"BLE",
		L"BLT",
		L"BNE",
		L"C2I",
		L"CALL",
		L"CALOAD",
		L"CASTORE",
		L"CHECKCAST",
		L"D2F",
		L"D2I",
		L"D2L",
		L"DADD",
		L"DALOAD",
		L"DASTORE",
		L"DCMP",
		L"DCONST",
		L"DDIV",
		L"DEQ_EQ",
		L"DGT",
		L"DGT_EQ",
		L"DLOAD",
		L"DLT",
		L"DLT_EQ",
		L"DMUL",
		L"DNEG",
		L"DNOT_EQ",
		L"DREM",
		L"DRETURN",
		L"DSTORE",
		L"DSUB",
		L"DUP",
		L"DUP2",
		L"DUP2_X1",
		L"DUP2_X2",
		L"DUP_X1",
		L"DUP_X2",
		L"GETFIELD",
		L"GETSTATIC",
		L"GOTO",
		L"HALT",
		L"I2B",
		L"I2C",
		L"I2D",
		L"IADD",
		L"IALOAD",
		L"IAND",
		L"IASTORE",
		L"ICMP",
		L"ICONST",
		L"IDIV",
		L"IEQ_EQ",
		L"IFEQ",
		L"IFNE",
		L"IFLT",
		L"IFGE",
		L"IFGT",
		L"IFLE",
		L"IF_ACMPEQ",
		L"IF_ACMPNE",
		L"IF_ICMPEQ",
		L"IF_ICMPNE",
		L"IF_ICMPLT",
		L"IF_ICMPGE",
		L"IF_ICMPGT",
		L"IF_ICMPLE",
		L"IFNONNULL",
		L"IFNULL",
		L"IGT",
		L"IGT_EQ",
		L"IINC",
		L"ILOAD",
		L"ILT",
		L"ILT_EQ",
		L"IMUL",
		L"INEG",
		L"INOT",
		L"INOT_EQ",
		L"INSTANCEOF",
		L"INVOKEDYNAMIC",
		L"INVOKEFUNCT",
		L"INVOKEINTERFACE",
		L"INVOKESPECIAL",
		L"INVOKESTATIC",
		L"INVOKEVIRTUAL",
		L"IOR",
		L"IREM",
		L"IRETURN",
		L"ISHL",
		L"ISHR",
		L"ISTORE",
		L"ISUB",
		L"IXOR",
		L"JSR",
		L"JSR_W",
		L"LDC",
		L"LDC2_W",
		L"LDC_W",
		L"LOOKUPSWITCH",
		L"LOGIC_OR",
		L"LOGIC_AND",
		L"LOGIC_NOT",
		L"MONITORENTER",
		L"MONITOREXIT",
		L"MULTIANEWARRAY",
		L"NEW",
		L"NEWARRAY",
		L"NOP",
		L"PLOAD",
		L"POSTOP",
		L"POP",
		L"POP2",
		L"PREOP",
		L"PUTFIELD",
		L"PUTSTATIC",
		L"RET",
		L"RETURN",
		L"SWAP",
		L"TABLESWITCH",
		L"WALOAD",
		L"WASTORE",
		L"WIDE"
	};

	namespace heap {
		malloc_map heap_;		// HEAP: For storing raw memory allocations
	}

	// turn on to view Cx debugging
#ifdef _DEBUG
	bool cx_dev_debug_flag = true;
#else
	bool cx_dev_debug_flag = false;
#endif

	// Type sizes
	const size_t type_size[] = {
		sizeof(cx_bool),
		sizeof(cx_char),
		sizeof(cx_byte),
		sizeof(cx_int),
		sizeof(cx_real)
	};


	size_t heap::mem_mapping::count(void) {
		return (this->size / type_size[this->typecode]);
	}

	// Stack Ops
#define _POPS (--vpu.stack_ptr)
#define _PUSHS (vpu.stack_ptr++)

	// Top of stack
#define _TOS vpu.stack_ptr[-1]

#define _ADDRTOINT(addr) (uintptr_t)*&addr

	// Load Array or reference to stack
#define _ALOAD(t_, type) {      \
    cx_int index = _POPS->i_;     \
    void *mem = _POPS->a_;       \
    _PUSHS->t_ = *((type *) ((char *) mem + (index * sizeof (type))));  \
}

	// Store to memory
#define _ASTORE(t_, type) {			\
	type v_ = _POPS->t_;			\
	cx_int index = _POPS->i_;		\
	void *mem = _VALUE->a_;			\
	*((type *)(((char *)mem) + (index * sizeof(type)))) = v_;	\
}

	// Value object
#define _VALUE ((symbol_table_node *) this->vpu.inst_ptr->arg0.a_)->runstack_item
#define _JMP(t_) vpu.inst_ptr = this->vpu.code_ptr->begin() + ((int)vpu.inst_ptr->arg0.t_ - 1)

#define _IFICMP(op) {       \
    cx_int value2 = _POPS->i_; \
    cx_int value1 = _POPS->i_; \
    if(value1 op value2) _JMP(i_);  \
									}

#define _IF(op) if(_POPS->i_ op 0) _JMP(i_)

	// Binary Operators
#define _BIN_OP(t_, type, op)  { \
	type b = _POPS->t_; \
	type a = _POPS->t_; \
	_PUSHS->t_ = (a op b); \
					}

	// Unary Operators
#define _UNA_OP(t_, type, op)  { \
	type a = _POPS->t_; \
	_PUSHS->t_ = op(a); \
				}

	// Relational Operators (bool)
#define _REL_OP(t_, type, op)  { \
	type b = _POPS->t_; \
	type a = _POPS->t_; \
	_PUSHS->i_ = (a op b); \
					}

	cxvm::cxvm() {
		// Pointer to the allocated stack
		this->vpu.stack_ptr = stack;
		//this->lock();
	}

	cxvm::~cxvm(void) {
		this->unlock();
	}

	value *cxvm::push(void) {
		return _PUSHS;
	}

	void cxvm::enter_function(symbol_table_node *p_function_id) {
		this->p_my_function_id = p_function_id;

		this->vpu.code_ptr = &this->p_my_function_id->defined.routine.program_code;
		this->vpu.inst_ptr = std::begin(*this->vpu.code_ptr);

		// load variables
		if (!p_my_function_id->defined.routine.p_variable_ids.empty()) {
			for (auto &local : p_my_function_id->defined.routine.p_variable_ids) {
				_PUSHS->a_ = nullptr;
				local->runstack_item = &_TOS;
			}
		}
	}

	void cxvm::nano_sleep(int nano_secs = 5) {
		std::this_thread::sleep_for(
			std::chrono::nanoseconds(nano_secs)
			);
	}

	inline void cxvm::lock(void) {
		//while (!this->vm_lock.try_lock()) nano_sleep();
	}

	inline void cxvm::unlock(void) {
		//this->vm_lock.unlock();
	}

	void cxvm::go(void) {
		using namespace heap;

		for (vpu.inst_ptr = std::begin(*this->vpu.code_ptr);
		vpu.inst_ptr != std::end(*this->vpu.code_ptr);
			++vpu.inst_ptr) {

			switch (vpu.inst_ptr->op) {
			case opcode::AALOAD: _PUSHS->a_ = _VALUE->a_; break;
			case opcode::AASTORE: _VALUE->a_ = _POPS->a_; break;
			case opcode::ACONST_NULL: _PUSHS->a_ = nullptr; break;
			case opcode::ALOAD: _PUSHS->a_ = _VALUE->a_;  break;
			case opcode::ANEWARRAY: {
				size_t size = (size_t)_POPS->i_ * sizeof(void *); //data_type::type_size[(dt = static_cast<data_type::dtype> (vpu.inst_ptr->arg0.b_))];

				void **mem = (void **)malloc(size);
				assert(mem != nullptr);

				mem_mapping *mem_map = &heap_[_ADDRTOINT(mem)]; // point to, only 1 hash calculation

				/* Compile with -D INSTRUCTION_TEST if testing.
				 * If undefined, RAM gets released and tests allocating RAM
				 * will fail.   */

				 // assign mem to smart pointer, release using free()
				mem_map->shared_ref = heap::managedmem((uintptr_t *)mem, free);
				mem_map->size = size; // size
				mem_map->typecode = T_REFERENCE; // type
				mem_map->typeform = F_ARRAY;
				_PUSHS->a_ = (void *)mem;
			} break;
			case opcode::ARRAYLENGTH: {
				void *mem = _POPS->a_;
				assert(mem != nullptr);
				_PUSHS->i_ = heap_[_ADDRTOINT(mem)].count();
			} break;
			case opcode::ASTORE: _VALUE->a_ = _POPS->a_; break;
			case opcode::ATHROW: { // Throws a string message
				char *message = (char *)_POPS->a_;
				assert(message != nullptr);

				throw std::string(message);
			} break;
			case opcode::BALOAD:	_ALOAD(b_, cx_byte); break;
			case opcode::BASTORE:	_ASTORE(b_, cx_byte); break;
			case opcode::C2I:		_PUSHS->i_ = static_cast<cx_int> (_POPS->c_); break;
			case opcode::CALL: {
				symbol_table_node *p_function_id = (symbol_table_node *)vpu.inst_ptr->arg0.a_;
				std::unique_ptr<cxvm> cx = std::make_unique<cxvm>();

				// Load parameters from the stack
				std::vector<std::shared_ptr<symbol_table_node>>::reverse_iterator parameter = std::rbegin(p_function_id->defined.routine.p_parameter_ids);
				for (; parameter != std::rend(p_function_id->defined.routine.p_parameter_ids); ++parameter) {
					value *stack_item = ((value *)&_POPS->a_);

					symbol_table_node *p_node = parameter->get();
					p_node->runstack_item = stack_item;
				}

				_POPS;

				// Only push non-void functions to the stack.
				if (p_function_id->p_type->typecode != T_VOID) {
					p_function_id->runstack_item = _PUSHS;
				}

				cx->enter_function(p_function_id);
				cx->go();
			} break;
			case opcode::CALOAD: _ALOAD(c_, cx_char); break;
			case opcode::CASTORE: _ASTORE(c_, cx_char); break;
			case opcode::CHECKCAST: break;

				/** Duplicate the top operand stack value
				 * Duplicate the top value on the operand stack and push
				 * the duplicated value onto the operand stack. */
			case opcode::DUP: {
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
				mem_map->shared_ref = std::move(heap::managedmem((uintptr_t *)new_value_copy));
				mem_map->size = sizeof(value); // size
				mem_map->typecode = T_REFERENCE; // type
				mem_map->typeform = F_SCALAR;

				// Push new copy
				_PUSHS->a_ = (void *)new_value_copy;

			} break;
			case opcode::DUP2:		break;
			case opcode::DUP2_X1:	break;
			case opcode::DUP2_X2:	break;
			case opcode::DUP_X1:	break;
			case opcode::DUP_X2:	break;
			case opcode::D2I:		_PUSHS->i_ = static_cast<cx_int> (_POPS->d_); break;
			case opcode::DADD:		_BIN_OP(d_, cx_real, +); break;
			case opcode::DALOAD:	_ALOAD(d_, cx_real); break;
			case opcode::DASTORE:	_ASTORE(d_, cx_real); break;
			case opcode::DCONST:	_PUSHS->d_ = vpu.inst_ptr->arg0.d_; break;
			case opcode::DDIV:		_BIN_OP(d_, cx_real, / ); break;
			case opcode::DEQ_EQ:	_REL_OP(d_, cx_real, == ); break;
			case opcode::DGT:		_REL_OP(d_, cx_real, > ); break;
			case opcode::DGT_EQ:	_REL_OP(d_, cx_real, >= ); break;
			case opcode::DLOAD:		_PUSHS->d_ = _VALUE->d_; break;
			case opcode::DLT:		_REL_OP(d_, cx_real, < ); break;
			case opcode::DLT_EQ:	_REL_OP(d_, cx_real, <= ); break;
			case opcode::DMUL:		_BIN_OP(d_, cx_real, *); break;
			case opcode::DNEG:		_UNA_OP(d_, cx_real, -); break;
			case opcode::DNOT_EQ:	_REL_OP(d_, cx_real, != ); break;
			case opcode::DREM: {
				cx_real b = _POPS->d_;
				cx_real a = _POPS->d_;

				_PUSHS->d_ = fmod(a, b);
			}break;
			case opcode::DSTORE:	_VALUE->d_ = _POPS->d_; break;
			case opcode::DSUB:		_BIN_OP(d_, cx_real, -); break;
			case opcode::GETFIELD: break;
			case opcode::GETSTATIC: break;
			case opcode::GOTO:		_JMP(i_); break;
			case opcode::I2B:		_PUSHS->b_ = static_cast<cx_byte> (_POPS->i_); break;
			case opcode::I2C:		_PUSHS->c_ = static_cast<cx_char> (_POPS->i_); break;
			case opcode::I2D:		_PUSHS->d_ = static_cast<cx_real> (_POPS->i_); break;
			case opcode::IADD:		_BIN_OP(i_, cx_int, +); break;
			case opcode::IALOAD:	_ALOAD(i_, cx_int); break;
			case opcode::ILT:		_REL_OP(i_, cx_int, < ); break;
				// Bitwise AND
			case opcode::IAND:		_BIN_OP(i_, cx_int, &); break;
			case opcode::IASTORE:	_ASTORE(i_, cx_int); break;
			case opcode::ICMP:
				break;
			case opcode::ICONST:	_PUSHS->i_ = vpu.inst_ptr->arg0.i_; break;
			case opcode::IDIV:		_BIN_OP(i_, cx_int, / ); break;
			case opcode::IEQ_EQ:	_REL_OP(i_, cx_int, == ); break;
			case opcode::IFEQ: _IF(== ); break;
			case opcode::IFNE: _IF(!= ); break;
			case opcode::IFLT: _IF(< ); break;
			case opcode::IFGE: _IF(>= ); break;
			case opcode::IFGT: _IF(> ); break;
			case opcode::IFLE: _IF(<= ); break;

			case opcode::IF_ACMPEQ: {
				void *value2 = _POPS->a_;
				void *value1 = _POPS->a_;

				if (!memcmp(value1, value2, heap_[_ADDRTOINT(value1)].size)) _JMP(i_);
			} break;

			case opcode::IF_ACMPNE: {
				void *value2 = _POPS->a_;
				void *value1 = _POPS->a_;

				if (memcmp(value1, value2, heap_[_ADDRTOINT(value1)].size)) _JMP(i_);
			} break;

			case opcode::IF_ICMPEQ: _IFICMP(== ); break;
			case opcode::IF_ICMPNE: _IFICMP(!= ); break;
			case opcode::IF_ICMPLT: _IFICMP(< ); break;
			case opcode::IF_ICMPGE: _IFICMP(>= ); break;
			case opcode::IF_ICMPGT: _IFICMP(> ); break;
			case opcode::IF_ICMPLE: _IFICMP(<= ); break;
			case opcode::IFNONNULL: if (_POPS->a_ != nullptr) _JMP(i_); break;
			case opcode::IFNULL: if (_POPS->a_ == nullptr) _JMP(i_); break;
			case opcode::IGT:		_REL_OP(i_, cx_int, > ); break;
			case opcode::IGT_EQ:	_REL_OP(i_, cx_int, >= ); break;
			case opcode::IINC: {
				cx_int a = _POPS->i_;
				_PUSHS->i_ += vpu.inst_ptr->arg0.i_;
			}break;
			case opcode::ILOAD:		_PUSHS->i_ = _VALUE->i_; break;
			case opcode::ILT_EQ:	_REL_OP(i_, cx_int, <= ); break;
			case opcode::IMUL:		_BIN_OP(i_, cx_int, *); break;
			case opcode::INEG:		_UNA_OP(i_, cx_int, -); break;
				// Unary complement (bit inversion)
			case opcode::INOT: 		_UNA_OP(i_, cx_int, ~); break;
			case opcode::INOT_EQ:	_REL_OP(i_, cx_int, != ); break;
			case opcode::INSTANCEOF: break;
			case opcode::INVOKEDYNAMIC: break;
			case opcode::INVOKEFUNCT: break;
			case opcode::INVOKEINTERFACE: break;
			case opcode::INVOKESPECIAL: break;
			case opcode::INVOKESTATIC: break;
			case opcode::INVOKEVIRTUAL: break;
				// Bitwise inclusive OR
			case opcode::IOR:		_BIN_OP(i_, cx_int, | ); break;
			case opcode::IREM: 		_BIN_OP(i_, cx_int, %); break;
			case opcode::ISHL: 		_BIN_OP(i_, cx_int, << ); break;
			case opcode::ISHR: 		_BIN_OP(i_, cx_int, >> ); break;
			case opcode::ISTORE: {
				cx_int i = _POPS->i_;
				_VALUE->i_ = i;
			}break;
			case opcode::ISUB:		_BIN_OP(i_, cx_int, -); break;
				// Bitwise exclusive OR
			case opcode::IXOR: 		_BIN_OP(i_, cx_int, ^); break;
			case opcode::JSR:
			case opcode::JSR_W: break;
			case opcode::LDC:
			case opcode::LDC2_W:
			case opcode::LDC_W: break;
			case opcode::LOOKUPSWITCH: break;
			case opcode::LOGIC_OR:	_BIN_OP(z_, cx_bool, || ); break;
			case opcode::LOGIC_AND:	_BIN_OP(z_, cx_bool, &&); break;
			case opcode::LOGIC_NOT: _PUSHS->z_ = !_POPS->i_; break;
			case opcode::MONITORENTER:
			case opcode::MONITOREXIT: break;
			case opcode::MULTIANEWARRAY: break;
			case opcode::NEW: break;

				/** newarray: allocate new array
				 * @param: vpu.stack_ptr[-1].l_ - number of elements
				 * @param: vpu.inst_ptr->arg0.b_ - type code
				 * @return: new array allocation managed by GC */
			case opcode::NEWARRAY: {
				type_code dt;
				size_t size = static_cast<size_t>(type_size[(dt = static_cast<type_code> (vpu.inst_ptr->arg0.b_))] * _POPS->i_);

				void *mem = malloc(size);
				assert(mem != nullptr);

				heap::mem_mapping *mem_map = &heap_[_ADDRTOINT(mem)]; // point to, only 1 hash calculation

				/* Compile with -D INSTRUCTION_TEST if testing.
				* If undefined, RAM gets released and tests allocating RAM
				* will fail.   */

				// assign mem to smart pointer, release using free()
				mem_map->shared_ref = std::move(heap::managedmem((uintptr_t *)mem, free));
				mem_map->size = size; // size
				mem_map->typecode = dt; // type
				mem_map->typeform = F_ARRAY;
				_PUSHS->a_ = mem;
			} break;
			case opcode::NOP: break;
			case opcode::PLOAD: _PUSHS->a_ = _VALUE->a_; break;
			case opcode::POP: _POPS; break;
			case opcode::POP2: _POPS; _POPS; break;
			case opcode::PUTFIELD: break;
			case opcode::PUTSTATIC: break;
			case opcode::RET:
			case opcode::RETURN:
				return;
				break;
			case opcode::SWAP: break;
			case opcode::TABLESWITCH: break;
			case opcode::WALOAD: _ALOAD(c_, cx_char); break;
			case opcode::WASTORE: _ASTORE(c_, cx_char); break;
			case opcode::WIDE: break;

				/**
				 * Halts VM and returns
				 * @param prog
				 * @return exit status (int)
				 */
			case opcode::HALT:
				goto end;
			} //switch
		} // for

	end:
		return;
	}
}
