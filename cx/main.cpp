/** main
 * main.cpp
 *
 * main entry point for the Cx interpretor.
 */

#include <iostream>
#include <memory>
#include <vld.h>

#ifdef __CX_PROFILE_EXECUTION__
#include <chrono>

#ifdef _WIN32_LEAK_DETECT
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC // below for leak detection on windows
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif

#endif

#include "error.h"
#include "buffer.h"
#include "parser.h"
#include "symtab.h"

const char* opcode_string[] = {
	"aaload",
	"aastore",
	"aconst_null",
	"aload",
	"anewarray",
	"areturn",
	"arraylength",
	"astore",
	"athrow",
	"baload",
	"bastore",
	"beq",
	"bge",
	"bgt",
	"bipush",
	"ble",
	"blt",
	"bne",
	"call",
	"caload",
	"castore",
	"checkcast",
	"d2f",
	"d2i",
	"d2l",
	"dadd",
	"daload",
	"dastore",
	"dcmp",
	"dconst",
	"ddiv",
	"dload",
	"dmul",
	"dneg",
	"drem",
	"dreturn",
	"dstore",
	"dsub",
	"dup",
	"dup2",
	"dup2_x1",
	"dup2_x2",
	"dup_x1",
	"dup_x2",
	"f2d",
	"f2i",
	"f2l",
	"fadd",
	"faload",
	"fastore",
	"fcmp",
	"fconst",
	"fdiv",
	"fload",
	"fmul",
	"fneg",
	"frem",
	"freturn",
	"fstore",
	"fsub",
	"getfield",
	"getstatic",
	"goto",
	"goto_w",
	"halt",
	"i2b",
	"i2c",
	"i2d",
	"i2f",
	"i2l",
	"i2s",
	"iadd",
	"iaload",
	"iand",
	"iastore",
	"icmp",
	"iconst",
	"idiv",
	"ifeq",
	"ifne",
	"iflt",
	"ifge",
	"ifgt",
	"ifle",
	"if_acmpeq",
	"if_acmpne",
	"if_icmpeq",
	"if_icmpne",
	"if_icmplt",
	"if_icmpge",
	"if_icmpgt",
	"if_icmple",
	"ifnonnull",
	"ifnull",
	"iinc",
	"iload",
	"imul",
	"ineg",
	"inot",
	"instanceof",
	"invokedynamic",
	"invokefunct",
	"invokeinterface",
	"invokespecial",
	"invokestatic",
	"invokevirtual",
	"ior",
	"irem",
	"ireturn",
	"ishl",
	"ishr",
	"istore",
	"isub",
	"iushr",
	"ixor",
	"jsr",
	"jsr_w",
	"l2d",
	"l2f",
	"l2i",
	"ladd",
	"laload",
	"land",
	"lastore",
	"lcmp",
	"lconst",
	"ldc",
	"ldc2_w",
	"ldc_w",
	"ldiv",
	"lload",
	"lmul",
	"lneg",
	"lookupswitch",
	"lor",
	"lrem",
	"lreturn",
	"lshl",
	"lshr",
	"lstore",
	"lsub",
	"lushr",
	"lxor",
	"monitorenter",
	"monitorexit",
	"multianewarray",
	"new",
	"newarray",
	"nop",
	"pload",
	"postop",
	"pop",
	"pop2",
	"preop",
	"putfield",
	"putstatic",
	"ret",
	"return",
	"saload",
	"sastore",
	"sipush",
	"swap",
	"tableswitch",
	"waload",
	"wastore",
	"wide"
};

namespace cx{
	// turn on to view Cx debugging
#ifdef _DEBUG
	bool cx_dev_debug_flag = true;
#else
	bool cx_dev_debug_flag = false;
#endif
}

void set_options(int argc, char **argv);

/** main        main entry point
 *
 * @param argc
 * @param argv
 * @return exit status.
 */
int main(int argc, char *argv[]) {
	using namespace cx;
//	using namespace cx::error;

    set_options(argc, argv);

    // Check the command line arguments.
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <source file>" << std::endl;
        abort_translation(ABORT_INVALID_COMMANDLINE_ARGS);
    }

    list_flag = cx_dev_debug_flag;
    error_arrow_flag = cx_dev_debug_flag;

    // Create the parser for the source file,
    // and then parse the file.
	std::shared_ptr<cx::parser> parser = std::make_shared<cx::parser>(new source_buffer(argv[1]));

#ifdef __CX_PROFILE_EXECUTION__
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif
    symbol_table_node_ptr p_program_id = parser->parse();

	std::ofstream output("debug.cxvm");
	output << "function:\t" << p_program_id->node_name << "\naddress:\t" << p_program_id << std::endl;
	for (auto &code : p_program_id->defined.routine.program_code){
		output << opcode_string[code.op] << "\t\t" << code.arg0.a_ << " " << code.arg1.a_ << std::endl;
	}

	for (auto node : p_global_symbol_table->symbols){
		if ((node.second->defined.defined_how == DC_FUNCTION) || (node.second->defined.defined_how == DC_PROGRAM)){
			output << "\nfunction:\t" << node.second->node_name << "\naddress:\t" << node.second << std::endl;
			for (auto &code : node.second->defined.routine.program_code){
				output << opcode_string[code.op] << "\t\t" << code.arg0.a_ << " " << code.arg1.a_ << std::endl;
			}
		}
	}

	output.close();

#ifdef __CX_PROFILE_EXECUTION__
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast < duration<double >> (t2 - t1);
    std::cout << "finished parsing in: " << time_span.count() << "(secs)" << std::endl;
#endif

    if (error_count == 0) {

		std::unique_ptr<cxvm> cx = std::make_unique<cxvm>();
        //std::unique_ptr<cx_backend> backend(new cx_executor);

#ifdef __CX_PROFILE_EXECUTION__
       // std::cin.get();
        t1 = high_resolution_clock::now();
#endif
		p_program_id->runstack_item = cx->push();
		cx->enter_function(p_program_id.get());
		cx->go();

		std::cout << p_program_id->node_name << " returned " << p_program_id->runstack_item->i_ << std::endl;

#ifdef __CX_PROFILE_EXECUTION__
        t2 = high_resolution_clock::now();
        time_span = duration_cast < duration<double >> (t2 - t1);
        std::cout << "finished executing in: " << time_span.count() << "(secs)" << std::endl;

#ifdef _WIN32_LEAK_DETECT
#ifdef _WIN32
        _CrtDumpMemoryLeaks();
#endif
#endif
        std::cin.get();
#endif

    }

	return p_program_id->runstack_item->i_;;
}

void set_options(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp("-ddev", argv[i])) cx::cx_dev_debug_flag = true;
    }
}