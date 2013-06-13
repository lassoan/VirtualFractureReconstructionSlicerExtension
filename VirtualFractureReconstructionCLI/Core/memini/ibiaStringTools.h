

#ifndef IBIASTRINGTOOLS_H
#define IBIASTRINGTOOLS_H


#include <string>
#include <sstream>


namespace ibia 
{


// This module contains some general tool-functions for string and 
// string-related operations not provided by ANSI-C++ or ITK.

/** 
  * Converts a value (from) of type F into another representation of type
  * T using string-streams internally.<br>
  * There are some ANSI-C++ problems concerning template-functions and so I 
  * had to directly implement this function here.
  * @param from the value of type F to be converted
  * @return the value in new representation of type T
  */
template<typename T, typename F> inline T StreamConvert(F from)
{
  std::stringstream temp;
  temp << from;
  T to = T();
  temp >> to;
  
  return to;
}
/** 
  * Converts a value (from) of type F into std::string representation using 
  * string-streams internally.
  * It is a specialized implementation of 
  * @see template<typename T, typename F> inline T StreamConvert(F from).<br>
  * There are some ANSI-C++ problems concerning template-functions and so I 
  * had to directly implement this function here.
  * @param from the value of type F to be converted
  * @return the value in new representation of type std::string
  */
template<typename F> inline std::string StreamConvert(F from)
{
  return StreamConvert<std::string>(from);
}

/**
  * Trims the referenced string parameter (cuts leading and trailing spaces).
  * @param s the string to be trimmedd
  */
void Trim(std::string &s);

/**
  * Trims the referenced string parameter (cuts leading and trailing spaces), 
  * but does not change the parameter itself, it returns the trimmed version
  * of the parameter.
  * @param s the string to be trimmed (but not changed)
  * @return the trimmed version of s
  */
std::string TrimF(std::string s);

/**
  * Replaces all occurences of specified searchStr with the specified 
  * replaceStr directly in specified inputStr.
  * @param inputStr the string to be modified
  * @param searchStr string expression specifying the search-sub-string
  * @param replaceStr string expression specifying the replace-sub-string
  */
void ReplaceString(std::string &inputStr, std::string searchStr,
  std::string replaceStr);

/** 
  * Ensures that the specified string ends with the specified string. It is 
  * appended if it does not already end with the specified string. This may
  * be useful for appending trailing slashes at the end for file pathes
  * in UNIX systems or backslashes in Windows systems.
  * @param s the string-reference to be checked (it is directly manipulated)
  * @param endWith s must end with this string (otherwise it is appended)
  */
void EnsureStringEndsWith(std::string &s, const std::string endWith);
/** 
  * Ensures that the specified string does not end with the specified string. It
  * is cut if it ends with the specified string. This may
  * be useful for cutting trailing slashes at the end of file pathes
  * in UNIX systems or backslashes in Windows systems.
  * @param s the string-reference to be checked (it is directly manipulated)
  * @param notEndWith s must not end with this string (otherwise it is cut)
  */
void EnsureStringNotEndsWith(std::string &s, const std::string notEndWith);


}


#endif
