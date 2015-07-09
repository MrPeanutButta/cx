/*
*
*/

#include "cxvm.h"
#include "symtab.h"

namespace cx{
	// Type sizes
	const size_t type_size[] = {
		sizeof(bool), sizeof(char), sizeof(uint8_t), sizeof(short),
		sizeof(wchar_t), sizeof(int), sizeof(float), sizeof(double),
		sizeof(long)
	};


	size_t heap::mem_mapping::length(void){
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
    int *index = &_POPS->i_;     \
    void *mem = _POPS->a_;       \
    _PUSHS->t_ = *((type *) ((char *) mem + (*index * sizeof (type))));  \
				}

	// Store to memory
#define _ASTORE(t_, type) {     \
    type *v_ = &_POPS->t_;       \
    int *index = &_POPS->i_;     \
    void *mem = _POPS->a_;       \
    *((type *) ((char *) mem + (*index * sizeof (type)))) = *v_;        \
				}

	// Value object
#define _VALUE ((symbol_table_node *) this->vpu.inst_ptr->arg0.a_)->runstack_item
#define _JMP(t_) vpu.inst_ptr = this->vpu.code_ptr->begin() + (vpu.inst_ptr->arg0.t_ - 1)

#define _IFICMP(op) {       \
    int value2 = _POPS->i_; \
    int value1 = _POPS->i_; \
    if(value1 op value2) _JMP(s_);  \
				}

#define _IF(op) if(_POPS->i_ op 0) _JMP(s_)

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
	
//		using namespace opcodes;
#ifdef INSTRUCTION_TEST
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
#endif

		for (vpu.inst_ptr = this->vpu.code_ptr->begin();
			vpu.inst_ptr != this->vpu.code_ptr->end();
			++vpu.inst_ptr) {

			switch (vpu.inst_ptr->op){
			case AALOAD: _PUSHS->a_ = _VALUE->a_; break;
			case AASTORE: _VALUE->a_ = _POPS->a_; break;
			case ACONST_NULL: _PUSHS->a_ = nullptr; break;
			case ALOAD: _PUSHS->a_ = _VALUE->a_;  break;
			case ANEWARRAY: {
				size_t size = _POPS->i_ * sizeof(void *); //data_type::type_size[(dt = static_cast<data_type::dtype> (vpu.inst_ptr->arg0.b_))];

				void **mem = (void **)malloc(size);
				assert(mem != nullptr);

				heap::mem_mapping *mem_map = &heap_[_ADDRTOINT(mem)]; // point to, only 1 hash calculation

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
			case ARETURN: break;
			case ARRAYLENGTH: {
				void *mem = _POPS->a_;
				assert(mem != nullptr);
				_PUSHS->i_ = heap_[_ADDRTOINT(mem)].length();
			} break;
			case ASTORE: _VALUE->a_ = _POPS->a_; break;
			case ATHROW: { // Throws a string message
				char *message;
				assert((message = (char *)_POPS->a_) != nullptr);

				throw std::string(message);
			} break;
			case BALOAD: _ALOAD(b_, uint8_t); break;
			case BASTORE: _ASTORE(b_, uint8_t); break;
			case BEQ: if (!vpu.flag) _JMP(i_); break;
			case BGE: if (vpu.flag >= 0) _JMP(i_); break;
			case BGT: if (vpu.flag > 0) _JMP(i_); break;
			case BIPUSH: _PUSHS->b_ = vpu.inst_ptr->arg0.i_; break;
			case BLE: if (vpu.flag <= 0) _JMP(i_); break;
			case BLT: if (vpu.flag < 0) _JMP(i_); break;
			case BNE: if (vpu.flag) _JMP(i_); break;
			case CALL:{
				symbol_table_node *p_function_id = (symbol_table_node *)vpu.inst_ptr->arg0.a_;
				std::unique_ptr<cxvm> cx = std::make_unique<cxvm>();

				// Load parameters from the stack
				std::vector<std::shared_ptr<symbol_table_node>>::reverse_iterator parameter = p_function_id->defined.routine.p_parameter_ids.rbegin();
				for (; parameter != p_function_id->defined.routine.p_parameter_ids.rend(); ++parameter){
					value *stack_item = ((value *)&_TOS.a_);
					
					_POPS;

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
			case D2F: _PUSHS->f_ = static_cast<float> (_POPS->d_); break;
			case D2I: _PUSHS->i_ = static_cast<int> (_POPS->d_); break;
			case D2L: _PUSHS->l_ = static_cast<long> (_POPS->d_); break;
			case DADD: _TOS.d_ += vpu.stack_ptr->d_; break;
			case DALOAD: _ALOAD(d_, double); break;
			case DASTORE: _ASTORE(d_, double); break;
			case DCMP:
				vpu.stack_ptr -= 2;
				vpu.flag = static_cast<int>(vpu.stack_ptr->d_ - vpu.stack_ptr[1].d_);
				break;
			case DCONST: _PUSHS->d_ = vpu.inst_ptr->arg0.d_; break;
			case DDIV: _TOS.d_ /= vpu.stack_ptr->d_; break;
			case DLOAD: _PUSHS->d_ = _VALUE->d_; break;
			case DMUL: _TOS.d_ *= vpu.stack_ptr->d_; break;
			case DNEG: vpu.stack_ptr[-1].d_ = -vpu.stack_ptr[-1].d_; break;
			case DREM: _TOS.d_ = fmod(vpu.stack_ptr[-1].d_, vpu.stack_ptr->d_); break;
//			case DRETURN: vpu.inst_ptr = --vpu.stack_ptr->i_ + vpu.base_ptr; break;
			case DSTORE: _VALUE->d_ = _POPS->d_; break;
			case DSUB: _TOS.d_ -= vpu.stack_ptr->d_; break;

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
			case F2D: _PUSHS->d_ = static_cast<double> (_POPS->f_); break;
			case F2I: _PUSHS->i_ = static_cast<int> (_POPS->f_); break;
			case F2L: _PUSHS->l_ = static_cast<long> (_POPS->f_); break;
			case FADD: _TOS.f_ += vpu.stack_ptr->f_; break;
			case FALOAD: _ALOAD(f_, float); break;
			case FASTORE: _ASTORE(f_, float); break;
			case FCMP:
				vpu.stack_ptr -= 2;
				vpu.flag = static_cast<int16_t>(vpu.stack_ptr->f_ - vpu.stack_ptr[1].f_);
				break;
			case FCONST: _PUSHS->f_ = vpu.inst_ptr->arg0.f_; break;
			case FDIV: _TOS.f_ /= vpu.stack_ptr->f_; break;
			case FLOAD: _PUSHS->f_ = _VALUE->f_; break;
			case FMUL: _TOS.f_ *= vpu.stack_ptr->f_; break;
			case FNEG: vpu.stack_ptr[-1].f_ = -vpu.stack_ptr[-1].f_; break;
			case FREM: _TOS.f_ = fmod(vpu.stack_ptr[-1].f_, vpu.stack_ptr->f_); break;
//			case FRETURN: vpu.inst_ptr = _POPS->i_ + vpu.base_ptr; break;
			case FSTORE: _VALUE->f_ = _POPS->f_; break;
			case FSUB: _TOS.f_ -= vpu.stack_ptr->f_; break;
			case GETFIELD: break;
			case GETSTATIC: break;
			case GOTO: _JMP(s_); break;
			case GOTO_W: _JMP(i_); break;
			case I2B: _PUSHS->b_ = static_cast<uint8_t> (_POPS->i_); break;
			case I2C: _PUSHS->c_ = static_cast<char> (_POPS->i_); break;
			case I2D: _PUSHS->d_ = static_cast<double> (_POPS->i_); break;
			case I2F: _PUSHS->f_ = static_cast<float> (_POPS->i_); break;
			case I2L: _PUSHS->l_ = static_cast<long> (_POPS->i_); break;
			case I2S: _PUSHS->s_ = static_cast<short> (_POPS->i_); break;
			case IADD:{
				int a = _POPS->i_;
				int b = _POPS->i_;
				_PUSHS->i_ = (a + b);
			}break;
			case IALOAD: _ALOAD(i_, int); break;
			case IAND: _TOS.i_ &= vpu.stack_ptr->i_; break;
			case IASTORE: _ASTORE(i_, int); break;
			case ICMP:
				vpu.stack_ptr -= 2;
				vpu.flag = vpu.stack_ptr->i_ - vpu.stack_ptr[1].i_;
				break;
			case ICONST: _PUSHS->i_ = vpu.inst_ptr->arg0.i_; break;
			case IDIV: _TOS.i_ /= vpu.stack_ptr->i_; break;
			case IFEQ: _IF(== ); break;
			case IFNE: _IF(!= ); break;
			case IFLT: _IF(< ); break;
			case IFGE: _IF(>= ); break;
			case IFGT: _IF(> ); break;
			case IFLE: _IF(<= ); break;

			case IF_ACMPEQ: {
				void *value2 = _POPS->a_;
				void *value1 = _POPS->a_;

				if (!memcmp(value1, value2, heap_[_ADDRTOINT(value1)].size)) _JMP(s_);
			} break;

			case IF_ACMPNE: {
				void *value2 = _POPS->a_;
				void *value1 = _POPS->a_;

				if (memcmp(value1, value2, heap_[_ADDRTOINT(value1)].size)) _JMP(s_);
			} break;

			case IF_ICMPEQ: _IFICMP(== ); break;
			case IF_ICMPNE: _IFICMP(!= ); break;
			case IF_ICMPLT: _IFICMP(< ); break;
			case IF_ICMPGE: _IFICMP(>= ); break;
			case IF_ICMPGT: _IFICMP(> ); break;
			case IF_ICMPLE: _IFICMP(<= ); break;
			case IFNONNULL: if (_POPS->a_ != nullptr) _JMP(s_); break;
			case IFNULL: if (_POPS->a_ == nullptr) _JMP(s_); break;
			case IINC: {
				value *v_ = (value *)_POPS;
				int i_ = _POPS->i_;
				int arg0 = vpu.inst_ptr->arg0.i_;
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
			case IMUL: _TOS.i_ *= vpu.stack_ptr->i_; break;
			case INEG: vpu.stack_ptr[-1].i_ = -vpu.stack_ptr[-1].i_; break;
			case INOT: vpu.stack_ptr[-1].i_ = !vpu.stack_ptr[-1].i_; break;
			case INSTANCEOF: break;
			case INVOKEDYNAMIC: break;
			case INVOKEFUNCT: break;
			case INVOKEINTERFACE: break;
			case INVOKESPECIAL: break;
			case INVOKESTATIC: break;
			case INVOKEVIRTUAL: break;
			case IOR: _TOS.i_ |= vpu.stack_ptr->i_; break;
			case IREM: _TOS.i_ %= vpu.stack_ptr->i_; break;
//			case IRETURN: vpu.inst_ptr = --vpu.stack_ptr->i_ + vpu.base_ptr; break;
			case ISHL: _TOS.i_ <<= vpu.stack_ptr->i_; break;
			case ISHR: _TOS.i_ >>= vpu.stack_ptr->i_; break;
			case ISTORE: {
				int i = _POPS->i_;
				_VALUE->i_ = i;
			}break;
			case ISUB: _TOS.i_ -= vpu.stack_ptr->i_; break;
			case IUSHR: _TOS.i_ = (int)((unsigned int)vpu.stack_ptr[-1].i_ >> vpu.stack_ptr->i_); break;
			case IXOR: _TOS.i_ ^= vpu.stack_ptr->i_; break;
			case JSR:
			case JSR_W: break;
			case L2D: _PUSHS->d_ = static_cast<double> (_POPS->l_); break;
			case L2F: _PUSHS->f_ = static_cast<float> (_POPS->l_); break;
			case L2I: _PUSHS->i_ = static_cast<int> (_POPS->l_); break;
			case LADD: _TOS.l_ += vpu.stack_ptr->l_; break;
			case LALOAD: _ALOAD(l_, long); break;
			case LAND: _TOS.l_ &= vpu.stack_ptr->l_; break;
			case LASTORE: _ASTORE(l_, long); break;
			case LCMP:
				vpu.stack_ptr -= 2;
				vpu.flag = static_cast<int16_t>(vpu.stack_ptr->l_ - vpu.stack_ptr[1].l_);
				break;
			case LCONST: _PUSHS->l_ = vpu.inst_ptr->arg0.l_; break;
			case LDC:
			case LDC2_W:
			case LDC_W: break;
			case LDIV: _TOS.l_ /= vpu.stack_ptr->l_; break;
			case LLOAD: _PUSHS->l_ = _VALUE->l_; break;
			case LMUL: _TOS.l_ *= vpu.stack_ptr->l_; break;
			case LNEG: vpu.stack_ptr[-1].l_ = -vpu.stack_ptr[-1].l_; break;
			case LOOKUPSWITCH: break;
			case LOR: _TOS.l_ |= vpu.stack_ptr->l_; break;
			case LREM: _TOS.l_ %= vpu.stack_ptr->l_; break;
//			case LRETURN: vpu.inst_ptr = _POPS->i_ + vpu.base_ptr; break;
			case LSHL: _TOS.l_ <<= vpu.stack_ptr->l_; break;
			case LSHR: _TOS.l_ >>= vpu.stack_ptr->l_; break;
			case LSTORE: _VALUE->l_ = _POPS->i_; break;
			case LSUB: _TOS.l_ -= vpu.stack_ptr->l_; break;
			case LUSHR: _TOS.l_ = (long)((unsigned long)vpu.stack_ptr[-1].l_ >> vpu.stack_ptr->l_); break;
			case LXOR: _TOS.l_ ^= vpu.stack_ptr->l_; break;
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
				size_t size = _POPS->i_ * type_size[(dt = static_cast<type_code> (vpu.inst_ptr->arg0.b_))];

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
			case SALOAD: _ALOAD(s_, short); break;
			case SASTORE: _ASTORE(s_, short); break;
			case SIPUSH: _PUSHS->s_ = vpu.inst_ptr->arg0.i_; break;
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
#ifdef INSTRUCTION_TEST
				std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> (t2 - t1);
				std::cout << " finished vpu execution in: " << std::fixed << time_span.count() << "(secs) ";
#endif	
				break;
				goto end;
			} //switch
		} // for

	end:
		// return frame header
		return;
	}
}
