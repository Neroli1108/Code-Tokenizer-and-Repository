#pragma once
///////////////////////////////////////////////////////////////////////
// Display.h - display type table and dependency table               //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Cunyang Shui, Computer Engineering   		         //
//				cushui@syr.edu								         //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public Displaysis class, support for Proj#3-OOD S16.
* Display Type table, and Dependenct table of an element pair or whole files
*
* Public Interface:
* ------------------
* void showTypeTable(map<string, TypeValue> typeTable);
* void showDepTable(map<string, DepValue> depTable);
* void showTypeElement(string key, TypeValue value);
* void showDepElement(string key, DepValue value);
* void showCombindResult(map<string, list<string>> combinedTable);
*
* Build Process:
* --------------
* Required Files: Parser.h, Parser.cpp
*                 Utilities.h, Utilities.cpp,
* Build Command: devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 8 Apr 2016
* - first release
*
*/

#include "../Parser/Parser.h"

class Display {
public:
	Display(){}
	~Display(){}
	void showTypeTable(map<string, TypeValue> typeTable);
	void showDepTable(map<string, DepValue> depTable);
	void showTypeElement(string key, TypeValue value);
	void showDepElement(string key, DepValue value);
	void showCombindResult(map<string, list<string>> combinedTable);
};

