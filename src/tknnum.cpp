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

/** Get         Extract a number token from the source and set
 *              its value.
 * 
 * @param buffer : ptr to text input buffer.
 */
void TNumberToken::Get(TTextInBuffer &buffer) {
    //const int maxInteger = ;
    //const int maxExponent = 37;

    float numValue = 0.0; /* value of number ignoring
                           * the decimal point */
    int wholePlaces = 0; // no. digits before the decimal point
    int decimalPlaces = 0; // no. digits after  the decimal point
    char exponentSign = '+';
    float eValue = 0.0; // value of number after 'E'
    int exponent = 0; // final value of exponent
    bool sawDotDotFlag = false; // true if encountered '..',
    
    // assume base 10 radix
    uint8_t radix = 10;

    ch = buffer.Char();
    ps = string;
    digitCount = 0;
    countErrorFlag = false;
    code = tcError; 
    type = tyInteger; /* we don't know what it is yet, but
                       * assume it'll be an integer */

    //--Get the whole part of the number by accumulating
    //--the values of its digits into numValue.  wholePlaces keeps
    //--track of the number of digits in this part.
    if (!AccumulateValue(buffer, numValue, radix, errInvalidNumber)) return;
    wholePlaces = digitCount;

    //--If the current character is a dot, then either we have a
    //--fraction part or we are seeing the first character of a '..'
    //--token.  To find out, we must fetch the next character.
    if (ch == '.') {
        ch = buffer.GetChar();

        if (ch == '.') {

            //--We have a .. token.  Back up bufferp so that the
            //--token can be extracted next.
            sawDotDotFlag = true;
            buffer.PutBackChar();
        } else {
            type = tyReal;
            *ps++ = '.';

            //--We have a fraction part.  Accumulate it into numValue.
            if (!AccumulateValue(buffer, numValue, radix, 
                    errInvalidFraction)) return;
            decimalPlaces = digitCount - wholePlaces;
        }
    }

    //--Get the exponent part, if any. There cannot be an
    //--exponent part if we already saw the '..' token.
    if (!sawDotDotFlag && ((ch == 'E') || (ch == 'e'))) {
        type = tyReal;
        *ps++ = ch;
        ch = buffer.GetChar();

        //--Fetch the exponent's sign, if any.
        if ((ch == '+') || (ch == '-')) {
            *ps++ = exponentSign = ch;
            ch = buffer.GetChar();
        }

        //--Accumulate the value of the number after 'E' into eValue.
        digitCount = 0;
        if (!AccumulateValue(buffer, eValue, radix,
                errInvalidExponent)) return;
        
        if (exponentSign == '-') eValue = -eValue;
    }

    //--Were there too many digits?
    if (countErrorFlag) {
        Error(errTooManyDigits);
        return;
    }

    //--Calculate and check the final exponent value,
    //--and then use it to adjust the number's value.
    exponent = int(eValue) - decimalPlaces;
    if ((exponent + wholePlaces < FLT_MIN_10_EXP) ||
            (exponent + wholePlaces > FLT_MAX_10_EXP)) {
        Error(errRealOutOfRange);
        return;
    }
    if (exponent != 0) numValue *= float(pow((double) 10, exponent));

    //--Check and set the numeric value.
    if (type == tyInteger) {
        if ((numValue < INT_MIN) || (numValue > INT_MAX)) {
            Error(errIntegerOutOfRange);
            return;
        }
        value.__int = int(numValue);
    } else value.__float = numValue;

    *ps = '\0';
    code = tcNumber;
}

/** AccumulateValue     Extract a number part from the source
 *                      and set its value.
 * 
 * @param buffer : ptr to text input buffer.
 * @param value  : accumulated value (from one or more calls).
 * @param ec     : error code if failure.
 * @return true  if success false if failure.
 */
int TNumberToken::AccumulateValue(TTextInBuffer &buffer,
        float &value, uint8_t &radix, TErrorCode ec) {
    
    const int maxDigitCount = 20;

    //--Error if the first character is not a digit.
    if (charCodeMap[ch] != ccDigit) {
        Error(ec);
        return false; // failure
    }

    //--Accumulate the value as long as the total allowable
    //--number of digits has not been exceeded.
    do {
        
        *ps++ = ch;

        if (++digitCount <= maxDigitCount) {
            value = radix * value + CharValue(ch); // shift left and add
        } else countErrorFlag = true; // too many digits

        ch = buffer.GetChar();
                
        // hex base
        if(ch == 'x') {
            radix = 16;
            ch = buffer.GetChar();
        }
        
        // binary base
        if(ch == 'b') radix = 2;
        
    } while ((charCodeMap[ch] == ccDigit) || IsXDigit(ch));

    return true; // success
}

int TNumberToken::CharValue(const char &c){
    char xc = tolower(c);
    if(IsXDigit(xc)){
        switch(xc){
            case 'a': return 10;
            case 'b' : return 11;
            case 'c' : return 12;
            case 'd' : return 13;
            case 'e' : return 14;
            case 'f' : return 15;
        }
    }
    
    return (c - '0');
}

bool TNumberToken::IsXDigit(const char& c){
    char xc = tolower(c);
    
    return (xc == 'a') || (xc == 'b') || (xc == 'c') || 
           (xc == 'd') || (xc == 'e') || (xc == 'f') ||
           ((xc >= '0') && (xc <= '9'));
}

/** Print       Print the token to the list file.
 * 
 */
void TNumberToken::Print(void) const {
    if (type == tyInteger) {
        sprintf(list.text, "\t%-18s =%d", ">> integer:",
                value.__int);
    } else {
        sprintf(list.text, "\t%-18s =%g", ">> real:",
                value.__float);
    }

    list.PutLine();
}