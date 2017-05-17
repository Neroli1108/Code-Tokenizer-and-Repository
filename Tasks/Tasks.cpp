///////////////////////////////////////////////////////////////////////
// Task.h   -   organize thread process                              //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Cunyang Shui, Computer Engineering   		         //
//				cushui@syr.edu								         //
//////////////////////////////////////////////////////////////////////
#ifdef TEST_TASKS
#include "Tasks.h"
#include "../FileMgr/FileMgr.h"
#include "../ThreadPool/ThreadPool.h"

// ------< test stub >--------
int main(int argc, char* argv[]) {
	using namespace Scanner;
	StringHelper *ut = new StringHelper();
	ut->Title(" test Task package ", '=');
	ut->putline();

	WorkItem<std::string> obj = []() {
		std::ostringstream temp;
		temp << std::this_thread::get_id();
		std::string threadID = temp.str();
		std::cout << "\n  working on thread " << threadID;
		return "thread" + threadID + " result";
	};
	ut->title("  start process callable object");
	Tasks<std::string> task;
	for (size_t i = 0; i < 10; i++)
		task.doSomething(&obj);

	ut->putline();
	ut->title("  stop threads");
	task.checkTaskDone();
	std::vector<std::string> results = task.dropResult();
	int number = 1;

	ut->putline();
	ut->title("  display results");
	for (auto x : results)
		std::cout << "\n  Result " << number++ << ": " << x;
	ut->putline();
	ut->putline();
}
#endif