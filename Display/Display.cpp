///////////////////////////////////////////////////////////////////////
// Display.cpp - display type table and dependency table             //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Yunding Li, Computer Engineering   		             //
//				yli327@syr.edu								         //
///////////////////////////////////////////////////////////////////////

#include "Display.h"

// ------< display whole type table >-----
void Display::showTypeTable(map<string, TypeValue> typeTable)
{
	std::map<string, TypeValue>::iterator my_Itr;
	for (my_Itr = typeTable.begin(); my_Itr != typeTable.end(); my_Itr++)
	{
		string k = my_Itr->first;
		TypeValue v = my_Itr->second;
		showTypeElement(k, v);
	}
}

// ------< display one element in type table >-----
void Display::showTypeElement(string key, TypeValue value)
{
	list<string>::iterator list_it_type = value.type.begin();
	list<string>::iterator list_it_in = value.inFile.begin();
	list<string>::iterator list_it_functionType = value.typeName.begin();
	list<list<string>>::iterator list_it_ns = value.namespaces.begin();
	list<string>::iterator list_it_temp;

	for (size_t i = 0; i < value.type.size(); i++)
	{
		cout << "\n    type name: " << key << endl;
		cout << "         type: " << *list_it_type << endl;
		if (*list_it_type == "function") 
			if (*list_it_functionType == "globe function")
				cout << "function type: " << "globe function" << endl;
			else
				cout << "function type: " << "member function - " << *list_it_functionType << endl;
		cout << " find in file: " << *list_it_in << endl;
		if (list_it_ns->size() < 2)
			cout << " in namespace: globe namespace" << endl;
		if (list_it_ns->size() == 2)
		{
			list_it_temp = list_it_ns->begin();
			list_it_temp++;
			cout << " in namespace: " << *list_it_temp << endl;
		}
		if (list_it_ns->size() > 2)
		{
			list_it_temp = list_it_ns->begin();
			list_it_temp++;
			cout << " in namespace: " << *list_it_temp;
			list_it_temp++;
			for (; list_it_temp != list_it_ns->end(); list_it_temp++)
				cout << ", " << *list_it_temp;
		}
		list_it_type++;
		list_it_in++;
		list_it_functionType++;
		list_it_ns++;
	}
}

// ------< display whole dependency table >-----
void Display::showDepTable(map<string, DepValue> depTable)
{
	std::map<string, DepValue>::iterator my_Itr;
	for (my_Itr = depTable.begin(); my_Itr != depTable.end(); my_Itr++)
	{
		string k = my_Itr->first;
		DepValue v = my_Itr->second;
		showDepElement(k, v);
	}
}

// ------< display one element in dependency table >-----
void Display::showDepElement(string key, DepValue value)
{
	list<string>::iterator list_it_type = value.type.begin();
	list<string>::iterator list_it_in = value.inFile.begin();
	list<string>::iterator list_it_dep = value.depFile.begin();
	for (size_t i = 0; i < value.type.size(); i++)
	{
		cout << "\n     type name: " << key << endl;
		cout << "          type: " << *list_it_type << endl;
		cout << "use it in file: " << *list_it_in << endl;
		cout << "depend on file: " << *list_it_dep << endl;
		list_it_type++;
		list_it_in++;
		list_it_dep++;
	}
}

// ------< display combined dependency analysis without repeat >-----
void Display::showCombindResult(map<string, list<string>> combinedTable)
{
	map<string, list<string>>::iterator my_Itr;
	for (my_Itr = combinedTable.begin(); my_Itr != combinedTable.end(); my_Itr++)
	{
		list<string>::iterator it = my_Itr->second.begin();
		cout << "     file: " << my_Itr->first << endl;
		cout << "depend on: " << *it;
		it++;
		for (; it != my_Itr->second.end(); it++)
			cout << ",\n           " << *it;
		cout << endl;
		cout << endl;
	}


}
// ------< test stub >--------
#ifdef TEST_DISPLAY
int main() {
	string key1 = "key1";
	string key2 = "key2";
	TypeValue type1;
	type1.type.push_back("type1");
	type1.inFile.push_back("in file1");
	list<string> ns1;
	ns1.push_back("globe namespace");
	type1.namespaces.push_back(ns1);
	type1.type.push_back("type2");
	type1.inFile.push_back("in file2");
	ns1.push_back("namespace1");
	type1.namespaces.push_back(ns1);
	TypeValue type2;
	type2.type.push_back("type3");
	type2.inFile.push_back("in file3");
	ns1.push_back("namespace3");
	type2.namespaces.push_back(ns1);
	map<string, TypeValue> typetable;
	typetable.insert(pair<string, TypeValue>(key1, type1));
	typetable.insert(pair<string, TypeValue>(key2, type2));
	DepValue dep1;
	dep1.type.push_back("type1");
	dep1.inFile.push_back("in file1");
	dep1.depFile.push_back("dep file1");
	dep1.type.push_back("type2");
	dep1.inFile.push_back("in file2");
	dep1.depFile.push_back("dep file2");
	DepValue dep2;
	dep2.type.push_back("type3");
	dep2.inFile.push_back("in file3");
	dep2.depFile.push_back("dep file3");
	map<string, DepValue> deptable;
	deptable.insert(pair<string, DepValue>(key1, dep1));
	deptable.insert(pair<string, DepValue>(key2, dep2));
	Display display;
	cout << "test show Type Table" << endl;
	cout << "====================" << endl;
	display.showTypeTable(typetable);
	cout << "\n\n\ntest show Dep Table" <<endl;
	cout << "====================" << endl;
	display.showDepTable(deptable);
}
#endif