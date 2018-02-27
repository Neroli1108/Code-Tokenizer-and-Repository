#pragma once
///////////////////////////////////////////////////////////////////////
// Task.h   -   organize thread process                              //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Yunding Li, Computer Engineering   		             //
//				yli327@syr.edu								         //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public Task class.
* Let work items working in parallel thread 
*
* Public Interface:
* ------------------
* void doSomething(CallObj<Result> *obj);
* bool checkTaskDone();
* std::vector<Result> dropResult();
*
* Build Process:
* --------------
* Required Files: ThreadPool.h, ThreadPool.cpp
*                 Utilities.h, Utilities.cpp,
* Build Command: devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 8 Apr 2016
* - first release
*
*/

#include <windows.h>
#include <vector>
#include "../ThreadPool/ThreadPool.h"

#define THNUM 4

template<typename Result>
using CallObj = std::function<Result()>;

template<typename Result>
class Tasks {
public:
	Tasks() {
		_thdpool.start(THNUM);
	}
	~Tasks() {}
	void doSomething(CallObj<Result> *obj);
	bool checkTaskDone();
	std::vector<Result> dropResult();
private:
	size_t _objNumber;
	ThreadPool<Result> _thdpool;
};

//----< doing a working in one thread >------------
template<typename Result>
void Tasks<Result>::doSomething(CallObj<Result> *obj) {
	_objNumber++;
	_thdpool.doWork(obj);
	Sleep(100);
}

//----< ckeck for working items done in multi-thread >------------
template<typename Result>
bool Tasks<Result>::checkTaskDone() {
	for (int i = 0; i < THNUM; i++)
	{
		_thdpool.doWork(nullptr);
	}
	while (_thdpool.checkResultItem() == _objNumber)
			return true;
	return false;
}

//----< get process result from multi-thread >------------
template<typename Result>
std::vector<Result> Tasks<Result>::dropResult() {
	std::vector<Result> res;
	Result temp;
	_thdpool.wait();
	for (int i = 0; i < _objNumber; i++)
	{
		temp = _thdpool.result();
		res.push_back(temp);
	}

	return res;
}