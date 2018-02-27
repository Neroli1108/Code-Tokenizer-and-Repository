///////////////////////////////////////////////////////////////////////
// SemiExp.cpp - collect tokens for analysis                         //
// ver 3.5                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Yunding Li, Computer Engineering   		             //
//				yli327@syr.edu								         //
//  Source:     Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <exception>
#include <algorithm>
#include "SemiExp.h"
#include "../Tokenizer/Tokenizer.h"

using namespace Scanner;

SemiExp::SemiExp(Toker* pToker) : _pToker(pToker) {}
//----< filte C and Cpp comment from tokens  >----------------------------
void SemiExp::getWithFilter(std::string &t) {
	do {
	t = _pToker->getTok();
	} while (isComment(t));
}

//----< detect for() loop in tokens  >----------------------------
void SemiExp::detectFor(std::string token) {
	if (token == "for")  
		_semicCounter = 2;

	if (token == "(")
		if (_semicCounter)
			_brCounter++;

	if (token == ")")
		if (_semicCounter) {
			_brCounter--;
			if (!_brCounter)
				_semicCounter = 0;
		}
}
//set begin line and end line
void SemiExp::pushBackWithLine(std::string tok) {
	size_t tokenSize = _tokens.size();
	if (tok != "\n")
	{
		if (!_tokens.size())	
			_beginLine = _pToker->getLine();
		else
			for (int i = 0; i < tokenSize; i++){
				if (_tokens[i] != "\n")
					break;
				if (i == tokenSize-1)
					_beginLine = _pToker->getLine();
			}

		_endLine = _pToker->getLine();
	}
	_tokens.push_back(tok);
}

//----< get tokens until terminate  >----------------------------
bool SemiExp::get(bool clear){
  if (_pToker == nullptr) throw(std::logic_error("no Toker reference"));
  if (clear) SemiExp::clear();
  while (true) {
	  std::string token;
	  if (_useTemp) {
		  token = _tempToken;
		  _useTemp = false;
	  } else
		  SemiExp::getWithFilter(token);
	  if (token == "") break;
	  pushBackWithLine(token);
	  if (token == "{" || token == "}")	return true;	// terminate with "{", "}"
	  SemiExp::detectFor(token);
	  if (token == ";")						// terminate with ";"
		  if (_semicCounter != 0)
			  if (_brCounter) _semicCounter--;
			  else {
				  _semicCounter = 0;
				  return true;
				}
		else return true;
	if (token == "\n")	{					// terminate with "\n" + "#"
		SemiExp::getWithFilter(_tempToken);
		_useTemp = true;
		if (_findPound) {
			_findPound = false;
			return true;
		}
		if (_tempToken == "#"){
			_findPound = true;
			for (size_t i = 0; i < _tokens.size(); i++)
				if (_tokens[i] == "\n")
					SemiExp::clear();
				else
					return true;
		}
	}

	if (token == "public" || token == "protected" || token == "private") {		// terminate with "p..." + ":"
		SemiExp::getWithFilter(_tempToken);
		if (_tempToken == ":"){
			pushBackWithLine(_tempToken);
			return true;
		}
	}
  }
  return false;
}

//----< assignment operator [] >----------------------------
std::string& SemiExp::operator[](size_t n)
{
	if (n < 0 || n >= _tokens.size())
		throw(std::invalid_argument("index out of range"));
	return _tokens[n];
}
//----< current token length  >----------------------------
size_t SemiExp::length()
{
  return _tokens.size();
}
//----< diplay semi expression >----------------------------
std::string SemiExp::show(bool showNewLines)
{
	std::string str = "";
  std::cout << "  ";
  for (auto token : _tokens)
	  if (token != "\n"){
		  str = token + " ";
		  std::cout << token << " ";
	  }
  return str;
}
//----< find string in current token >----------------------------
size_t SemiExp::find(const std::string& tok)
{
	for (size_t i = 0; i < _tokens.size(); i++)
	{
		if (_tokens[i] == tok)
			return i;
	}
	return _tokens.size();
}
//----< add string in current token >----------------------------
void SemiExp::push_back(const std::string& tok)
{
	_tokens.push_back(tok);
}
//----< remove specified string in current token >----------------------------
bool SemiExp::remove(const std::string& tok)
{
	bool result = false;
	for (size_t i = 0; i < _tokens.size(); i++)
	{
		if (_tokens[i] == tok)
		{
			_tokens.erase(_tokens.begin()+i);
			result = true;
		}
	}
	return result;
}
//----< remove specified position string in current token >----------------------------
bool SemiExp::remove(size_t i){
	if (i < _tokens.size())
	{
		_tokens.erase(_tokens.begin() + i);
		return true;
	}
	return false;
}
//----< change capital to lower in token >---------------------
void SemiExp::toLower(){
	for (size_t i = 0; i < _tokens.size(); i++)
	{
		transform(_tokens[i].begin(), _tokens[i].end(), _tokens[i].begin(), ::tolower);
	}
}
//----< cur front trim in roken >---------------------
void SemiExp::trimFront(){
	if (!_tokens.size())
		return;
	if ( _tokens[0].empty() )
		return;
	_tokens[0].erase(0, _tokens[0].find_first_not_of(" "));
}
//----< clear current token >---------------------
void SemiExp::clear(){
	_tokens.clear();
}
//----< this token is comment? >---------------------
bool SemiExp::isComment(const std::string& tok){
	std::size_t found;
	found = tok.find("//");
	if (found != std::string::npos && found == 0)
		return true;

	found = tok.find("/*");
	if (found != std::string::npos && found == 0)
		return true;
	return false;
}
//----< test stub >--------------------------------------------------
#ifdef TEST_SEMIEXP
int main()
{
	StringHelper *ut = new StringHelper();
	ut->Title("SemiExp test stub", '=');
	ut->title("test1");

  Toker toker;
  std::string fileSpec = "../test.txt";
  std::fstream in(fileSpec);
  if (!in.good())
  {
    std::cout << "\n  can't open file " << fileSpec << "\n\n";
    return 1;
  }
  toker.attach(&in);

  SemiExp semi(&toker);
  while(semi.get())
  {
    std::cout << "\n  -- semiExpression --";
    semi.show();
  }

  if (semi.length() > 0)
  {
    std::cout << "\n  -- semiExpression --";
    semi.show();
    std::cout << "\n\n";
  }

  std::cout << "\n\n";
  ut->title("test2");

  toker.resetStarter();

  return 0;
}
#endif