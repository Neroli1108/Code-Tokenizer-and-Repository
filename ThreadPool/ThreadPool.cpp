///////////////////////////////////////////////////////////////////////
// ThreadPool.cpp - a thread pool                                    //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Cunyang Shui, Computer Engineering   		         //
//				cushui@syr.edu								         //
//  Source:     Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
#ifdef TEST_THREADPOOL
#include <string>
#include <iostream>
#include <conio.h>
#include <windows.h>
#include "ThreadPool.h"
#include "../Utilities/Utilities.h"

//----< demonstrate ProcessWorkItem class >----------------------------
using namespace Scanner;
using WorkResult = std::string;
// ------< test stub >--------
int main()
{
	StringHelper *ut = new StringHelper();

	ut->title("Enqueued Work Items");

	std::cout << "\n  main thread id = " << std::this_thread::get_id();
	ThreadPool<WorkResult> processor;
	processor.start(2);

	// define 1st work item

	WorkItem<WorkResult> wi1 = []() {
		std::cout << "\n  working on thread " << std::this_thread::get_id();
		return "Hello from wi1";
	};

	processor.doWork(&wi1);

	// define 2nd work item

	WorkItem<WorkResult> wi2 = []()
	{
		Sleep(1000);
		std::cout << "\n  working on thread " << std::this_thread::get_id();
		size_t sum = 0;
		for (size_t i = 0; i < 100000; ++i)
			sum += i;

		std::ostringstream temp;
		temp << "wi2 result = ";
		temp << sum;
		return temp.str();
	};

	processor.doWork(&wi2);

	// the following calls to result() block until results are enqueued

	//std::cout << "\n  " << processor.result();
	//std::cout << "\n  " << processor.result();
	//processor.doWork(nullptr);


	// wait for child thread to complete

	processor.wait();
	std::cout << "\n\n";
}
#endif