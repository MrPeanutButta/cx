#include <sstream>
#include "cxstdio.h"
#include "types.h"
#include "symtable.h"
#include <cstdio>
#include <iostream>

cx_type *cx_stdio::puts(cx_executor *cx,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	// first param as string
	std::string _string = (const char *)cx->top()->basic_types.addr__;

	cx->pop();

	// get output stream node from top of stack
	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
	
	bool ret_val = (p_snode->p_type->_iostream != nullptr);

	if (ret_val){
		p_snode->p_type->_iostream->write(_string);
		ret_val = !(p_snode->p_type->_iostream->fail());
	}

	cx->pop();

	// push return value of (!ios::fail)
	cx->push(ret_val);

	return cx_function_id->p_type;
}

/* Opens a file indicated by filename and returns a file stream associated 
 * with that file. mode is used to determine the file access mode. */
cx_type *cx_stdio::open(cx_executor *cx,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	// char *mode
	const char *mode = (const char *)cx->top()->basic_types.addr__;
	cx->pop();

	// char *filename
	const char *filename = (const char *)cx->top()->basic_types.addr__;
	cx->pop();

	// open stream
	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
	bool ret_val = true;

	// alloc new streams and redirect stream buffer
	if (p_snode->p_type->_iostream == nullptr){
		p_snode->p_type->_iostream = new cx_iostream();
		ret_val = p_snode->p_type->_iostream->open(filename);
	}
	else {
		ret_val = p_snode->p_type->_iostream->open(filename);
	}

	cx->pop();

	cx->push(ret_val);

	return cx_function_id->p_type;
}

cx_type *cx_stdio::reopen(cx_executor *cx,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	// char *mode
	const char *mode = (const char *)cx->top()->basic_types.addr__;
	cx->pop();

	// char *filename
	const char *filename = (const char *)cx->top()->basic_types.addr__;
	cx->pop();

	// open stream
//	FILE *stream = ((cx_symtab_node *)cx->top()->basic_types.addr__)->p_type->stream.p_file_stream;
	//stream = std::freopen(filename, mode, stream);
	cx->pop();

///	cx->push((bool)(stream != nullptr));

	return cx_function_id->p_type;
}

cx_type *cx_stdio::close(cx_executor *cx,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	// get node
	cx_symtab_node *p_node = (cx_symtab_node *)cx->top()->basic_types.addr__;
	cx->pop();

	// attempt to close the nodes file stream
//	cx->push((bool)(std::fclose(p_node->p_type->stream.p_file_stream) != EOF));

	return cx_function_id->p_type;
}

/* Causes the output file stream to be synchronized with the actual contents of the file.
 *
 * The behavior is undefined if the given stream is of the input type or if the given stream 
 * is of the update type, but the last I/O operation was not an output operation.
 *
 * If stream is NULL, all open streams are flushed. */
cx_type *cx_stdio::flush(cx_executor *cx,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	// get node
//	FILE *stream = ((cx_symtab_node *)cx->top()->basic_types.addr__)->p_type->stream.p_file_stream;
	cx->pop();

	// attempt to flush the nodes file stream
//	cx->push((bool)(std::fflush(stream) != EOF));

	return cx_function_id->p_type;
}

/* If mode > 0, attempts to make stream wide-oriented. 
 * If mode < 0, attempts to make stream byte-oriented. 
 * If mode==0, only queries the current orientation of the stream. 
 */
cx_type *cx_stdio::wide(cx_executor *cx,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	// int mode
	int mode = cx->top()->basic_types.int__;
	cx->pop();

	// get stream
//	FILE *stream = ((cx_symtab_node *)cx->top()->basic_types.addr__)->p_type->stream.p_file_stream;
	cx->pop();

	// attempt to widen the nodes file stream
	//cx->push((int)std::fwide(stream));

	return cx_function_id->p_type;
}

/*cx_type *cx_stdio::setbuf(cx_executor *cx,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	// char *buffer
	char *buffer = (char *)cx->top()->basic_types.addr__;

	// get node
	cx_symtab_node *p_node = (cx_symtab_node *)cx->top()->basic_types.addr__;
	cx->pop();

	std::setbuf(p_node->p_type->stream.p_file_stream, buffer);

	return cx_function_id->p_type;
}*/

cx_type *cx_stdio::read(cx_executor *cx,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	// int count
	int count = cx->top()->basic_types.int__;
	cx->pop();

	// int size
	int size = cx->top()->basic_types.int__;
	cx->pop();

	// read buffer
	char *buffer = (char *)std::malloc((size * count) + 1);
	memset(buffer, 0, (size * count) + 1);

	// get stream
//	FILE *stream = ((cx_symtab_node *)cx->top()->basic_types.addr__)->p_type->stream.p_file_stream;
	cx->pop();

//	std::size_t s = fread(buffer, size, count, stream);
	cx->push((void *)buffer);

	//cx_type *p_str = new cx_type(fc_array, s * size, nullptr);
	//set_type(p_str->array.p_element_type, p_char_type);
	//p_str->array.element_count = s;
	//p_str->array.max_index = s;

//	return p_str;
	return nullptr;
}