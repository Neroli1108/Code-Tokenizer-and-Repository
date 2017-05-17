///////////////////////////////////////////////////////////////////////
// Utilities.cpp - small, generally usefule, helper classes          //
// ver 1.2                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:		Cunyang Shui, Computer Engineering					 //
//				cushui@syr.edu										 //
// Source:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////

#include <cctype>
#include <iostream>
#include <locale>
#include "Utilities.h"

using namespace Scanner;

// ------< small title >-----
void StringHelper::title(const std::string& src)
{
  std::cout << "\n  " << src;
  std::cout << "\n " << std::string(src.size() + 2, '-');
}
// ------< Big title >-----
void StringHelper::Title(const std::string& src, char underline)
{
  std::cout << "\n  " << src;
  std::cout << "\n " << std::string(src.size() + 2, underline);
}
// ------< empty line >--------
void StringHelper::putline()
{
  std::cout << "\n";
}

std::string StringHelper::trim(const std::string& src)
{
	std::locale loc;
	std::string trimmed = src;
	size_t first = 0;
	while (true)
	{
		if (std::isspace(trimmed[first], loc))
			++first;
		else
			break;
	}
	size_t last = trimmed.size() - 1;
	while (true)
	{
		if (std::isspace(trimmed[last], loc) && last > 0)
			--last;
		else
			break;

	}
	return trimmed.substr(first, last - first + 1);
}
// ------< test stub >--------
#ifdef TEST_UTILITIES

int main()
{
  Title("Testing Utilities Package");
  putline();

  title("test StringHelper::split(std::string)");

  std::string test = "a, \n, bc, de, efg, i, j k lm nopq rst";
  std::cout << "\n  test string = " << test;
  
  std::vector<std::string> result = StringHelper::split(test);
  
  std::cout << "\n";
  for (auto item : result)
  {
    if (item == "\n")
      std::cout << "\n  " << "newline";
    else
      std::cout << "\n  " << item;
  }
  std::cout << "\n\n";
  return 0;
}
#endif
