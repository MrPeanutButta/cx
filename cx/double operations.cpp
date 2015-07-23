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
			case F2D: _PUSHS->d_ = static_cast<double> (_POPS->f_); break;
			case I2D: _PUSHS->d_ = static_cast<double> (_POPS->i_); break;
			case L2D: _PUSHS->d_ = static_cast<double> (_POPS->l_); break;
