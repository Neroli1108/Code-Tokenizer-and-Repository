#pragma once
///////////////////////////////////////////////////////////////////////
// DepAnalysis.h - analysis dependency of a file                     //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Cunyang Shui, Computer Engineering   		         //
//				cushui@syr.edu								         //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public DepAnalysis class. 
* Before doing dependency analysis, we need get type table.
*
* Public Interface:
* ------------------
* DepAnalysis(const std::string& path);
* void doAnalysis(map<string, TypeValue> *typeTable);
* map<string, DepValue> *getTable();
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
#include <map>
#include "../Parser/Parser.h"
#include "../Parser/ConfigureParser.h"
using namespace std;
class DepAnalysis {
public:
	DepAnalysis(const std::string& path) : path_(path) { _depTable = new map<string, DepValue>; }
	~DepAnalysis() { }
	void doAnalysis(map<string, TypeValue> *typeTable);
	map<string, DepValue> *getTable() { return _depTable; }
private:
	std::string path_;
	map<string, DepValue> *_depTable;
};

