#include <sstream>
#include "types.h"
#include "symtable.h"
#include <cstdio>
#include <iostream>

//cx_type *cx_stdio::puts(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// first param as string
//	const char *_string = (const char *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// get output stream node from top of stack
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	
//	bool ret_val = (p_snode->defn.io.stream != nullptr);
//
//	if (ret_val){
//		ret_val = (bool)(std::fputs(_string, p_snode->defn.io.stream) != EOF);
//	}
//
//	cx->pop();
//
//	cx->push(ret_val);
//
//	return cx_function_id->p_type;
//}
//
///* Opens a file indicated by filename and returns a file stream associated 
// * with that file. mode is used to determine the file access mode. */
//cx_type *cx_stdio::open(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// char *mode
//	const char *mode = (const char *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// char *filename
//	const char *filename = (const char *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// open stream
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	
//	p_snode->defn.io.stream = std::fopen(filename, mode);
//
//	cx->pop();
//	cx->push((bool)(p_snode->defn.io.stream != nullptr));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::reopen(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// char *mode
//	const char *mode = (const char *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// char *filename
//	const char *filename = (const char *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// open stream
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//
//	p_snode->defn.io.stream = std::freopen(filename, mode, p_snode->defn.io.stream);
//
//	cx->pop();
//
//	cx->push((bool)(p_snode->defn.io.stream != nullptr));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::close(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// attempt to close the nodes file stream
//	cx->push((bool)(std::fclose(p_snode->defn.io.stream) != EOF));
//
//	return cx_function_id->p_type;
//}
//
///* Causes the output file stream to be synchronized with the actual contents of the file.
// *
// * The behavior is undefined if the given stream is of the input type or if the given stream 
// * is of the update type, but the last I/O operation was not an output operation.
// *
// * If stream is NULL, all open streams are flushed. */
//cx_type *cx_stdio::flush(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// attempt to close the nodes file stream
//	cx->push((bool)(std::fflush(p_snode->defn.io.stream) != EOF));
//
//	return cx_function_id->p_type;
//}
//
///* If mode > 0, attempts to make stream wide-oriented. 
// * If mode < 0, attempts to make stream byte-oriented. 
// * If mode==0, only queries the current orientation of the stream. 
// */
//cx_type *cx_stdio::wide(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// int mode
//	int mode = cx->top()->basic_types.int__;
//	cx->pop();
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// attempt to widen the nodes file stream
//	cx->push((int)std::fwide(p_snode->defn.io.stream, mode));
//
//	return cx_function_id->p_type;
//}
//
///*cx_type *cx_stdio::setbuf(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// char *buffer
//	char *buffer = (char *)cx->top()->basic_types.addr__;
//
//	// get node
//	cx_symtab_node *p_node = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	std::setbuf(p_node->p_type->stream.p_file_stream, buffer);
//
//	return cx_function_id->p_type;
//}*/
//
//cx_type *cx_stdio::read(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// int count
//	int count = cx->top()->basic_types.int__;
//	cx->pop();
//
//	// int size
//	int size = cx->top()->basic_types.int__;
//	cx->pop();
//
//	// read buffer
//	char *buffer = (char *)std::malloc((size * count) + 1);
//	memset(buffer, 0, (size * count) + 1);
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	std::size_t s = std::fread(buffer, size, count, p_snode->defn.io.stream);
//	cx->push((void *)buffer);
//
//	cx_type *p_str = new cx_type(fc_array, s * size, nullptr);
//	set_type(p_str->array.p_element_type, p_char_type);
//	p_str->array.element_count = s;
//	p_str->array.max_index = s;
//
//	return p_str;
//}
//
//cx_type *cx_stdio::write(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// int count
//	int count = cx->top()->basic_types.int__;
//	cx->pop();
//
//	// int size
//	int size = cx->top()->basic_types.int__;
//	cx->pop();
//
//	// byte buffer
//	const uint8_t *buffer = (const uint8_t *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	cx->push((int)std::fwrite(buffer, size, count, p_snode->defn.io.stream));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::getc(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	cx_stack_item *rv = new cx_stack_item;
//	rv->basic_types.char__ = std::getc(p_snode->defn.io.stream);
//	cx->push((void *)rv);
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::gets(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// int count
//	int count = cx->top()->basic_types.int__;
//	cx->pop();
//
//	// read buffer
//	char *buffer = (char *)std::malloc(count + 1);
//	memset(buffer, 0, count + 1);
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	std::fgets(buffer, count + 1, p_snode->defn.io.stream);
//	cx->push((void *)buffer);
//
//	cx_type *p_str = new cx_type(fc_array, count, nullptr);
//	set_type(p_str->array.p_element_type, p_char_type);
//	p_str->array.element_count = count;
//	p_str->array.max_index = count;
//
//	return p_str;
//}
//
//cx_type *cx_stdio::putc(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// int ch
//	int ch = cx->top()->basic_types.int__;
//	cx->pop();
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	cx->push((int)std::putc(ch, p_snode->defn.io.stream));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::ungetc(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// int ch
//	int ch = cx->top()->basic_types.int__;
//	cx->pop();
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	cx->push((int)std::ungetc(ch, p_snode->defn.io.stream));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::ungetwc_(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// int ch
//	wchar_t ch = cx->top()->basic_types.wchar__;
//	cx->pop();
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	cx->push((wchar_t)std::ungetwc(ch, p_snode->defn.io.stream));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::getwc_(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	cx->push((wchar_t)std::fgetwc(p_snode->defn.io.stream));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::getws_(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// int count
//	int count = cx->top()->basic_types.int__;
//	cx->pop();
//
//	int size = (sizeof(wchar_t) * count) + 1;
//
//	// read buffer
//	wchar_t *buffer = (wchar_t *)std::malloc(size);
//	memset(buffer, 0, size);
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	std::fgetws(buffer, count + 1, p_snode->defn.io.stream);
//	cx->push((void *)buffer);
//
//	cx_type *p_wstr = new cx_type(fc_array, count, nullptr);
//	set_type(p_wstr->array.p_element_type, p_wchar_type);
//	p_wstr->array.element_count = count;
//	p_wstr->array.max_index = count;
//
//	return p_wstr;
//}
//
//cx_type *cx_stdio::putwc_(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// int ch
//	wchar_t ch = cx->top()->basic_types.wchar__;
//	cx->pop();
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	cx->push((wchar_t)std::putwc(ch, p_snode->defn.io.stream));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::putws_(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// first param as string
//	const wchar_t *_wstring = (wchar_t *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// get output stream node from top of stack
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//
//	bool ret_val = (p_snode->defn.io.stream != nullptr);
//
//	if (ret_val){
//		ret_val = (bool)(std::fputws(_wstring, p_snode->defn.io.stream) != EOF);
//	}
//
//	cx->pop();
//
//	cx->push(ret_val);
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::tell(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// attempt to close the nodes file stream
//	cx->push((int)std::ftell(p_snode->defn.io.stream));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::seek(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	int origin = cx->top()->basic_types.int__;
//	cx->pop();
//
//	int offset = cx->top()->basic_types.int__;
//	cx->pop();
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// attempt to seek
//	cx->push((bool)(std::fseek(p_snode->defn.io.stream, offset, origin) == 0));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::rewind(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// attempt to rewind
//	std::rewind(p_snode->defn.io.stream);
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::clearerr(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// attempt to clear errors
//	std::clearerr(p_snode->defn.io.stream);
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::eof(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// push bool EOF state
//	cx->push((bool)(std::feof(p_snode->defn.io.stream) != 0));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::error(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	// push bool error state
//	cx->push((bool)(std::ferror(p_snode->defn.io.stream) != 0));
//
//	return cx_function_id->p_type;
//}
//
//cx_type *cx_stdio::tmpfile(cx_executor *cx,
//	cx_symtab_node *cx_function_id,
//	const cx_type *p_type){
//
//	// get node
//	cx_symtab_node *p_snode = (cx_symtab_node *)cx->top()->basic_types.addr__;
//	cx->pop();
//
//	p_snode->defn.io.stream = std::tmpfile();
//
//	// push bool state
//	cx->push((bool)(p_snode->defn.io.stream != nullptr));
//
//	return cx_function_id->p_type;
//}