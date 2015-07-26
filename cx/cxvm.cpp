/*
*
*/

#include <iostream>
#include "cxvm.h"
#include "symtab.h"

namespace cx{

	const char* opcode_string[] = {
		"AALOAD",
		"AASTORE",
		"ACONST_NULL",
		"ALOAD",
		"ANEWARRAY",
		"ARRAYLENGTH",
		"ASTORE",
		"ATHROW",
		"BALOAD",
		"BASTORE",
		"BEQ",
		"BGE",
		"BGT",
		"BIPUSH",
		"BLE",
		"BLT",
		"BNE",
		"CALL",
		"CALOAD",
		"CASTORE",
		"CHECKCAST",
		"D2F",
		"D2I",
		"D2L",
		"DADD",
		"DALOAD",
		"DASTORE",
		"DCMP",
		"DCONST",
		"DDIV",
		"DLOAD",
		"DMUL",
		"DNEG",
		"DREM",
		"DRETURN",
		"DSTORE",
		"DSUB",
		"DUP",
		"DUP2",
		"DUP2_X1",
		"DUP2_X2",
		"DUP_X1",
		"DUP_X2",
		"GETFIELD",
		"GETSTATIC",
		"GOTO",
		"HALT",
		"I2B",
		"I2C",
		"I2D",
		"I2W",
		"IADD",
		"IALOAD",
		"IAND",
		"IASTORE",
		"ICMP",
		"ICONST",
		"IDIV",
		"IFEQ",
		"IFNE",
		"IFLT",
		"IFGE",
		"IFGT",
		"IFLE",
		"IF_ACMPEQ",
		"IF_ACMPNE",
		"IF_ICMPEQ",
		"IF_ICMPNE",
		"IF_ICMPLT",
		"IF_ICMPGE",
		"IF_ICMPGT",
		"IF_ICMPLE",
		"IFNONNULL",
		"IFNULL",
		"IINC",
		"ILOAD",
		"IMUL",
		"INEG",
		"INOT",
		"INSTANCEOF",
		"INVOKEDYNAMIC",
		"INVOKEFUNCT",
		"INVOKEINTERFACE",
		"INVOKESPECIAL",
		"INVOKESTATIC",
		"INVOKEVIRTUAL",
		"IOR",
		"IREM",
		"IRETURN",
		"ISHL",
		"ISHR",
		"ISTORE",
		"ISUB",
		"IUSHR",
		"IXOR",
		"JSR",
		"JSR_W",
		"LDC",
		"LDC2_W",
		"LDC_W",
		"LOOKUPSWITCH",
		"LOGIC_OR",
		"LOGIC_AND",
		"MONITORENTER",
		"MONITOREXIT",
		"MULTIANEWARRAY",
		"NEW",
		"NEWARRAY",
		"NOP",
		"PLOAD",
		"POSTOP",
		"POP",
		"POP2",
		"PREOP",
		"PUTFIELD",
		"PUTSTATIC",
		"RET",
		"RETURN",
		"SWAP",
		"TABLESWITCH",
		"WALOAD",
		"WASTORE",
		"WIDE"
	};

	namespace heap{
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
		sizeof(bool), 
		sizeof(char), 
		sizeof(uint8_t), 
		sizeof(wchar_t), 
		sizeof(cx_int), 
		sizeof(cx_real)
	};


	size_t heap::mem_mapping::count(void){
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
    cx_int *index = &_POPS->i_;     \
    void *mem = _POPS->a_;       \
    _PUSHS->t_ = *((type *) ((char *) mem + (*index * sizeof (type))));  \
					}

	// Store to memory
#define _ASTORE(t_, type) {     \
    type *v_ = &_POPS->t_;       \
    cx_int *index = &_POPS->i_;     \
    void *mem = _POPS->a_;       \
    *((type *) ((char *) mem + (*index * sizeof (type)))) = *v_;        \
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

	cxvm::cxvm(){
		// Pointer to the allocated stack
		this->vpu.stack_ptr = stack;
		//this->lock();
	}

	cxvm::~cxvm(void){
		this->unlock();
	}

	value *cxvm::push(void){
		return _PUSHS;
	}

	void cxvm::enter_function(symbol_table_node *p_function_id){
		this->p_my_function_id = p_function_id;

		this->vpu.code_ptr = &this->p_my_function_id->defined.routine.program_code;
		this->vpu.inst_ptr = this->vpu.code_ptr->begin();

		// load variables
		if (!p_my_function_id->defined.routine.p_variable_ids.empty()){
			for (auto &local : p_my_function_id->defined.routine.p_variable_ids){
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

	inline void cxvm::lock(void){
		//while (!this->vm_lock.try_lock()) nano_sleep();
	}

	inline void cxvm::unlock(void){
		//this->vm_lock.unlock();
	}

	void cxvm::go(void) {
		using namespace heap;

		for (vpu.inst_ptr = this->vpu.code_ptr->begin();
			vpu.inst_ptr != this->vpu.code_ptr->end();
			++vpu.inst_ptr) {

			switch (vpu.inst_ptr->op){
			case AALOAD: _PUSHS->a_ = _VALUE->a_; break;
			case AASTORE: _VALUE->a_ = _POPS->a_; break;
			case ACONST_NULL: _PUSHS->a_ = nullptr; break;
			case ALOAD: _PUSHS->a_ = _VALUE->a_;  break;
			case ANEWARRAY: {
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
			case ARRAYLENGTH: {
				void *mem = _POPS->a_;
				assert(mem != nullptr);
				_PUSHS->i_ = heap_[_ADDRTOINT(mem)].count();
			} break;
			case ASTORE: _VALUE->a_ = _POPS->a_; break;
			case ATHROW: { // Throws a string message
				char *message = (char *)_POPS->a_;
				assert(message != nullptr);

				throw std::string(message);
			} break;
			case BALOAD: _ALOAD(b_, uint8_t); break;
			case BASTORE: _ASTORE(b_, uint8_t); break;
			case CALL:{
				symbol_table_node *p_function_id = (symbol_table_node *)vpu.inst_ptr->arg0.a_;
				std::unique_ptr<cxvm> cx = std::make_unique<cxvm>();

				// Load parameters from the stack
				std::vector<std::shared_ptr<symbol_table_node>>::reverse_iterator parameter = p_function_id->defined.routine.p_parameter_ids.rbegin();
				for (; parameter != p_function_id->defined.routine.p_parameter_ids.rend(); ++parameter){
					value *stack_item = ((value *)&_POPS->a_);

					symbol_table_node *p_node = parameter->get();
					p_node->runstack_item = stack_item;
				}

				_POPS;
				p_function_id->runstack_item = _PUSHS;
				cx->enter_function(p_function_id);
				cx->go();

				std::cout << "function " << p_function_id->node_name << " returned " << p_function_id->runstack_item->i_ << std::endl;

				// If void function, pop off it's return value.
				if (p_function_id->p_type->typecode == T_VOID){
					_POPS;
				}
			} break;
			case CALOAD: _ALOAD(c_, char); break;
			case CASTORE: _ASTORE(c_, char); break;
			case CHECKCAST: break;

				/** Duplicate the top operand stack value
				 * Duplicate the top value on the operand stack and push
				 * the duplicated value onto the operand stack. */
			case DUP: {
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
			case DUP2: break;
			case DUP2_X1: break;
			case DUP2_X2: break;
			case DUP_X1: break;
			case DUP_X2: break;
			case D2I: _PUSHS->i_ = static_cast<cx_int> (_POPS->d_); break;
			case DADD: {
				cx_real b = _POPS->d_;
				cx_real a = _POPS->d_;

				_PUSHS->d_ = (a + b);
			}break;
			case DALOAD: _ALOAD(d_, cx_real); break;
			case DASTORE: _ASTORE(d_, cx_real); break;
			case DCONST: _PUSHS->d_ = vpu.inst_ptr->arg0.d_; break;
			case DDIV: {
				cx_real b = _POPS->d_;
				cx_real a = _POPS->d_;

				_PUSHS->d_ = (a / b);
			}break;
			case DLOAD: _PUSHS->d_ = _VALUE->d_; break;
			case DMUL: {
				cx_real b = _POPS->d_;
				cx_real a = _POPS->d_;

				_PUSHS->d_ = (a * b);
			}break;
			case DNEG: {
				cx_real a = _POPS->d_;

				_PUSHS->d_ = (-a);
			}break;
			case DREM: {
				cx_real b = _POPS->d_;
				cx_real a = _POPS->d_;

				_PUSHS->d_ = fmod(a ,b);
			}break;
			case DSTORE: _VALUE->d_ = _POPS->d_; break;
			case DSUB: {
				cx_real b = _POPS->d_;
				cx_real a = _POPS->d_;

				_PUSHS->d_ = (a - b);
			}break;
			case GETFIELD: break;
			case GETSTATIC: break;
			case GOTO: _JMP(i_); break;
			case I2B: _PUSHS->b_ = static_cast<uint8_t> (_POPS->i_); break;
			case I2C: _PUSHS->c_ = static_cast<char> (_POPS->i_); break;
			case I2D: _PUSHS->d_ = static_cast<cx_real> (_POPS->i_); break;
			case I2W: _PUSHS->w_ = static_cast<wchar_t> (_POPS->i_); break;
			case IADD:{
				cx_int b = _POPS->i_;
				cx_int a = _POPS->i_;

				_PUSHS->i_ = (a + b);
			}break;
			case IALOAD: _ALOAD(i_, cx_int); break;
				// Bitwise AND
			case IAND: {
				cx_int b = _POPS->i_;
				cx_int a = _POPS->i_;

				_PUSHS->i_ = (a & b);
			}break;
			case IASTORE: _ASTORE(i_, cx_int); break;
			case ICMP:
				break;
			case ICONST: _PUSHS->i_ = vpu.inst_ptr->arg0.i_; break;
			case IDIV: {
				cx_int b = _POPS->i_;
				cx_int a = _POPS->i_;

				_PUSHS->i_ = (a / b);
			}break;
			case IFEQ: _IF(== ); break;
			case IFNE: _IF(!= ); break;
			case IFLT: _IF(< ); break;
			case IFGE: _IF(>= ); break;
			case IFGT: _IF(> ); break;
			case IFLE: _IF(<= ); break;

			case IF_ACMPEQ: {
				void *value2 = _POPS->a_;
				void *value1 = _POPS->a_;

				if (!memcmp(value1, value2, heap_[_ADDRTOINT(value1)].size)) _JMP(i_);
			} break;

			case IF_ACMPNE: {
				void *value2 = _POPS->a_;
				void *value1 = _POPS->a_;

				if (memcmp(value1, value2, heap_[_ADDRTOINT(value1)].size)) _JMP(i_);
			} break;

			case IF_ICMPEQ: _IFICMP(== ); break;
			case IF_ICMPNE: _IFICMP(!= ); break;
			case IF_ICMPLT: _IFICMP(< ); break;
			case IF_ICMPGE: _IFICMP(>= ); break;
			case IF_ICMPGT: _IFICMP(> ); break;
			case IF_ICMPLE: _IFICMP(<= ); break;
			case IFNONNULL: if (_POPS->a_ != nullptr) _JMP(i_); break;
			case IFNULL: if (_POPS->a_ == nullptr) _JMP(i_); break;
			case IINC: {
				value *v_ = (value *)_POPS;
				cx_int i_ = _POPS->i_;
				int arg0 = static_cast<int>(vpu.inst_ptr->arg0.i_);
				opcode order = (opcode)vpu.inst_ptr->arg1.i_;

				switch (order)
				{
				case PREOP:{
					v_->i_ += arg0;
					_PUSHS->i_ = v_->i_;
				}break;
				case POSTOP:{
					i_ = v_->i_;
					v_->i_ += arg0;
					_PUSHS->i_ = i_;
				}
							break;
				default:
					break;
				}

			}break;
			case ILOAD: _PUSHS->i_ = _VALUE->i_; break;
			case IMUL: {
				cx_int b = _POPS->i_;
				cx_int a = _POPS->i_;

				_PUSHS->i_ = (a * b);
			}break;
			case INEG:{
				cx_int a = _POPS->i_;
				_PUSHS->i_ = (-a);
			}break;
				// Unary complement (bit inversion)
			case INOT: {
				cx_int a = _POPS->i_;
				_PUSHS->i_ = (~a);
			}break;
			case INSTANCEOF: break;
			case INVOKEDYNAMIC: break;
			case INVOKEFUNCT: break;
			case INVOKEINTERFACE: break;
			case INVOKESPECIAL: break;
			case INVOKESTATIC: break;
			case INVOKEVIRTUAL: break;
				// Bitwise inclusive OR
			case IOR:  {
				cx_int b = _POPS->i_;
				cx_int a = _POPS->i_;

				_PUSHS->i_ = (a | b);
			}break;
			case IREM: {
				cx_int b = _POPS->i_;
				cx_int a = _POPS->i_;

				_PUSHS->i_ = (a % b);
			}break;
			case ISHL: {
				cx_int b = _POPS->i_;
				cx_int a = _POPS->i_;

				_PUSHS->i_ = (a << b);
			}break;
			case ISHR: {
				cx_int b = _POPS->i_;
				cx_int a = _POPS->i_;

				_PUSHS->i_ = (a >> b);
			}break;
			case ISTORE: {
				cx_int i = _POPS->i_;
				_VALUE->i_ = i;
			}break;
			case ISUB:{
				cx_int b = _POPS->i_;
				cx_int a = _POPS->i_;

				_PUSHS->i_ = (a - b);
			}break;
				// Bitwise exclusive OR
			case IXOR: {
				cx_int b = _POPS->i_;
				cx_int a = _POPS->i_;

				_PUSHS->i_ = (a ^ b);
			}break;
			case JSR:
			case JSR_W: break;
			case LDC:
			case LDC2_W:
			case LDC_W: break;
			case LOOKUPSWITCH: break;
			case LOGIC_OR:{
				bool b = _POPS->z_;
				bool a = _POPS->z_;

				_PUSHS->z_ = (a || b);
			}break;
			case LOGIC_AND:{
				bool b = _POPS->z_;
				bool a = _POPS->z_;

				_PUSHS->z_ = (a && b);
			}break;
			case MONITORENTER:
			case MONITOREXIT: break;
			case MULTIANEWARRAY: break;
			case NEW: break;

				/** newarray: allocate new array
				 * @param: vpu.stack_ptr[-1].l_ - number of elements
				 * @param: vpu.inst_ptr->arg0.b_ - type code
				 * @return: new array allocation managed by GC */
			case NEWARRAY: {
				type_code dt;
				size_t size = static_cast<size_t>(_POPS->i_ * type_size[(dt = static_cast<type_code> (vpu.inst_ptr->arg0.b_))]);

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
			case NOP: break;
			case PLOAD: _PUSHS->a_ = _VALUE->a_; break;
			case POP: _POPS; break;
			case POP2: _POPS; _POPS; break;
			case PUTFIELD: break;
			case PUTSTATIC: break;
			case RET:
			case RETURN:
				return;
				break;
			case SWAP: break;
			case TABLESWITCH: break;
			case WALOAD: _ALOAD(w_, wchar_t); break;
			case WASTORE: _ASTORE(w_, wchar_t); break;
			case WIDE: break;

				/**
				 * Halts VM and returns
				 * @param prog
				 * @return exit status (int)
				 */
			case HALT:
				goto end;
			} //switch
		} // for

	end:
		// return frame header
		return;
			}
	}
