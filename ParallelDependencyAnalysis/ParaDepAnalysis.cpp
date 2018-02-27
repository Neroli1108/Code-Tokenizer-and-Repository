///////////////////////////////////////////////////////////////////////
// ParaDepAnalysis.cpp - analysis type and dependency of a file      //
//                     collection in parallel                        //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Yunding Li, Computer Engineering   		             //
//				yli327@syr.edu								         //
///////////////////////////////////////////////////////////////////////

#include "ParaDepAnalysis.h"

// ------< doing type analysis of a file collection in parallel >-----
void ParaDepAnalysis::doParaTypeAnalysis() {
	static mutex m;
	static int countNum = 0;
	static vector<string> paths = _paths;
	countNum = 0;
	paths = _paths;
	static CallObj<WorkResult1*> obj = [&]() {
		m.lock();
		string path = paths[countNum++];
		std::cout << "\n  working on thread " << std::this_thread::get_id() << "    number:" << countNum;
		std::cout << "\n  path: " << path;
		m.unlock();

		TypeAnalysis typeAs(path);
		typeAs.doAnalysis();
		WorkResult1* pTable = typeAs.getTable();

		return pTable;
	};

	for (auto x : _paths)
		_task_1->doSomething(&obj);

	_task_1->checkTaskDone();
	vector<WorkResult1*> typeResults = _task_1->dropResult();
	mergeTypeTable(typeResults);
}

// ------< doing dependency analysis of a file collection in parallel >-----
void ParaDepAnalysis::doParaDepAnalysis() {
	static mutex m;
	static int countNum = 0;
	static vector<string> paths = _paths;
	static WorkResult1 mergedTable = _typeResult;
	countNum = 0;
	paths = _paths;
	mergedTable = _typeResult;
	static CallObj<WorkResult2*> obj = [&]() {
		m.lock();
		string path = paths[countNum++];
		std::cout << "\n  working on thread " << std::this_thread::get_id() << "    number:" << countNum;
		std::cout << "\n  path: " << path;
		m.unlock();

		DepAnalysis depAs(path);
		depAs.doAnalysis(&mergedTable);
		WorkResult2* table = depAs.getTable();
		return table;
	};

	for (auto x : _paths)
		_task_2->doSomething(&obj);

	_task_2->checkTaskDone();
	vector<WorkResult2*> depResults = _task_2->dropResult();
	mergeDepTable(depResults);
}

// ------< merge type table of each files to one >-----
void ParaDepAnalysis::mergeTypeTable(vector<WorkResult1*> typeResults) {
	WorkResult1 mergedResult;

	std::map<string, TypeValue>::iterator it;
	std::map<string, TypeValue>::iterator my_Itr;

	for (auto tempTable : typeResults)
	{
		for (my_Itr = tempTable->begin(); my_Itr != tempTable->end(); ++my_Itr)
		{
			string k = my_Itr->first;
			TypeValue v = my_Itr->second;
			if (mergedResult.count(k))
			{
				it = mergedResult.find(k);
				TypeValue temp = it->second;
				for (auto x : temp.type)		v.type.push_back(x);
				for (auto x : temp.typeName)	v.typeName.push_back(x);
				for (auto x : temp.inFile)		v.inFile.push_back(x);
				for (auto x : temp.namespaces)	v.namespaces.push_back(x);
				mergedResult.erase(it);
			}

			mergedResult.insert(pair<string, TypeValue>(k, v));
			//std::cout << "\n merge Type here" << std::endl;
		}
	}
	std::cout << "\n\n merges Type Table Size is:" << mergedResult.size() << endl;
	_typeResult = mergedResult;
}

// ------< merge dependency table of each files to one >-----
void ParaDepAnalysis::mergeDepTable(vector<WorkResult2*> depResults) {
	WorkResult2 mergedResult;
	std::map<string, DepValue>::iterator it;
	std::map<string, DepValue>::iterator my_Itr;

	for (auto tempTable : depResults)
	{
		for (my_Itr = tempTable->begin(); my_Itr != tempTable->end(); ++my_Itr)
		{
			string k = my_Itr->first;
			DepValue v = my_Itr->second;
			if (mergedResult.count(k))
			{
				it = mergedResult.find(k);
				DepValue temp = it->second;
				for (auto x : temp.type)		v.type.push_back(x);
				for (auto x : temp.typeName)	v.typeName.push_back(x);
				for (auto x : temp.inFile)		v.inFile.push_back(x);
				for (auto x : temp.depFile)		v.depFile.push_back(x);
				for (auto x : temp.namespaces)	v.namespaces.push_back(x);
				mergedResult.erase(it);
			}

			mergedResult.insert(pair<string, DepValue>(k, v));
			//std::cout << "\n merge Type here" << std::endl;
		}
	}
	//std::cout << "\n merges Type Table Size is:" << mergedResult.size() << endl;
	_depResult = mergedResult;
}

// ------< combine dependency table without repeat >-----
void ParaDepAnalysis::combineDependency()
{
	std::map<string, DepValue>::iterator my_Itr;
	list<SingleDependency> SingleCombinedResult;
	SingleDependency depdency;
	bool exist = false;
	for (my_Itr = _depResult.begin(); my_Itr != _depResult.end(); my_Itr++) {
		DepValue v = my_Itr->second;
		list<string>::iterator list_it_in = v.inFile.begin();
		list<string>::iterator list_it_dep = v.depFile.begin();
		for (size_t i = 0; i < v.inFile.size(); i++) {
			exist = false;
			list<SingleDependency>::iterator checkDp;
			for (checkDp = SingleCombinedResult.begin(); checkDp != SingleCombinedResult.end(); checkDp++)
				if (checkDp->rawFile == *list_it_in && checkDp->depFile == *list_it_dep)
					exist = true;
			if (!exist) {
				depdency.rawFile = *list_it_in;
				depdency.depFile = *list_it_dep;
				SingleCombinedResult.push_back(depdency);
			}
			list_it_in++;
			list_it_dep++;
		}
	}

	for (auto x : SingleCombinedResult)
	{
		string k = x.rawFile;
		list<string> v;
		v.push_back(x.depFile);
		map<string, list<string>>::iterator it;
		if (_combinedResult.count(k) > 0)
		{
			it = _combinedResult.find(k);
			for (auto m : it->second) v.push_back(m);
			_combinedResult.erase(it);
		}

		_combinedResult.insert(pair<string, list<string>>(k, v));
	}
}

#ifdef TEST_PARADEPANALYSIS
#include "../FileMgr/FileMgr.h"

// ------< test stub >--------
int main(int argc, char* argv[]) {
	std::cout << "\n  Testing Para Dependency Analysis";
	std::cout << "\n =================================";

	FileMgr fm("..");
	fm.search();
	std::vector<std::string> ds = fm.getStores();

	ParaDepAnalysis *para = new ParaDepAnalysis(ds);
	para->doParaTypeAnalysis();

	Display display;
	std::cout << "\n\n  Display type result";
	std::cout << "\n ======================\n";
	display.showTypeTable(para->getTypeTable());

	para->doParaDepAnalysis();

	std::cout << "\n\n  Display dep result";
	std::cout << "\n =====================\n";
	display.showDepTable(para->getDepTable());

	para->combineDependency();

	std::cout << "\n\n  Display combined result";
	std::cout << "\n ==========================\n";
	display.showCombindResult(para->getCombinedResult());

	std::cout << "\n\n";
	return 0;
}
#endif