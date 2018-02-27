/////////////////////////////////////////////////////////////////////
//  RulesAndActions.cpp - declares new parsing rules and actions   //
//  ver 3.0                                                        //
//  Language:      Visual Studio 2016                              //
//  Platform:      MacBook Pro, Windows 10                         //
//  Application:   CSE687 Pr3, Sp16								   //
//  Author:        Yunding Li, Computer Engineering,			   //
//			       Syracuse University, yli327@syr.edu	           //
//  Source:        Jim Fawcett, Syracuse University, CST 4-187     //
//                 jfawcett@twcny.rr.com                           //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_ACTIONSANDRULES

#include <iostream>
#include "ActionsAndRules.h"
#include "Tokenizer.h"
#include "SemiExpression.h"

// ------< test stub >--------
int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ActionsAndRules class\n "
		<< std::string(30, '=') << std::endl;

	try
	{
		std::queue<std::string> resultsQ;
		PreprocToQ ppq(resultsQ);
		PreprocStatement pps;
		pps.addAction(&ppq);

		FunctionDefinition fnd;
		PrettyPrintToQ pprtQ(resultsQ);
		fnd.addAction(&pprtQ);

		Toker toker("../ActionsAndRules.h");
		SemiExp se(&toker);
		Parser parser(&se);
		parser.addRule(&pps);
		parser.addRule(&fnd);
		while (se.get())
			parser.parse();
		size_t len = resultsQ.size();
		for (size_t i = 0; i<len; ++i)
		{
			std::cout << "\n  " << resultsQ.front().c_str();
			resultsQ.pop();
		}
		std::cout << "\n\n";
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
	}
}
#endif
