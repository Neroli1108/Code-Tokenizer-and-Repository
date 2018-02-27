/////////////////////////////////////////////////////////////////////
//  Parser.cpp - Analyzes C++ language constructs                  //
//  ver 1.6                                                        //
//  Language:    Visual Studio 2016                                //
//  Platform:    MacBook Pro, Windows 10                           //
//  Author:      Yunding Li, Computer Engineering,			       //
//			     Syracuse University, yli327@syr.edu	           //
//  Source:      Jim Fawcett, Syracuse University, CST 4-187       //
//               jfawcett@twcny.rr.com                             //
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "../Utilities/Utilities.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "../RulesAndActions/RulesAndActions.h"
#include "Parser.h"
#include "ConfigureParser.h"

using namespace Scanner;

//----< register parsing rule---type >--------------------------------
void Parser::addTypeRule(IRule* pRule)
{
	_rulesType.push_back(pRule);
}

//----< register parsing rule---dependency >--------------------------------
void Parser::addDepRule(IRule* pRule)
{
	_rulesDep.push_back(pRule);
}

//----< get next ITokCollection >------------------------------
bool Parser::next()
{
	// default operation of doFold(pTokColl) is to simply return
	// - code analysis provides specific rules for handling
	//   for statements and operator expressions which are
	//   bound in ConfigureParser.cpp

	bool succeeded = _pTokColl->get();
	_pTokColl->remove("\n");
	if (!succeeded)
		return false;
	return true;
}

//----< parse the SemiExp by applying all rules(Type) to it >--------
bool Parser::parseType()
{
	for (size_t i = 0; i < _rulesType.size(); ++i)
	{
		if (_rulesType[i]->doTest(_pTokColl))
			return true;
	}
	return false;
}

//----< parse the SemiExp by applying all rules(Dependency) to it >--------
bool Parser::parseDep()
{
	for (size_t i = 0; i < _rulesDep.size(); ++i)
	{
		if (_rulesDep[i]->doTest(_pTokColl))
			return true;
	}
	return false;
}

//----< set type table, only use in build >--------
void Parser::setTypeMap(map<string, TypeValue> *map) { _typeMap = map; }

//----< get type table >--------
map<string, TypeValue> *Parser::getTypeMap() { return _typeMap; }

//----< set dependency table, only use in build >--------
void Parser::setDepTable(map<string, DepValue> *map) { _depMap = map; }

//----< get dependency table >--------
map<string, DepValue> *Parser::getDepTable() { return _depMap; }

//----< register action with a rule >--------------------------
void IRule::addAction(IAction *pAction){ actions.push_back(pAction); }

//----< invoke all actions associated with a rule >------------
void IRule::doActions(ITokCollection*& pTokColl){
	if (actions.size() > 0)
		for (size_t i = 0; i<actions.size(); ++i)
			actions[i]->doAction(pTokColl);
}

//----< test stub >--------------------------------------------
#ifdef TEST_PARSER

#include <queue>
#include <string>
int main()
{
	StringHelper *ut = new StringHelper();
	ut->Title("Testing Parser Class", '=');
	ut->putline();
		std::string fileSpec = "Z://Desktop/CSE-687/Project3/Parser/Parser.cpp";
		std::string msg = "Processing file " + fileSpec;
		ut->Title(msg);
		ut->putline();
		ConfigParseToConsole configure;
		Parser* pParser = configure.Build();
		try
		{
			if (pParser)
			{
				if (!configure.Attach(fileSpec))
				{
					std::cout << "\n  could not open file " << fileSpec << std::endl;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;
			}

			while (pParser->next())
				pParser->parseType();
			std::cout << "\n";
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
	std::cout << "\n";
}

#endif
