#pragma once
///////////////////////////////////////////////////////////////////////
// ParaDepAnalysis.h - analysis type and dependency of a file        //
//                     collection in parallel                        //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Yunding Li, Computer Engineering   		             //
//				yli327@syr.edu								         //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public ParaDepAnalysis class.
* Combine type analysis, dependency analysis and task packages, 
* let them work togrther in parallel
*
* Public Interface:
* ------------------
* void doParaTypeAnalysis();
* void doParaDepAnalysis();
* void combineDependency();
* WorkResult1 getTypeTable();
* WorkResult2 getDepTable();
* map<string, list<string>> getCombinedResult();
*
* Build Process:
* --------------
* Required Files: Parser.h, Parser.cpp, Tasks.h
*                 TypeAnalysis.h, TypeAnalysis.cpp
*                 DepAnalysis.h, DepAnalysis.cpp
*                 Display.h, Display.cpp
*                 Utilities.h, Utilities.cpp,
* Build Command: devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 8 Apr 2016
* - first release
*
*/

#include <vector>
#include <iostream>
#include "../Parser/Parser.h"
#include "../Tasks/Tasks.h"
#include "../TypeAnalysis/TypeAnalysis.h"
#include "../DependencyAnalysis/DepAnalysis.h"
#include "../Display/Display.h"

using WorkResult1 = map<string, TypeValue>;
using WorkResult2 = map<string, DepValue>;
struct SingleDependency {
	string rawFile;
	string depFile;
};
class ParaDepAnalysis {
public:
	ParaDepAnalysis(const vector<string>& path):_paths(path) {
		_task_1 = new Tasks<WorkResult1*>();
		_task_2 = new Tasks<WorkResult2*>();
	}
	~ParaDepAnalysis() { 
	}
	void doParaTypeAnalysis();
	void doParaDepAnalysis();
	void combineDependency();
	WorkResult1 getTypeTable() { return _typeResult; }
	WorkResult2 getDepTable() { return _depResult; }
	map<string, list<string>> getCombinedResult() { return _combinedResult; }

private:
	Tasks<WorkResult1*> *_task_1;
	Tasks<WorkResult2*> *_task_2;
	vector<string> _paths;
	WorkResult1 _typeResult;
	WorkResult2 _depResult;
	map<string, list<string>> _combinedResult;
	void mergeTypeTable(vector<WorkResult1*> typeResults);
	void mergeDepTable(vector<WorkResult2*> depResults);

};

