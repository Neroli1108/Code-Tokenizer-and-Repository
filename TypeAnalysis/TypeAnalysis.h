#pragma once
///////////////////////////////////////////////////////////////////////
// TypeAnalysis.h - analysis type definition of a file               //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Yunding Li, Computer Engineering   		             //
//				Yli327@syr.edu								         //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public TypeAnalysis class.
*
* Public Interface:
* ------------------
* TypeAnalysis(const std::string& path);
* void doAnalysis();
* map<string, TypeValue> *getTable();
*
* Build Process:
* --------------
* Required Files: Parser.h, Parser.cpp, ConfigureParser.h, ConfigureParser.cpp
*                 Utilities.h, Utilities.cpp,
* Build Command: devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 8 Apr 2016
* - first release
*
*/
#include <iostream>
#include "../Parser/Parser.h"
#include "../Parser/ConfigureParser.h"
#include "../Utilities/Utilities.h"
using namespace std;
class TypeAnalysis {
public:
	TypeAnalysis(const std::string& path) : path_(path) { _typeTable = new map<string, TypeValue>; }
	~TypeAnalysis() { } // cannot delete _typeTable because of refence pointer 
	void doAnalysis();
	map<string, TypeValue> *getTable() { return _typeTable; }
private:
	map<string, TypeValue> *_typeTable;
	std::string path_;
};