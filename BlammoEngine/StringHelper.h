/**
 * StringHelper.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef _STRINGHELPER_H_
#define _STRINGHELPER_H_

#include <string>
#include <iostream>

#define WHITESPACE_CHARACTERS " \t\r\n"

namespace stringhelper {

inline std::string trimRight(const std::string & s) {
	std::string d(s);
  std::string::size_type i = d.find_last_not_of(WHITESPACE_CHARACTERS);
	if (i == std::string::npos) {
      return "";
	}
	else {
      return d.erase(d.find_last_not_of(WHITESPACE_CHARACTERS) + 1) ;
	}
}  

inline std::string trimLeft(const std::string & s) {
    std::string d(s);
    return d.erase(0, s.find_first_not_of(WHITESPACE_CHARACTERS)) ;
}  

inline std::string trim(const std::string & s) {
    std::string d(s);
    return trimLeft(trimRight(d)) ;
} 

inline void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters) {
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

/**
 * Adds commas to a basic number stored in the given string.
 * e.g., given 243556 will return 243,556
 */
inline std::string AddNumberCommas(long number) {
    std::string scoreStr;
    long currNumber = number;

    // Add comma separators for every grouping of 3 digits...
    for (;;) {
        std::stringstream scorePartStrStream;
        int tempThreeDigitValue = (currNumber % 1000);
        bool isMoreNumber = currNumber >= 1000;

        // There is no preservation of the zeros when we mod - we want
        // a zero value to show up as three zeros! Or a tens value to show
        // up as a zero followed by that value. 
        // NOTE: We only do this if we're stuck in the middle of breaking apart the number
        if (isMoreNumber) {
            if (tempThreeDigitValue < 100) {
                scorePartStrStream << "0";
                if (tempThreeDigitValue < 10) {
                    scorePartStrStream << "0";
                }
            }
        }
            
        scorePartStrStream << tempThreeDigitValue;
        scoreStr = scorePartStrStream.str() + scoreStr;

        if (!isMoreNumber) {
            break;
        }
        else {
            currNumber /= 1000;
            scoreStr = std::string(",") + scoreStr;
        }
    }
    return scoreStr;
}

};
#endif