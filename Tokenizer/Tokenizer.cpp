///////////////////////////////////////////////////////////////////////
// Tokenizer.cpp - read words from a std::stream                     //
// ver 4                                                             //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Cunyang Shui, Computer Engineering   		         //
//				cushui@syr.edu								         //
//  Source:     Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////

#include "Tokenizer.h"
#include <iostream>
#include <cctype>
#include <string>

namespace Scanner
{

	struct Context
	{
		Context();
		~Context();
		std::string token;
		std::istream* _pIn;
		std::vector<std::string> sVector = { "<", ">", "[", "]", "(", ")", "{", "}", ":", "=", "+", "-", "*", "\n", ";" };
		std::vector<std::string> pVector = { "<<", ">>", "::", "++", "--", "==", "+=", "-=", "*=", "/=", "%=", "ˆ=", "&=", "\\=", "!=", ">=", "<=", "&&", "||" };
		int prevChar;
		int currChar;
		bool returnComm;
		size_t lineCount;
		ConsumeState* _pState;
		ConsumeState* _pEatCppComment;
		ConsumeState* _pEatCComment;
		ConsumeState* _pEatWhitespace;
		ConsumeState* _pEatPunctuator;
		ConsumeState* _pEatAlphanum;

		ConsumeState* _pEatQuoter;
		ConsumeState* _pEatNewline;
	};

	class ConsumeState
	{
		friend class Toker;
	public:
		ConsumeState();
		ConsumeState(const ConsumeState&) = delete;
		ConsumeState& operator=(const ConsumeState&) = delete;
		virtual ~ConsumeState();
		void attach(std::istream* pIn) { _pContext->_pIn = pIn; }
		virtual void eatChars() = 0;
		void consumeChars() {
			_pContext->_pState->eatChars();
			_pContext->_pState = nextState();
		}
		bool canRead() { return _pContext->_pIn->good(); }
		std::string getTok() {
			if (_pContext->token == "\n")
				_pContext->lineCount++;
			return _pContext->token;
		}
		bool hasTok() { return _pContext->token.size() > 0; }
		ConsumeState* nextState();
		void addSingle(std::string ssc); // add single spectial char
		void addPair(std::string scp);   // add special char pair
		void setReturnComm(bool rm);
		size_t getLineCount() { return _pContext->lineCount; }
		void setContext(Context* pContext);

	protected:
		Context* _pContext;
		static bool firstStart;
		static bool firstEnd;
	};
}

using namespace Scanner;
void testLog(const std::string& msg);

//----< used by Toker to provide Consumer with Context ptr >---------
void ConsumeState::setContext(Context* pContext)
{
	_pContext = pContext;
}

//----< add special single character >----------------------------
void ConsumeState::addSingle(std::string str) {

	_pContext->sVector.push_back(str);
}
//----< add special character pair  >----------------------------
void ConsumeState::addPair(std::string str) {
	_pContext->pVector.push_back(str);
}
void ConsumeState::setReturnComm(bool rm) {
	_pContext->returnComm = rm;
}

//----< get next char in file  >----------------------------
ConsumeState* ConsumeState::nextState()
{
	if (!(_pContext->_pIn->good()))
		return nullptr;
	int chNext = _pContext->_pIn->peek();
	if (chNext == EOF)	// if peek() reads end of file character, EOF, then eofbit is set and _pIn->good() will return false.	
		_pContext->_pIn->clear();		//clear() restores state to good

	if (std::isspace(_pContext->currChar) && _pContext->currChar != '\n') {
		testLog("state: eatWhitespace");
		return _pContext->_pEatWhitespace;
	}
	if (_pContext->currChar == '/' && chNext == '/') {
		testLog("state: eatCppComment");
		return _pContext->_pEatCppComment;
	}
	if (_pContext->currChar == '/' && chNext == '*') {
		testLog("state: eatCComment");
		return _pContext->_pEatCComment;
	}
	if (_pContext->currChar == '\n') {
		testLog("state: eatNewLine");
		return _pContext->_pEatNewline;
	}
	if (std::isalnum(_pContext->currChar) || _pContext->currChar == '_') {
		testLog("state: eatAlphanum");
		return _pContext->_pEatAlphanum;
	}
	if (_pContext->currChar == '"') {
		testLog("state: eatQuoter");
		return _pContext->_pEatQuoter;
	}
	if (ispunct(_pContext->currChar) && _pContext->currChar != '_') {
		testLog("state: eatPunctuator");
		return _pContext->_pEatPunctuator;
	}
	if (!_pContext->_pIn->good()) {
		//std::cout << "\n  end of stream with currChar = " << currChar << "\n\n";
		return _pContext->_pEatWhitespace;
	}
	throw(std::logic_error("invalid type"));
}
//----< override function in derived class  >----------------------------
class EatWhitespace : public ConsumeState
{
public:
	EatWhitespace(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		//std::cout << "\n  eating whitespace";
		do {
			if (!_pContext->_pIn->good())  // end of stream
				return;
			_pContext->currChar = _pContext->_pIn->get();
		} while (std::isspace(_pContext->currChar) && _pContext->currChar != '\n');
	}
};
//----< override function in derived class  >----------------------------
class EatCppComment : public ConsumeState
{
public:
	EatCppComment(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		//std::cout << "\n  eating C++ comment";
		do {
			_pContext->token += _pContext->currChar;
			if (!_pContext->_pIn->good())  // end of stream
				return;
			_pContext->currChar = _pContext->_pIn->get();
		} while (_pContext->currChar != '\n');
		if (!_pContext->returnComm) {
			_pContext->token.clear();
		}
	}
};
//----< override function in derived class  >----------------------------
class EatCComment : public ConsumeState
{
public:
	EatCComment(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		//std::cout << "\n  eating C comment";
		do {
			_pContext->token += _pContext->currChar;
			if (!_pContext->_pIn->good())  // end of stream
				return;
			_pContext->currChar = _pContext->_pIn->get();
			if (_pContext->currChar == '\n')
				_pContext->lineCount++;
		} while (_pContext->currChar != '*' || _pContext->_pIn->peek() != '/');
		_pContext->token += _pContext->currChar;
		_pContext->currChar = _pContext->_pIn->get();
		_pContext->token += _pContext->currChar;
		_pContext->currChar = _pContext->_pIn->get();
		if (!_pContext->returnComm) {
			_pContext->token.clear();
		}
	}
};
//----< override function in derived class  >----------------------------
class EatQuoter : public ConsumeState
{
public:
	EatQuoter(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		do {
			_pContext->token += _pContext->currChar;
			if (!_pContext->_pIn->good())  // end of stream
				return;
			_pContext->currChar = _pContext->_pIn->get();

			if (_pContext->currChar == '\\')
			{
				_pContext->token += '\\';
				_pContext->token += _pContext->_pIn->peek();
				if (!_pContext->_pIn->good())  // end of stream
					return;
				_pContext->currChar = _pContext->_pIn->get();
				_pContext->currChar = _pContext->_pIn->get();
			}

		} while (_pContext->currChar != '"');
		_pContext->token += _pContext->currChar;
		_pContext->currChar = _pContext->_pIn->get();
	}
};
//----< override function in derived class  >----------------------------
class EatPunctuator : public ConsumeState
{
public:
	EatPunctuator(Context* pContext)
	{
		_pContext = pContext;
	}
	std::string temp;
	virtual void eatChars() {
		_pContext->token.clear();
		do {
			_pContext->token += _pContext->currChar;
			if (!_pContext->_pIn->good())  // end of stream
				return;
			_pContext->currChar = _pContext->_pIn->get();
			//check current pair charactor
			for (int i = 0; i < (int)_pContext->pVector.size(); i++) {
				if (_pContext->token == _pContext->pVector[i])
					return;
			}
			//check half-pair charactor
			temp.clear();
			temp = _pContext->token;
			temp += _pContext->currChar;
			for (int i = 0; i < (int)_pContext->pVector.size(); i++)
				if (temp == _pContext->pVector[i]) {
					_pContext->token += _pContext->currChar;
					if (!_pContext->_pIn->good())  // end of stream
						return;
					_pContext->currChar = _pContext->_pIn->get();
					return;
				}
			//check current single charactor
			for (int i = 0; i < (int)_pContext->sVector.size(); i++)
				if (_pContext->token == _pContext->sVector[i])
					return;
			//check next single charactor
			temp.clear();
			temp += _pContext->currChar;
			for (int i = 0; i < (int)_pContext->sVector.size(); i++)
				if (temp == _pContext->sVector[i])
					return;
			//check next pair charactor	  
			temp += _pContext->_pIn->peek();
			for (int i = 0; i < (int)_pContext->pVector.size(); i++)
				if (temp == _pContext->pVector[i])
					return;
		} while (ispunct(_pContext->currChar) && _pContext->currChar != '_');
	}
};
//----< override function in derived class  >----------------------------
class EatAlphanum : public ConsumeState
{
public:
	EatAlphanum(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		//std::cout << "\n  eating alphanum";
		do {
			_pContext->token += _pContext->currChar;
			if (!_pContext->_pIn->good())  // end of stream
				return;
			_pContext->currChar = _pContext->_pIn->get();
		} while (isalnum(_pContext->currChar) || _pContext->currChar == '_');
	}
};
//----< override function in derived class  >----------------------------
class EatNewline : public ConsumeState
{
public:
	EatNewline(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		//std::cout << "\n  eating alphanum";
		_pContext->token += _pContext->currChar;
		if (!_pContext->_pIn->good())  // end of stream
			return;
		_pContext->currChar = _pContext->_pIn->get();
	}
};

//----< constructor >----------------------------
Context::Context()
{
	_pEatAlphanum = new EatAlphanum(this);
	_pEatCComment = new EatCComment(this);
	_pEatCppComment = new EatCppComment(this);
	_pEatQuoter = new EatQuoter(this);
	_pEatPunctuator = new EatPunctuator(this);
	_pEatWhitespace = new EatWhitespace(this);
	_pEatNewline = new EatNewline(this);
	_pState = _pEatWhitespace;
	lineCount = 1;
	returnComm = false;
}
//----< return shared resources >------------------------------------

Context::~Context()
{
	delete _pState;
	delete _pEatAlphanum;
	delete _pEatCComment;
	delete _pEatCppComment;
	delete _pEatQuoter;
	delete _pEatPunctuator;
	delete _pEatWhitespace;
	delete _pEatNewline;
}
//----< ConsumeState constructor  >----------------------------
ConsumeState::ConsumeState() { }

//----< ConsumeState deconstructor  >----------------------------
ConsumeState::~ConsumeState() { }
//----< constructor >----------------------------
Toker::Toker() : pConsumer() 
{
	_pContext = new Context();
	pConsumer = _pContext->_pEatWhitespace;
	pConsumer->setContext(_pContext);
}
//----< destructor >----------------------------

Toker::~Toker() {
	delete _pContext;
	_pContext = nullptr;
}
//----< attach file content  >----------------------------
bool Toker::attach(std::istream* pIn)
{
	if (pIn != nullptr && pIn->good())
	{
		pConsumer->attach(pIn);
		return true;
	}
	return false;
}
//----< get one token  >----------------------------
std::string Toker::getTok()
{
	while (true)
	{
		if (!pConsumer->canRead())
			return "";
		pConsumer->consumeChars();
		if (pConsumer->hasTok())
			break;
	}
	return pConsumer->getTok();
}

//----< arrival end ?  >----------------------------
bool Toker::canRead() { return pConsumer->canRead(); }
//----< set Special Single Chars  >----------------------------
void Toker::setSpecialSingleChars(std::string ssc) { pConsumer->addSingle(ssc); }
//----< set Special Char pairs  >----------------------------
void Toker::setSpecialCharpairs(std::string scp) { pConsumer->addPair(scp); }

//----< set return comment bool  >----------------------------
void Toker::returnComments(bool rCmm) {
	pConsumer->setReturnComm(rCmm);
}

//----< get line count  >----------------------------
size_t Toker::getLine() {
	return pConsumer->getLineCount();
}

//----< testLog >----------------------------
void testLog(const std::string& msg) {
#ifdef TEST_LOG
	std::cout << "\n  " << msg;
#endif
}

//----< test stub >--------------------------------------------------

#ifdef TEST_TOKENIZER

#include <fstream>
int main()
{
	StringHelper *ut = new StringHelper();
	ut->Title("Tokenizer test stub", '=');
	ut->title("test1");

	std::string fileSpec = "../test.txt";
	std::ifstream in(fileSpec);
	if (!in.good())
	{
		std::cout << "\n  can't open " << fileSpec << "\n\n";
		return 1;
	}

	Toker toker;
	toker.attach(&in);

	toker.setSpecialSingleChars("$"); // test add special character
	toker.setSpecialCharpairs("$%"); // test add special character pair

	do
	{
		std::string tok = toker.getTok();
		if (tok == "\n")
			tok = "newline";
		std::cout << "\n -- " << tok;
	} while (in.good());

	std::cout << "\n\n";
	return 0;
}
#endif
