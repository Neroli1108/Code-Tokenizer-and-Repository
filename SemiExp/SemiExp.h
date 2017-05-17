#ifndef SEMIEXPRESSION_H
#define SEMIEXPRESSION_H
///////////////////////////////////////////////////////////////////////
// SemiExp.h - collect tokens for analysis                           //
// ver 3.5                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Cunyang Shui, Computer Engineering   		         //
//				cushui@syr.edu								         //
//  Source:     Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public SemiExp class that collects and makes
* available sequences of tokens.  SemiExp uses the services of a Toker
* class to acquire tokens.  Each call to SemiExp::get() returns a 
* sequence of tokens that ends in 
* 1. {, }, ; 
* 2. '\n' + the line begins with #.
* 3. prublic, private, or protected + :
*
* This is a solution for Project #1.
*
* Build Process:
* --------------
* Required Files: 
*   SemiExp.h, SemiExp.cpp, Tokenizer.h, Tokenizer.cpp, 
*   Utilities.h, Utilities.cpp
* 
* Build Command: devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 3.5 : 08 Apr 2016
* - modify prologe
* ver 3.4 : 14 Mar 2016
* - add linecount function
* ver 3.3 : 09 Feb 2016
* - implement interface from itokcollection.h
* - implement terminate
*		1. {, }, ;
*		2. '\n' + the line begins with #.
*		3. prublic, private, or protected + :
* - ignore ; in for()
* - filter comment from input tokens
* ver 3.2 : 02 Feb 2016
* - declared SemiExp copy constructor and assignment operator = delete
* - added default argument for Toker pointer to nullptr so SemiExp
*   can be used like a container of tokens.
* - if pToker is nullptr then get() will throw logic_error exception
* ver 3.1 : 30 Jan 2016
* - changed namespace to Scanner
* - fixed bug in termination due to continually trying to read
*   past end of stream if last tokens didn't have a semiExp termination
*   character
* ver 3.0 : 29 Jan 2016
* - built in help session, Friday afternoon
*
*/

#include <vector>
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/itokcollection.h"
#include "../Utilities/Utilities.h"

namespace Scanner
{
  using Token = std::string;
  class StringHelper;

  class SemiExp : public ITokCollection
  {
  public:
    SemiExp(Toker* pToker = nullptr);
    SemiExp(const SemiExp&) = delete;

	bool isComment(const std::string& tok);	//this token is comment?
	void getWithFilter(std::string &t);		//filte C and Cpp comment from tokens 

	bool get(bool clear = true);			//get tokens until terminate
	size_t length();						//current token length 
	std::string& operator[](size_t n);
	size_t find(const std::string& tok);	//find string in current token
	void push_back(const std::string& tok); //add string in current token
	bool remove(const std::string& tok);	//remove specified string in current token
	bool remove(size_t i);					//remove specified position string in current token
	void toLower();							//change capital to lower in token
	void trimFront();						//cur front trim in roken
	void clear();							//clear current token
	std::string show(bool showNewLines = false);
	size_t getBeginLine() { return _beginLine; }
	size_t getEndLine() { return _endLine; }

  private:
    std::vector<Token> _tokens;
    Toker* _pToker;
	std::string _tempToken;
	bool _useTemp = false;
	bool _findPound = false;
	int _semicCounter = 0;
	int _brCounter = 0;
	size_t _beginLine = 0;
	size_t _endLine = 0;
	void detectFor(std::string token);
	void pushBackWithLine(std::string tok);
  };
}
#endif
