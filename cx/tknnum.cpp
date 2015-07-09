#include <cstring>
#include <cstdio>
#include <cmath>
#include <sstream>
#include <climits>
#include <cfloat>
#include <cctype>
#include "token.h"

namespace cx{

	/// radix number bases

	enum cx_radix_base {
		rb_DECIMAL = 10,
		rb_BINARY = 2,
		rb_HEXADECIMAL = 16,
		rb_OCTAL = 8
	};

	/*******************
	 *                 *
	 *  Number Tokens  *
	 *                 *
	 *******************/

	/** get         Extract a number token from the source and set
	 *              its value.
	 *
	 * @param buffer : ptr to text input buffer.
	 */
	void number_token::get(text_in_buffer &buffer) {

		float number_value = 0.0; /* value of number ignoring
							   * the decimal point */
		int whole_places = 0; // no. digits before the decimal point
		int decimal_places = 0; // no. digits after  the decimal point
		char exponent_sign = '+';
		float e_value = 0.0; // value of number after 'E'
		int exponent = 0; // final value of exponent
		bool saw_dot_dot_Flag = false; // true if encountered '..',

		ch = buffer.current_char();
		ps = string;
		digit_count = 0;
		count_error_flag = false;
		code__ = TC_ERROR;
		type__ = T_INT; /* we don't know what it is yet, but
						   * assume it'll be an integer */

		// assume base 10 radix
		radix = rb_DECIMAL;

		// octal base
		if (ch == '0') {
			radix = rb_OCTAL;
			ch = buffer.get_char();
			switch (ch) {
			case 'x':
				radix = rb_HEXADECIMAL;
				ch = buffer.get_char();
				break;
			case 'b':
				radix = rb_BINARY;
				ch = buffer.get_char();
				break;
			case '.':
				radix = rb_DECIMAL;
				ch = buffer.put_back_char();
				break;
			default:
				ch = buffer.put_back_char();
				break;
			}
		}

		/* get the whole part of the number by accumulating
		 * the values of its digits into number_value.  whole_places keeps
		 * track of the number of digits in this part. */
		if (!accumulate_value(buffer, number_value, ERR_INVALID_NUMBER)) return;
		whole_places = digit_count;

		/* If the current character is a dot, then either we have a
		 * fraction part or we are seeing the first character of a '..'
		 * token.  To find out, we must fetch the next__ character. */
		if (ch == '.') {
			ch = buffer.get_char();

			if (ch == '.') {

				/* We have a .. token.  Back up bufferp so that the
				 * token can be extracted next__. */
				saw_dot_dot_Flag = true;
				buffer.put_back_char();
			}
			else {
				type__ = T_FLOAT;
				*ps++ = '.';

				// We have a fraction part.  Accumulate it into number_value.
				if (!accumulate_value(buffer, number_value,
					ERR_INVALID_FRACTION)) return;
				decimal_places = digit_count - whole_places;
			}
		}

		/* get the exponent part, if any. There cannot be an
		 * exponent part if we already saw the '..' token. */
		if (!saw_dot_dot_Flag && ((ch == 'E') || (ch == 'e'))) {
			type__ = T_FLOAT;
			*ps++ = ch;
			ch = buffer.get_char();

			// Fetch the exponent's sign, if any.
			if ((ch == '+') || (ch == '-')) {
				*ps++ = exponent_sign = ch;
				ch = buffer.get_char();
			}

			// Accumulate the value of the number after 'E' into e_value.
			digit_count = 0;
			if (!accumulate_value(buffer, e_value,
				ERR_INVALID_EXPONENT)) return;

			if (exponent_sign == '-') e_value = -e_value;
		}

		// Were there too many digits?
		if (count_error_flag) {
			cx_error(ERR_TOO_MANY_DIGITS);
			return;
		}

		/* Calculate and check the final exponent value,
		 * and then use it to adjust the number's value. */
		exponent = int(e_value) - decimal_places;
		if ((exponent + whole_places < FLT_MIN_10_EXP) ||
			(exponent + whole_places > FLT_MAX_10_EXP)) {
			cx_error(ERR_REAL_OUT_OF_RANGE);
			return;
		}
		if (exponent != 0) number_value *= float(pow((double)10, exponent));

		// Check and set the numeric value.
		if (type__ == T_INT) {
			if ((number_value < INT_MIN) || (number_value > INT_MAX)) {
				cx_error(ERR_INTEGER_OUT_OF_RANGE);
				return;
			}
			value__.i_ = int(number_value);
		}
		else value__.f_ = number_value;

		*ps = '\0';
		code__ = TC_NUMBER;
	}

	/** accumulate_value     Extract a number part from the source
	 *                      and set its value.
	 *
	 * @param buffer : ptr to text input buffer.
	 * @param value  : accumulated value (from one or more calls).
	 * @param ec     : error code if failure.
	 * @return true  if success false if failure.
	 */
	int number_token::accumulate_value(text_in_buffer &buffer,
		float &value, error_code ec) {

		const int max_digit_count = 20;

		/* cx_error if the first character is not a digit
		 * and radix base is not hex */
		if ((char_map[ch] != CC_DIGIT) && (!isxdigit((unsigned)ch))) {
			cx_error(ec);
			return false; // failure
		}

		/* Accumulate the value as long as the total allowable
		 * number of digits has not been exceeded. */
		do {

			*ps++ = ch;

			if (++digit_count <= max_digit_count) {
				value = radix * value + char_value(ch); // shift left__ and add
			}
			else count_error_flag = true; // too many digits

			ch = buffer.get_char();

		} while ((char_map[ch] == CC_DIGIT) || isxdigit((unsigned)ch));

		return true; // success
	}

	int number_token::char_value(const unsigned char &c) {
		if (isxdigit(c)) {
			switch (c) {
			case 'A':
			case 'a': return 10;
			case 'B':
			case 'b': return 11;
			case 'C':
			case 'c': return 12;
			case 'D':
			case 'd': return 13;
			case 'E':
			case 'e': return 14;
			case 'F':
			case 'f': return 15;
			}
		}

		return (c - '0');
	}

	/** C already has this function
	bool cx_number_token::is_x_digit(const char& c) {
	return (c == 'a') || (c == 'A') || (c == 'b') || (c == 'B') ||
	(c == 'c') || (c == 'C') || (c == 'd') || (c == 'D') ||
	(c == 'e') || (c == 'E') || (c == 'f') || (c == 'F') ||
	((c >= '0') && (c <= '9'));
	}*/

	/** print       print the token to the list file.
	 *
	 */
	void number_token::print(void) const {
		if (type__ == T_INT) {
			sprintf(list.text, "\t%-18s =%d", ">> integer:",
				value__.i_);
		}
		else {
			sprintf(list.text, "\t%-18s =%g", ">> real:",
				value__.f_);
		}

		list.put_line();
	}
}