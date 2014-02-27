/** I/O Buffers
 * buffer.cpp
 *
 * Process text I/O files.  Included are member functions
 * to read the source file and write to the list file.
 */

#ifndef CX_DYNAMIC_API
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <ctime>
#endif

#include "common.h"
#include "buffer.h"

/***********************
 *                     *
 *  Text Input Buffer  *
 *                     *
 ***********************/

// special end-of-file character
const char eof_char = 0x7F;

/* "virtual" position of the current char
 *  In the input buffer (with tabs expanded) */
int input_position;

// true if list source lines, else false
int list_flag = true;

/** Constructor     Construct a input text buffer by opening the
 *                  input file.
 *
 * @param p_input_file_name : ptr to the name of the input file
 * @param ac             : abort code to use if open failed
 */
cx_text_in_buffer::cx_text_in_buffer (const char *p_input_file_name,
                                      cx_abort_code ac)
: p_file_name (new char[strlen (p_input_file_name) + 1]) {
    // Copy the input file name.
    strcpy(p_file_name, p_input_file_name);

    // Open the input file.  Abort if failed.
    file.open(p_file_name, std::ios::in);
    if (!file.good()) {
        std::cout << p_file_name << ": " << std::strerror(errno) << std::endl;
        abort_translation(ac);
    }
}

/** get_char        Fetch and return the next__ character from the
 *                 text buffer.  If at the end of the buffer,
 *                 read the next__ source line.  If at the end of
 *                 the file, return the end-of-file character.
 *
 * @return next__ character from the source file
 *          or the end-of-file character.
 */
char cx_text_in_buffer::get_char (void) {
    const int tab_size = 8; // size of tabs
    char ch; // character to return

    if (*p_char == eof_char) return eof_char; // end of file
    else if (*p_char == '\0') ch = get_line(); // null
    else { // next__ char
        ++p_char;
        ++input_position;
        ch = *p_char;
    }

    // If tab character, increment input_position to the next__
    // multiple of tab_size.
    if (ch == '\t') input_position += tab_size - input_position % tab_size;

    return ch;
}

/** put_back_char     put the current character back into the
 *                  input buffer so that the next__ call to
 *                  get_char will fetch this character. (Only
 *                  called to put back a '.')
 *
 * @return the previous character
 */
char cx_text_in_buffer::put_back_char (void) {
    --p_char;
    --input_position;

    return *p_char;
}

/*******************
 *                 *
 *  Source Buffer  *
 *                 *
 *******************/

/** Constructor     Construct a source buffer by opening the
 *                  source file.  Initialize the list file, and
 *                  read the first line from the source file.
 *
 * @param p_source_file_name : ptr to name of source file
 */
cx_source_buffer::cx_source_buffer (const char *p_source_file_name)
: cx_text_in_buffer (p_source_file_name, abort_source_file_open_failed) {
    // Initialize the list file and read the first source line.
    if (list_flag) list.initialize(p_source_file_name);
    get_line();
}

/** get_line         Read the next__ line from the source file, and
 *                  print it to the list file preceded by the
 *                  line number and the current nesting level.
 *
 * @return first character of the source line, or the
 *          end-of-file character if at the end of the file
 */
char cx_source_buffer::get_line (void) {
    extern int current_nesting_level;

    // If at the end of the source file, return the end-of-file char.
    if (file.eof()) p_char = (char *) &eof_char;

        // Else read the next__ source line and print it to the list file.
    else {
        memset(text, '\0', sizeof (text));


        file.getline(text, max_input_buffer_size);

        p_char = text; // point to first source line char

        // if list_flag == true, list the source to stdout
        if (list_flag) {
            list.put_line(
                          text,
                          ++current_line_number,
                          current_nesting_level
                          );
        }

    }

    input_position = 0;
    return *p_char;
}

/*****************
 *               *
 *  List Buffer  *
 *               *
 *****************/

const int max_printline_length = 80;
//const int max_lines_per_page = 50;

cx_list_buffer list; // the list file buffer

/** print_page_header     Start a new page of the list file and
 *                      print the page header.
 */
//void cx_list_buffer::print_page_header (void) {
//    const char form_feed_char = '\f';
//
//    std::cout << form_feed_char << "Page " << ++page_number
//            << "   " << p_source_file_name << "   " << date
//            << std::endl << std::endl;
//
//    line_count = 0;
//}

/** Initialize      Initialize the list buffer.  Set the date
 *                  for the page header, and print the first
 *                  header.
 *
 * @param p_file_name : ptr to source file name (for page header)
 */
void cx_list_buffer::initialize (const char *p_file_name) {
    memset(text, '\0', sizeof (text));
    //page_number = 0;

    // Copy the input file name.
    p_source_file_name = new char[strlen(p_file_name) + 1];
    strcpy(p_source_file_name, p_file_name);

    // Set the date.
//    time_t timer;
//    time(&timer);
//    strcpy(date, asctime(localtime(&timer)));
//    date[strlen(date) - 1] = '\0'; // remove '\n' at end
//
//    print_page_header();
}


///  put_line         print a line of text to the list file.

void cx_list_buffer::put_line (void) {
    // Start a new page if the current one is full.
    //if (list_flag && (line_count == max_lines_per_page)) print_page_header();

    // Truncate the line if it's too long.
    text[max_printline_length] = '\0';

    // print the text line, and then blank out the text.
    std::cout << text << std::endl;
    memset(text, '\0', sizeof (text));

    ++line_count;
}

cx_text_out_buffer::~cx_text_out_buffer () {

}

