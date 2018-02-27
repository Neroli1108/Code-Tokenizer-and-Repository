#ifndef UTILITIES_H
#define UTILITIES_H
///////////////////////////////////////////////////////////////////////
// Utilities.h - small, generally usefule, helper classes            //
// ver 1.2                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:		Yunding Li, Computer Engineering					 //
//				yli327@syr.edu										 //
// Source:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
*
* Build Process:
* --------------
* Required Files: Utilities.h, Utilities.cpp
*
* Build Command: devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.2 : 08 Apr 2016
* - modify prologue
* ver 1.1 : 09 Feb 2016
* - delete useless function in Proj 1 (Convert, split)
* ver 1.0 : 05 Feb 2016
* - first release
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <ctime>
#include <iostream>


namespace Scanner
{
  class StringHelper
  {
  public:
    static void Title(const std::string& src, char underline = '-');
    static void title(const std::string& src);
	static void putline();
	static std::string trim(const std::string& src);

  };

  ///////////////////////////////////////////////////////////////////
  // Converter converts template type T to and from string

  template <typename T>
  class Converter
  {
  public:
	  static std::string toString(const T& t);
	  static T toValue(const std::string& src);
  };

  template <typename T>
  std::string Converter<T>::toString(const T& t)
  {
	  std::ostringstream out;
	  out << t;
	  return out.str();
  }

  template<typename T>
  T Converter<T>::toValue(const std::string& src)
  {
	  std::istringstream in(src);
	  T t;
	  in >> t;
	  return t;
  }

}
#endif
