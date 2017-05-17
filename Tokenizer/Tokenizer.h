#ifndef TOKENIZER_H
#define TOKENIZER_H
///////////////////////////////////////////////////////////////////////
// Tokenizer.h - read words from a std::stream                       //
// ver 4                                                             //
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
 * This package provides a public Toker class and private ConsumeState class.
 * Toker reads words from a std::stream, throws away whitespace returns words 
 * from the stream in the order encountered.  Quoted strings and certain p
 * unctuators and newlines are returned as single tokens.
 *
 *
 * Build Process:
 * --------------
 * Required Files: Tokenizer.h, Tokenizer.cpp, Utilities.h, Utilities.cpp
 * Build Command: devenv Project3.sln /rebuild debug
 *
 * Maintenance History:
 * --------------------
 * ver 4.0 :  8 Apr 2016
 * - change from static method to non-static method
 * ver 3.6 : 14 Mar 2016
 * - add linecount
 * ver 3.5 : 09 Feb 2016
 * - provide function 	
 *		void setSpecialSingleChars(std::string ssc);
 *		void setSpecialCharpairs(std::string scp);
 *   to differeniate special character and set special characters
 * - provide function to take quoted string as token
 * - provide reset function to start multi-toker in one application
 * ver 3.4 : 03 Feb 2016
 * - fixed bug that prevented reading of last character in source by
 *   clearing stream errors at beginning of ConsumeState::nextState()
 * ver 3.3 : 02 Feb 2016
 * - declared ConsumeState copy constructor and assignment operator = delete
 * ver 3.2 : 28 Jan 2016
 * - fixed bug in ConsumeState::nextState() by returning a valid state
 *   pointer if all tests fail due to reaching end of file instead of
 *   throwing logic_error exception.
 * ver 3.1 : 27 Jan 2016
 * - fixed bug in EatCComment::eatChars()
 * - removed redundant statements assigning _pState in derived eatChars() 
 *   functions
 * - removed calls to nextState() in each derived eatChars() and fixed
 *   call to nextState() in ConsumeState::consumeChars()
 * ver 3.0 : 11 Jun 2014
 * - first release of new design
 */
#include <iosfwd>
#include <string>
#include <vector>
#include "../Utilities/Utilities.h"

namespace Scanner
{
  class StringHelper;
  class ConsumeState;
  struct Context;

  class Toker
  {
  public:
    Toker();
    Toker(const Toker&) = delete;
    ~Toker();
    Toker& operator=(const Toker&) = delete;
    bool attach(std::istream* pIn);					//attach file content
    std::string getTok();							//get one token
    bool canRead();									
	void setSpecialSingleChars(std::string ssc);	//set Special Single Chars
	void setSpecialCharpairs(std::string scp);		//set Special Char pairs
	void returnComments(bool rCmm);
	size_t getLine();
  private:
    ConsumeState* pConsumer;
	Context* _pContext;
  };
}

#endif