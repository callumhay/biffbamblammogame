/**
 * StringHelper.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef _STRINGHELPER_H_
#define _STRINGHELPER_H_

#include <string>
#include <iostream>

#define WHITESPACE_CHARACTERS " \t\r\n"

namespace stringhelper {

static inline std::string trimRight(const std::string & s) {
	std::string d(s);
  std::string::size_type i = d.find_last_not_of(WHITESPACE_CHARACTERS);
	if (i == std::string::npos) {
      return "";
	}
	else {
      return d.erase(d.find_last_not_of(WHITESPACE_CHARACTERS) + 1) ;
	}
}  

static inline std::string trimLeft(const std::string & s) {
    std::string d(s);
    return d.erase(0, s.find_first_not_of(WHITESPACE_CHARACTERS)) ;
}  

static inline std::string trim(const std::string & s) {
    std::string d(s);
    return trimLeft(trimRight(d)) ;
} 

};
#endif