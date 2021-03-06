/**
 * StringHelper.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _STRINGHELPER_H_
#define _STRINGHELPER_H_

#include <string>
#include <iostream>
#include <cctype>

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

inline bool IsPositiveNumber(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) {
        ++it;
    }
    return (!s.empty() && it == s.end());
}

};
#endif