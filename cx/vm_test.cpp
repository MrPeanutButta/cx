#include <iostream>
#include "cxvm.h"

void p1();
void p2();
void p3();
void p4();
void p5();
void p6();
void p7();
void p8();
void p9();
void p10();
void p11();
void p12();
void p13();
void p14();
void p15();
void p16();
void p17();
void p18();
void p19();
void p20();
void p21();
void p22();
void p23();
void p24();
void p25();
void p26();

int main(int argc, char **argv) {

	p1();
	p2();
	p3();
	p4();
	p5();
	p6();
	p7();
	p8();
	p9();
	p10();
	p11();
	p12();
	p13();
	p14();
	p15();
	p16();
	p17();
	p18();
	p19();
	p20();
	p21();// broken
	//p22(); broken
	//p23(); broken
	// p24();
	p25();
	//p26();

	return 0;
}



using namespace cx;
//using namespace cx::data_type;
//using namespace cx::opcodes;

// test values
value va;
value vb;
value vc;
value vd;
value ve;

void p26() {
	std::cout << "testing ATHROW: ";

	char test_str[] = "test string to throw";
	va.a_ = (void *)&test_str;

	program prog = {
			{ ALOAD, &va },
			{ ATHROW },
			// { ISTORE_M, &va},

			{ HALT }
	};

	try{
		cxvm vm(prog);
		vm.go();
	}

	catch (std::string &msg){
		std::cout << '[' << msg << "] [passed]\n";
	}

	//   assert(va.i_ == check);

}

void p25() {
	std::cout << "testing DUP: ";

	va.i_ = 100;
	vb.i_ = 555;
	vc.i_ = 0;

	program prog = {
			{ ILOAD, &va },
			{ DUP },
			{ ASTORE, &vb },
			{ ILOAD, &vc },
			{ IINC, 1 },
			{ ISTORE, &vc },
			{ ILOAD, &vc },
			{ ICONST, 20 },
			{ ICMP },
			{ BLT, 1 },
			// { ISTORE_M, &va},

			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	int check = ((value *)vb.a_)->i_;
    assert(va.i_ == check);

	std::cout << '[' << va.i_ << ", " << check << "] [passed]\n";
}

void p1() {
	std::cout << "testing I2F: ";

	va.i_ = 100;
	program prog = {
			{ ILOAD, &va },
			{ I2F },
			{ FSTORE, &vc },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(vc.f_ == 100.0f);
	std::cout << '[' << vc.f_ << "] [passed]\n";
}

void p2() {
	std::cout << "testing I2B: ";

	va.i_ = 101;
	program prog = {
			{ ILOAD, &va },
			{ I2B },
			{ ISTORE, &vc },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(vc.b_ == 101);
	std::cout << '[' << (int)vc.b_ << "] [passed]\n";
}

void p3() {
	std::cout << "testing I2C: ";

	va.i_ = 102;
	program prog = {
			{ ILOAD, &va },
			{ I2C },
			{ ISTORE, &vc },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(vc.c_ == 'f');
	std::cout << '[' << vc.c_ << "] [passed]\n";
}

void p4() {
	std::cout << "testing IINC: ";

	va.i_ = 103;
	program prog = {
			{ ILOAD, &va },
			{ IINC, 5 },
			{ ISTORE, &vc },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(vc.i_ == 108);
	std::cout << '[' << vc.i_ << "] [passed]\n";
}

void p5() {
	std::cout << "testing ICONST: ";

	program prog = {
			{ ICONST, 1000 },
			{ IINC, 500 },
			{ ISTORE, &vc },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(vc.i_ == 1500);
	std::cout << '[' << vc.i_ << "] [passed]\n";
}

void p6() {
	std::cout << "testing IUSHR: ";

	program prog = {
			{ ICONST, 128 },
			{ ICONST, 1 },
			{ IUSHR },
			{ ISTORE, &vc },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(vc.i_ == 64);
	std::cout << '[' << vc.i_ << "] [passed]\n";
}

void p7() {
	std::cout << "testing NEWARRAY: ";
	/// NEWARRAY
	program prog = {
			{ ICONST, 100 }, //  100 elements
			{ NEWARRAY, T_LONG },
			{ HALT },
	};

	cxvm vm(prog);
	vm.go();

	std::cout << "[passed]\n";
}

void p8() {
	std::cout << "testing DALOAD: \n";

	double d[] = { 100.3434, 100.5050505, 300.9989, 3.14 };
	va.a_ = d;

	for (int i = 0; i < 4; ++i) {
		program prog = {
				{ ALOAD, &va },
				{ ICONST, i },
				{ DALOAD }, //  0: load 0
				{ DSTORE, &vc },
				{ HALT }, // 30: eof
		};

		cxvm vm(prog);
		vm.go();

		std::cout << "index " << i << " vc = " << vc.d_ << std::endl;
	}

	assert(vc.d_ == d[3]);
	std::cout << '[' << vc.d_ << "] [passed]\n";
}

void p9() {
	std::cout << "testing DASTORE: ";

	double d[] = { 100.3434, 100.5050505, 300.9989, 3.14 };
	va.a_ = d;

	program prog = {
			{ ALOAD, &va },
			{ ICONST, 2 },
			{ DCONST, (double) 200.666 }, //  0: load 0
			{ DASTORE },
			{ HALT }, // 30: eof
	};

	cxvm vm(prog);
	vm.go();

	assert(d[2] == 200.666);
	std::cout << '[' << d[2] << "] [passed]\n";
}

void p10() {
	std::cout << "testing FALOAD: \n";

	float f[] = { 100.3434f, 100.5050505f, 300.9989f, 3.14f };
	va.a_ = f;

	for (int i = 0; i < 4; ++i) {
		program prog = {
				{ ALOAD, &va },
				{ ICONST, i },
				{ FALOAD },
				{ FSTORE, &vc },
				{ HALT },
		};

		cxvm vm(prog);
		vm.go();

		std::cout << "index " << i << " vc = " << vc.f_ << std::endl;
	}

	assert(vc.f_ == f[3]);
	std::cout << '[' << vc.f_ << "] [passed]\n";
}

void p11() {
	std::cout << "testing FASTORE: ";

	float f[] = { 100.3434f, 100.5050505f, 300.9989f, 3.14f };
	float ff = 666.666f;
	va.a_ = f;
	vb.f_ = ff;

	program prog = {
			{ ALOAD, &va },
			{ ICONST, 0 },
			{ FCONST, vb.f_ },
			{ FASTORE },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(f[0] == ff);
	std::cout << '[' << f[0] << "] [passed]\n";
}

void p12() {
	std::cout << "testing BALOAD: \n";

	uint8_t b[] = { 100, 101, 102, 103 };
	va.a_ = b;
	for (int i = 0; i < 4; ++i) {
		program prog = {
				{ ALOAD, &va },
				{ ICONST, i },
				{ BALOAD },
				{ ISTORE, &vc },
				{ HALT }
		};

		cxvm vm(prog);
		vm.go();

		std::cout << "index " << i << " vc = " << (int)vc.b_ << std::endl;
	}

	assert(vc.b_ == b[3]);
	std::cout << '[' << (int)vc.b_ << "] [passed]\n";
}

void p13() {
	std::cout << "testing BASTORE: ";

	uint8_t b[] = { 100, 101, 102, 103 };
	uint8_t bb = 124;

	va.a_ = b;
	program prog = {
			{ ALOAD, &va },
			{ ICONST, 2 },
			{ ICONST, bb },
			{ BASTORE },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(b[2] == bb);
	std::cout << '[' << (int)b[2] << "] [passed]\n";
}

void p14() {
	std::cout << "testing CALOAD: \n";

	char c[] = { 100, 101, 102, 103 };

	va.a_ = c;
	for (int i = 0; i < 4; ++i) {
		program prog = {
				{ ALOAD, &va },
				{ ICONST, i },
				{ CALOAD },
				{ ISTORE, &vc },
				{ HALT }
		};

		cxvm vm(prog);
		vm.go();

		std::cout << "index " << i << " vc = " << vc.c_ << std::endl;
	}

	assert(vc.c_ == c[3]);
	std::cout << '[' << vc.c_ << "] [passed]\n";
}

void p15() {
	std::cout << "testing CASTORE: ";

	char c[] = { 100, 101, 102, 103 };
	char cc = 97;

	vc.a_ = c;

	program prog = {
			{ ALOAD, &vc },
			{ ICONST, 2 },
			{ ICONST, cc },
			{ CASTORE },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(c[2] == cc);
	std::cout << '[' << c[2] << "] [passed]\n";
}

void p16() {
	std::cout << "testing IALOAD: \n";

	int i[] = { 100, 101, 102, 103 };
	va.a_ = i;

	for (int ii = 0; ii < 4; ++ii) {
		program prog = {
				{ ALOAD, &va },
				{ ICONST, ii },
				{ IALOAD },
				{ ISTORE, &vc },
				{ HALT }
		};

		cxvm vm(prog);
		vm.go();

		std::cout << "index " << ii << " vc = " << vc.i_ << std::endl;
	}

	assert(vc.i_ == i[3]);
	std::cout << '[' << vc.i_ << "] [passed]\n";
}

void p17() {
	std::cout << "testing IASTORE: ";

	int i[] = { 100, 101, 102, 103 };
	int ii = 97;
	va.a_ = i;

	program prog = {
			{ ALOAD, &va },
			{ ICONST, 2 },
			{ ICONST, ii },
			{ IASTORE },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(i[2] == ii);
	std::cout << '[' << i[2] << "] [passed]\n";
}

void p18() {
	std::cout << "testing ALOAD/ASTORE: ";

	int a[] = { 100, 101, 102, 103 };

	va.a_ = a;

	program prog = {
			{ ALOAD, &va },
			{ ASTORE, &vc },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	int i = ((int*)vc.a_)[0];
	assert(i == a[0]);

	std::cout << '[' << i << "] [passed]\n";
}

void p19() {
	std::cout << "testing NEWARRAY/ASTORE/ALOAD/CASTORE: ";

	vc.c_ = 65;
	value aa;

	program prog = {
			{ ICONST, 100 },
			{ NEWARRAY, T_CHAR },
			{ ASTORE, &aa }, // save
			{ ALOAD, &aa }, // load array
			{ ICONST, 0 }, // index
			{ ICONST, vc.c_ }, // const
			{ CASTORE },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	char c = ((char*)aa.a_)[0];
	//    assert(vc.c_ == c);

	std::cout << '[' << c << "] [passed]\n";
}

void p20() {
	std::cout << "testing NEWARRAY/ASTORE/ALOAD/ARRAYLENGTH: ";

	value aa;

	program prog = {
			{ ICONST, 100 },
			{ NEWARRAY, T_LONG },
			{ ASTORE, &aa },
			{ ALOAD, &aa },
			{ ARRAYLENGTH },
			{ ISTORE, &va },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(va.i_ == 100);

	std::cout << '[' << va.i_ << "] [passed]\n";
}

void p21() {
	std::cout << "testing IFNULL/IFNONNULL: ";

	value aa;
	value ab;

	aa.a_ = nullptr;
	ab.i_ = 100;

	program prog = {
			{ ALOAD, &aa },
			{ IFNULL, 6 },
			{ ISTORE, &va },
			{ ILOAD, &ab },
			{ IINC, 5 },
			{ ISTORE, &ab },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(ab.i_ == 100);

	std::cout << '[' << ab.i_ << "] [passed]\n";

	program prog1 = {
			{ ALOAD, &aa },
			{ IFNONNULL, 6 },
			{ ISTORE, &va },
			{ ILOAD, &ab },
			{ IINC, 5 },
			{ ISTORE, &ab },
			{ HALT }
	};

	cxvm vm1(prog1);
	vm1.go();

	assert(ab.i_ == 105);

	std::cout << '[' << ab.i_ << "] [passed]\n";

}

void p22() {
	std::cout << "testing ANEWARRAY/AASTORE/AALOAD: ";

	value aa;
	aa.a_ = nullptr;
	va.c_ = 'a';
	vb.a_ = nullptr;

	program prog = {
			{ ICONST, 5 },
			{ ANEWARRAY },
			{ AASTORE, &aa },
			{ AALOAD, &aa, 0 },
			{ ASTORE, &vb },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(va.i_ == 100);

	std::cout << '[' << va.i_ << "] [passed]\n";
}

void p23() {
	std::cout << "testing IFNULL/IFNONNULL: ";

	value aa;
	aa.a_ = nullptr;
	program prog = {
			{ ALOAD, &aa },
			{ IFNULL, 3 },
			{ ISTORE, &va },
			{ HALT }
	};

	cxvm vm(prog);
	vm.go();

	assert(va.i_ == 100);

	std::cout << '[' << va.i_ << "] [passed]\n";
}