#include <cstring>
#include <cstdio>
#include <cmath>
#include <sstream>
#include <climits>
#include <cfloat>
#include <cctype>
#include "token.h"

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
void cx_number_token::get(cx_text_in_buffer &buffer) {

    float numValue = 0.0; /* value of number ignoring
                           * the decimal point */
    int wholePlaces = 0; // no. digits before the decimal point
    int decimalPlaces = 0; // no. digits after  the decimal point
    char exponentSign = '+';
    float eValue = 0.0; // value of number after 'E'
    int exponent = 0; // final value of exponent
    bool sawDotDotFlag = false; // true if encountered '..',

    // assume base 10 radix
    radix = 10;

    ch = buffer.current_char();
    ps = string;
    digit_count = 0;
    count_error_flag = false;
    code__ = tc_error;
    type__ = ty_integer; /* we don't know what it is yet, but
                       * assume it'll be an integer */

    /* get the whole part of the number by accumulating
     * the values of its digits into numValue.  wholePlaces keeps
     * track of the number of digits in this part. */
    if (!accumulate_value(buffer, numValue, err_invalid_number)) return;
    wholePlaces = digit_count;

    /* If the current character is a dot, then either we have a
     * fraction part or we are seeing the first character of a '..'
     * token.  To find out, we must fetch the next__ character. */
    if (ch == '.') {
        ch = buffer.get_char();

        if (ch == '.') {

            /* We have a .. token.  Back up bufferp so that the
             * token can be extracted next__. */
            sawDotDotFlag = true;
            buffer.put_back_char();
        } else {
            type__ = ty_real;
            *ps++ = '.';

            // We have a fraction part.  Accumulate it into numValue.
            if (!accumulate_value(buffer, numValue,
                    err_invalid_fraction)) return;
            decimalPlaces = digit_count - wholePlaces;
        }
    }

    /* get the exponent part, if any. There cannot be an
     * exponent part if we already saw the '..' token. */
    if (!sawDotDotFlag && ((ch == 'E') || (ch == 'e'))) {
        type__ = ty_real;
        *ps++ = ch;
        ch = buffer.get_char();

        // Fetch the exponent's sign, if any.
        if ((ch == '+') || (ch == '-')) {
            *ps++ = exponentSign = ch;
            ch = buffer.get_char();
        }

        // Accumulate the value of the number after 'E' into eValue.
        digit_count = 0;
        if (!accumulate_value(buffer, eValue,
                err_invalid_exponent)) return;

        if (exponentSign == '-') eValue = -eValue;
    }

    // Were there too many digits?
    if (count_error_flag) {
        cx_error(err_too_many_digits);
        return;
    }

    /* Calculate and check the final exponent value,
     * and then use it to adjust the number's value. */
    exponent = int(eValue) - decimalPlaces;
    if ((exponent + wholePlaces < FLT_MIN_10_EXP) ||
            (exponent + wholePlaces > FLT_MAX_10_EXP)) {
        cx_error(err_real_out_of_range);
        return;
    }
    if (exponent != 0) numValue *= float(pow((double) 10, exponent));

    // Check and set the numeric value.
    if (type__ == ty_integer) {
        if ((numValue < INT_MIN) || (numValue > INT_MAX)) {
            cx_error(err_integer_out_of_range);
            return;
        }
        value__.int__ = int(numValue);
    } else value__.float__ = numValue;

    *ps = '\0';
    code__ = tc_number;
}

/** accumulate_value     Extract a number part from the source
 *                      and set its value.
 * 
 * @param buffer : ptr to text input buffer.
 * @param value  : accumulated value (from one or more calls).
 * @param ec     : error code if failure.
 * @return true  if success false if failure.
 */
int cx_number_token::accumulate_value(cx_text_in_buffer &buffer,
        float &value, cx_error_code ec) {

    const int maxDigitCount = 20;

    // cx_error if the first character is not a digit.
    if (char_code_map[ch] != cc_digit) {
        cx_error(ec);
        return false; // failure
    }

    /* Accumulate the value as long as the total allowable
     * number of digits has not been exceeded. */
    do {

        *ps++ = ch;

        if (++digit_count <= maxDigitCount) {
            value = radix * value + char_value(ch); // shift left and add
        } else count_error_flag = true; // too many digits

        ch = buffer.get_char();

        // hex base
        if (ch == 'x') {
            radix = 16;
            ch = buffer.get_char();
        }

        // binary base
        if (ch == 'b') radix = 2;

    } while ((char_code_map[ch] == cc_digit) || is_x_digit(ch));

    return true; // success
}

int cx_number_token::char_value(const char &c) {
    char xc = tolower(c);
    if (is_x_digit(xc)) {
        switch (xc) {
            case 'a': return 10;
            case 'b': return 11;
            case 'c': return 12;
            case 'd': return 13;
            case 'e': return 14;
            case 'f': return 15;
        }
    }

    return (c - '0');
}

bool cx_number_token::is_x_digit(const char& c) {
    char xc = tolower(c);

    return (xc == 'a') || (xc == 'b') || (xc == 'c') ||
            (xc == 'd') || (xc == 'e') || (xc == 'f') ||
            ((xc >= '0') && (xc <= '9'));
}

/** print       print the token to the list file.
 * 
 */
void cx_number_token::print(void) const {
    if (type__ == ty_integer) {
        sprintf(list.text, "\t%-18s =%d", ">> integer:",
                value__.int__);
    } else {
        sprintf(list.text, "\t%-18s =%g", ">> real:",
                value__.float__);
    }

    list.put_line();
}