#pragma once
/*
* Package Operations:
* -------------------
* This package provides a ThreadPool Task class to organize working thread
* reference Dr.Fawcett's class code - S 15
*
* Public Interface:
* ------------------
* void start(size_t number);
* void doWork(WorkItem<Result>* pWi);
* Result result();
* void wait();
* size_t checkResultItem();
*
* Build Process:
* --------------
* Build Command: devenv Project3.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 : 8 Apr 2016
* - first release
*
*/

#include <thread>
#include <functional>
#include <vector>
#include "BlockingQueue.h"

template<typename Result>
using WorkItem = std::function<Result()>;
///////////////////////////////////////////////////////////////////////
// class to process work items
// - each work item is processed sequentially on a single child thread

template<typename Result>
class ThreadPool
{
public:
	void start(size_t number);
	void doWork(WorkItem<Result>* pWi);
	Result result();
	void wait();
	size_t checkResultItem();
	~ThreadPool();
private:
	std::vector<std::thread*> _pThread;
	BlockingQueue<WorkItem<Result>*> _workItemQueue;
	BlockingQueue<Result> _resultsQueue;
};
//----< wait for child thread to terminate >---------------------------

template<typename Result>
void ThreadPool<Result>::wait()
{
	for (int i = 0; i < _pThread.size(); i++)
		_pThread[i]->join();
}
//----< enqueue work item >--------------------------------------------

template<typename Result>
void ThreadPool<Result>::doWork(WorkItem<Result>* pWi)
{
	_workItemQueue.enQ(pWi);
}
//----< retrieve results with blocking call >--------------------------

template<typename Result>
Result ThreadPool<Result>::result()
{
	return _resultsQueue.deQ();
}
//----< start child thread that dequeus work items >-------------------

template<typename Result>
void ThreadPool<Result>::start(size_t number)
{
	static std::mutex m;
	std::function<void()> threadProc =
		[&]() {
		while (1) {
			WorkItem<Result>* pWi = _workItemQueue.deQ();
			if (pWi == nullptr) {
				m.lock();
				std::cout << "\n  thread " << std::this_thread::get_id() << " shut down";
				m.unlock();
				return;
			}
			Result result = (*pWi)();
			_resultsQueue.enQ(result);
		}
	};

	for (size_t i = 0; i < number; i++)
	{
		std::thread *aThread = new std::thread(threadProc);
		_pThread.push_back(aThread);
	}
}
//----< return result queue item number >-------------------

template<typename Result>
size_t ThreadPool<Result>::checkResultItem() {
	return _resultsQueue.size();
}

//----< clean up heap >------------------------------------------------

template<typename Result>
ThreadPool<Result>::~ThreadPool()
{
	while (_pThread.size())
	{
		delete _pThread[0];
		_pThread.erase(_pThread.begin());
	}
}