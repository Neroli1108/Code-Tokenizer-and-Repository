#ifndef RULESANDACTIONS_H
#define RULESANDACTIONS_H
/////////////////////////////////////////////////////////////////////
//  RulesAndActions.h - declares new parsing rules and actions     //
//  ver 3.0                                                        //
//  Language:      Visual Studio 2016                              //
//  Platform:      MacBook Pro, Windows 10                         //
//  Application:   CSE687 Pr3, Sp16								   //
//  Author:        Yunding Li, Computer Engineering,			   //
//			       Syracuse University, yli327@syr.edu	           //
//  Source:        Jim Fawcett, Syracuse University, CST 4-187     //
//                 jfawcett@twcny.rr.com                           //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module defines several action classes.  Its classes provide
specialized services needed for specific applications.  The modules
Parser, SemiExpression, and Tokenizer, are intended to be reusable
without change.  This module provides a place to put extensions of
these facilities and is not expected to be reusable.

Build Process:
==============
Required files
- Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,
  RulesAndActions.h, RulesAndActions.cpp, ConfigureParser.cpp,
  itokCollection.h, SemiExp.h, SemiExp.cpp, tokenizer.h, tokenizer.cpp
  Build commands
- devenv Project3.sln /rebuild debug

Maintenance History:
====================
ver 3.0 :  8 Apr 16
- change almost all rules to handle type and dependenct information 
ver 2.2 : 14 Mar 16
- add a tree in Repository class
- replace element with ASNode
- rewrite all rules and actions to detect all scopes and add them to the tree in Repository
ver 2.1 : 15 Feb 16
- small functional change to a few of the actions changes display strategy
- preface the (new) Toker and SemiExp with Scanner namespace
ver 2.0 : 01 Jun 11
- added processing on way to building strong code analyzer
ver 1.1 : 17 Jan 09
- changed to accept a pointer to interfaced ITokCollection instead
of a SemiExpression
ver 1.0 : 12 Jan 06
- first release

*/
//
#include <queue>
#include <string>
#include <sstream>
#include <cctype>
#include <mutex>

#include "../Parser/Parser.h"
#include "../SemiExp/itokcollection.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"


class Repository  // application specific
{
public:
	Repository(Scanner::Toker* pToker)
	{
		p_Toker = pToker;
		typeMap = new map<string, TypeValue>();
		depMap = new map<string, DepValue>();
		myNamespace.push_back("default namespace");
		usingNamespace.push_back("default namespace");
		scopePair.push_back(0);
		usingNamespaceScope.push_back(0);
	}
	map<string, TypeValue> *getTypeMap() { return typeMap; }
	map<string, DepValue> *getDepMap() { return depMap; }
	Scanner::Toker* Toker() { return p_Toker; }

	bool showFind = true;
	list<string> myNamespace;
	string *fileName;
	map<string, TypeValue> *mergedTypeTable;
	list<int> scopePair;

	string currentClass = "no calss";
	int classScopePair = 0;
	bool inClassDef = false;

	list<string> usingNamespace;
	list<int> usingNamespaceScope;
private:
	map<string, TypeValue> *typeMap;
	map<string, DepValue> *depMap;
	Scanner::Toker* p_Toker;
};

/////////////////////////////////////////
// rule to detect { or } in first pass 
class scopeRule : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		if (pTc->find("}") < pTc->length() || pTc->find("{") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return true;
	}
};

//////////////////////////////////////////
// action to handle { or } in first pass
class scopeAction : public IAction
{
	Repository* p_Repos;
public:
	scopeAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		if (pTc->find("{") < pTc->length())
		{
			p_Repos->scopePair.back()++;
		}

		if (pTc->find("}") < pTc->length())
		{
			p_Repos->scopePair.back()--;
			if (!p_Repos->scopePair.back())
			{
				if (p_Repos->myNamespace.size() > 1) p_Repos->scopePair.pop_back();
				if (p_Repos->myNamespace.size() > 1) p_Repos->myNamespace.pop_back();
			}

			if (p_Repos->inClassDef)
			{
				p_Repos->classScopePair--;
				if (!p_Repos->classScopePair)
				{
					p_Repos->inClassDef = false;
					p_Repos->currentClass = "no class";
				}
			}
		}
	}
};

/////////////////////////////
// rule to detect namespace
class namespaceCheck : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			if (tc.find("namespace") < tc.length())
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

/////////////////////////////
// action to handle namespace
class namespaceCheckAction : public IAction
{
	Repository* p_Repos;
public:
	namespaceCheckAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		size_t len = tc.find("namespace");
		if (len < tc.length())
		{
			p_Repos->scopePair.push_back(0);
			p_Repos->scopePair.back()++;
			p_Repos->myNamespace.push_back(tc[len + 1]);
		}
	}
};

/////////////////////////////
// rule to detect enum
class enumType : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("enum");
			if (len < tc.length())
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

/////////////////////////////
// action to handle enum
class enumTypeAction : public IAction
{
	Repository* p_Repos;
public:
	enumTypeAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		p_Repos->scopePair.back()++;
		if (p_Repos->inClassDef) p_Repos->classScopePair++;

		ITokCollection& tc = *pTc;

		size_t len = tc.find("enum");
		string typeName;
		if (tc[len + 1] == "struct" || tc[len + 1] == "class")
			typeName = tc[len + 2];
		else
			typeName = tc[len + 1];

		TypeValue testValue;

		testValue.type.push_back("enum");
		testValue.typeName.push_back(typeName);
		testValue.inFile.push_back(p_Repos->fileName[0]);
		
		testValue.namespaces.push_back(p_Repos->myNamespace);
 
		map<string, TypeValue> *typeTable = p_Repos->getTypeMap();
		std::map<string, TypeValue>::iterator it;
		if (typeTable->count(typeName)){
			it = typeTable->find(typeName);
			TypeValue temp = it->second;
			for (auto x : temp.type)		testValue.type.push_back(x);
			for (auto x : temp.typeName)	testValue.typeName.push_back(x);
			for (auto x : temp.inFile)		testValue.inFile.push_back(x);
			for (auto x : temp.namespaces)	testValue.namespaces.push_back(x);
			typeTable->erase(it);
		}
		p_Repos->getTypeMap()->insert(pair<string, TypeValue>(typeName, testValue));
	}
};

/////////////////////////////
// rule to detect class
class classType : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("class");
			if (len < tc.length())
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

/////////////////////////////
// action to handle class
class classTypeAction : public IAction
{
	Repository* p_Repos;
public:
	classTypeAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;

		p_Repos->inClassDef = true;
		p_Repos->classScopePair++;
		size_t len = tc.find("class");
		p_Repos->currentClass = tc[len + 1];

		p_Repos->scopePair.back()++;

		TypeValue testValue;

		testValue.type.push_back("class");
		string typeName = tc[len + 1];
		testValue.typeName.push_back(typeName);
		testValue.inFile.push_back(p_Repos->fileName[0]);
		
		testValue.namespaces.push_back(p_Repos->myNamespace);

		map<string, TypeValue> *typeTable = p_Repos->getTypeMap();
		std::map<string, TypeValue>::iterator it;
		if (typeTable->count(typeName)){
			it = typeTable->find(typeName);
			TypeValue temp = it->second;
			for (auto x : temp.type)		testValue.type.push_back(x);
			for (auto x : temp.typeName)	testValue.typeName.push_back(x);
			for (auto x : temp.inFile)		testValue.inFile.push_back(x);
			for (auto x : temp.namespaces)	testValue.namespaces.push_back(x);
			typeTable->erase(it);
		}
		p_Repos->getTypeMap()->insert(pair<string, TypeValue>(typeName, testValue));
	}
};

/////////////////////////////
// rule to detect struct
class structType : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("struct");
			if (len < tc.length())
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

/////////////////////////////
// action to handle struct
class structTypeAction : public IAction
{
	Repository* p_Repos;
public:
	structTypeAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		p_Repos->scopePair.back()++;
		if (p_Repos->inClassDef) p_Repos->classScopePair++;

		ITokCollection& tc = *pTc;
		TypeValue testValue;

		testValue.type.push_back("struct");
		size_t len = tc.find("struct");
		string typeName = tc[len + 1];
		testValue.typeName.push_back(typeName);
		testValue.inFile.push_back(p_Repos->fileName[0]);

		testValue.namespaces.push_back(p_Repos->myNamespace);

		map<string, TypeValue> *typeTable = p_Repos->getTypeMap();
		std::map<string, TypeValue>::iterator it;
		if (typeTable->count(typeName)){
			it = typeTable->find(typeName);
			TypeValue temp = it->second;
			for (auto x : temp.type)		testValue.type.push_back(x);
			for (auto x : temp.typeName)	testValue.typeName.push_back(x);
			for (auto x : temp.inFile)		testValue.inFile.push_back(x);
			for (auto x : temp.namespaces)	testValue.namespaces.push_back(x);
			typeTable->erase(it);
		}
		p_Repos->getTypeMap()->insert(pair<string, TypeValue>(typeName, testValue));
	}
};

/////////////////////////////
// rule to detect typedef
class typedefType : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		if (pTc->find("typedef") < pTc->length()) {
			doActions(pTc);
			return true;
		}
		return false;
	}
};

/////////////////////////////
// action to handle typedef
class typedefTypeAction : public IAction
{
	Repository* p_Repos;
public:
	typedefTypeAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		TypeValue testValue;
		size_t length = tc.length();
		if (tc[--length] == ";")
		{
			int currT = tc[--length][0];
			while ((!std::isalnum(currT) && currT != '_') || std::isdigit(currT))
				currT = tc[--length][0];

			string typeName = tc[length];
			testValue.type.push_back("typedef");
			testValue.typeName.push_back(typeName);
			testValue.inFile.push_back(p_Repos->fileName[0]);

			testValue.namespaces.push_back(p_Repos->myNamespace);

			map<string, TypeValue> *typeTable = p_Repos->getTypeMap();
			std::map<string, TypeValue>::iterator it;
			if (typeTable->count(typeName)){
				it = typeTable->find(typeName);
				TypeValue temp = it->second;
				for (auto x : temp.type)		testValue.type.push_back(x);
				for (auto x : temp.typeName)	testValue.typeName.push_back(x);
				for (auto x : temp.inFile)		testValue.inFile.push_back(x);
				for (auto x : temp.namespaces)	testValue.namespaces.push_back(x);
				typeTable->erase(it);
			}
			p_Repos->getTypeMap()->insert(pair<string, TypeValue>(typeName, testValue));
		}
	}
};

/////////////////////////////
// rule to detect alias
class aliasType : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		if (pTc->find("using") < pTc->length() && pTc->find("=") < pTc->length()) {
			doActions(pTc);
			return true;
		}
		return false;
	}
};

/////////////////////////////
// action to handle alias
class aliasTypeAction : public IAction
{
	Repository* p_Repos;
public:
	aliasTypeAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		TypeValue testValue;

		testValue.type.push_back("alias");
		size_t len = tc.find("=");
		string typeName = tc[len - 1];
		testValue.typeName.push_back(typeName);
		testValue.inFile.push_back(p_Repos->fileName[0]);

		testValue.namespaces.push_back(p_Repos->myNamespace);

		map<string, TypeValue> *typeTable = p_Repos->getTypeMap();
		std::map<string, TypeValue>::iterator it;
		if (typeTable->count(typeName))
		{
			it = typeTable->find(typeName);
			TypeValue temp = it->second;
			for (auto x : temp.type)		testValue.type.push_back(x);
			for (auto x : temp.typeName)	testValue.typeName.push_back(x);
			for (auto x : temp.inFile)		testValue.inFile.push_back(x);
			for (auto x : temp.namespaces)	testValue.namespaces.push_back(x);
			typeTable->erase(it);
		}

		p_Repos->getTypeMap()->insert(pair<string, TypeValue>(typeName, testValue));
	}
};

/////////////////////////////
// rule to detect function
class functionType : public IRule
{
public:
	//----< is Special Key Word  ? >--------------------------------------------
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch", "]" };
		for (int i = 0; i < 6; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}

	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("operator");
			if (len < tc.length())
			{
				doActions(pTc);
				return true;
			}

			len = tc.find("function");
			if (len < tc.length())
				return false; // lambda no process

			len = tc.find("(");
			if (len < tc.length() && !isSpecialKeyWord(tc[len - 1]))
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

/////////////////////////////
// action to handle function
class functionTypeAction : public IAction
{
	Repository* p_Repos;
public:
	functionTypeAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc){
		p_Repos->scopePair.back()++;
		if (p_Repos->inClassDef) p_Repos->classScopePair++;
		ITokCollection& tc = *pTc;
		TypeValue testValue;
		size_t len = tc.find("operator");
		string typeName;
		string tempString = "globe function";
		if (p_Repos->inClassDef) tempString = p_Repos->currentClass;
		if (len < tc.length()) {
			typeName = "operator" + tc[len + 1];
			if (len > 0)
				if (tc[len - 1] == "::")
					tempString = tc[len - 2];
		} else {
			len = tc.find("(");
			if (len > 1 && tc[len - 2] == "~"){
				typeName = "~" + tc[len - 1];
				if (len > 2) 
					if (tc[len - 3] == "::")
						tempString = tc[len - 4];
			} else {
				typeName = tc[len - 1];
				if (typeName == "main") return;
				if (len > 1) 
					if (tc[len - 2] == "::")
						tempString = tc[len - 3];
			}
		}
		if (tempString == ">") return;  // fuck template class
		if (tempString == "globe function" && p_Repos->myNamespace.size() == 1) return;
		testValue.type.push_back("function");
		testValue.typeName.push_back(tempString);
		testValue.inFile.push_back(p_Repos->fileName[0]);
		testValue.namespaces.push_back(p_Repos->myNamespace);
		map<string, TypeValue> *typeTable = p_Repos->getTypeMap();
		std::map<string, TypeValue>::iterator it;
		if (typeTable->count(typeName)){
			it = typeTable->find(typeName);
			TypeValue temp = it->second;
			for (auto x : temp.type)		testValue.type.push_back(x);
			for (auto x : temp.typeName)	testValue.typeName.push_back(x);
			for (auto x : temp.inFile)		testValue.inFile.push_back(x);
			for (auto x : temp.namespaces)	testValue.namespaces.push_back(x);
			typeTable->erase(it);
		}
		p_Repos->getTypeMap()->insert(pair<string, TypeValue>(typeName, testValue));
	}
};


/////////////////////////////
// rule to ckeck class
class classCheck : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("class");
			if (len < tc.length())
			{
				doActions(pTc);
				return false;
			}
		}
		return false;
	}
};

////////////////////////////////
// action to handle class check
class classCheckAction : public IAction
{
	Repository* p_Repos;
public:
	classCheckAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;

		p_Repos->inClassDef = true;
		size_t len = tc.find("class");
		p_Repos->currentClass = tc[len + 1];
	}
};

/////////////////////////////
// rule to ckeck namespace
class usingNamespaceCheck : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;

		if (tc.find("namespace") < tc.length())
		{
			doActions(pTc);
			return false;
		}
		return false;
	}
};

////////////////////////////////////
// action to handle namespace check
class usingNamespaceCheckAction : public IAction
{
	Repository* p_Repos;
public:
	usingNamespaceCheckAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;

		if (tc.find("using") < tc.length()) 
		{
			p_Repos->usingNamespace.push_back(tc[tc.length() - 2]);
			p_Repos->usingNamespaceScope.push_back(0);
		}

		size_t len = tc.find("{");
		if (len < tc.length())
		{
			p_Repos->scopePair.push_back(0);
			p_Repos->scopePair.back()++;
			p_Repos->myNamespace.push_back(tc[len - 1]);
		}
	}
};

/////////////////
// default rule
class depRule : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		doActions(pTc);
		return false;
	}
};

/////////////////
// default action
class depAction : public IAction
{
	Repository* p_Repos;
public:
	bool inNamespace(list<string> typeNamepaces, string cn)
	{
		list<string> tokenNamespace = p_Repos->usingNamespace;
		for (auto x : p_Repos->myNamespace) tokenNamespace.push_back(x);
		if (typeNamepaces.size() > 1)
		{
			for (auto x : typeNamepaces)
			{
				if (x == "default namespace") continue;
				if (cn == x) return true;		// namespace :: 
				for (auto y : tokenNamespace)
					if (x == y)
						return true;
					else
						continue;
			}
		} else 
			return true;
		return false;
	}
	depAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc){
		ITokCollection& tc = *pTc;
		DepValue testValue;
		std::map<string, TypeValue> *typeTable = p_Repos->mergedTypeTable;
		for (size_t i = 0; i < tc.length(); i++){
			std::map<string, TypeValue>::iterator my_Itr;
			for (my_Itr = typeTable->begin(); my_Itr != typeTable->end(); ++my_Itr){
				string k = my_Itr->first;
				TypeValue v = my_Itr->second;
				string token, checkNamespace;
				if (i > 1 && tc[i - 1] == "::") checkNamespace = tc[i - 2];
				if (i > 0 && tc[i - 1] == "~") 
					token = "~" + tc[i];
				else 
					token = tc[i];
				if (tc[i] == "operator") token = "operator" + tc[i + 1];
				if (token == k){
					list<string>::iterator it_type = v.type.begin();
					list<string>::iterator it_typeName = v.typeName.begin();
					list<string>::iterator it_inFile = v.inFile.begin();
					list<list<string>>::iterator it_namespace = v.namespaces.begin();
					for (size_t n = 0; n < v.inFile.size(); n++){
						if ((p_Repos->fileName[0] != *it_inFile) && inNamespace(*it_namespace, checkNamespace)) // in different file and right namespace
						{
							if (*it_type == "class" || *it_type == "struct" || *it_type == "enum" || *it_type == "typedef" || *it_type == "alias"){
								testValue.type.push_back(*it_type);
								testValue.typeName.push_back(*it_typeName);
								testValue.inFile.push_back(p_Repos->fileName[0]);
								testValue.depFile.push_back(*it_inFile);
								p_Repos->getDepMap()->insert(pair<string, DepValue>(k, testValue));
							}
							if (*it_type == "function" && (*it_typeName == p_Repos->currentClass || *it_typeName == "globe function") && i < tc.length() - 1 && tc[i + 1] == "("){
								testValue.type.push_back(*it_type);
								testValue.typeName.push_back(*it_typeName);
								testValue.inFile.push_back(p_Repos->fileName[0]);
								testValue.depFile.push_back(*it_inFile);
								p_Repos->getDepMap()->insert(pair<string, DepValue>(k, testValue));
							}
						}
						it_type++;
						it_typeName++;
						it_inFile++;
						it_namespace++;
					}
				}

			}
		}
	}
};

/////////////////////////////////////////
// rule to detect { or } in second pass
class scopeDepRule : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		if (pTc->find("}") < pTc->length() || pTc->find("{") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return true;
	}
};

//////////////////////////////////////////
// action to handle { or } in second pass
class scopeDepAction : public IAction
{
	Repository* p_Repos;
public:
	scopeDepAction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		if (pTc->find("{") < pTc->length())
		{
			//if (p_Repos->inClassDef) p_Repos->classScopePair++;
			p_Repos->usingNamespaceScope.back()++;

			if (!(pTc->find("namespace") < pTc->length())) p_Repos->scopePair.back()++;

			if (p_Repos->inClassDef) p_Repos->classScopePair++;
		}

		if (pTc->find("}") < pTc->length())
		{
			if (p_Repos->inClassDef)
			{
				p_Repos->classScopePair--;
				if (!p_Repos->classScopePair)
				{
					p_Repos->inClassDef = false;
					p_Repos->currentClass = "no class";
				}
			}
			p_Repos->usingNamespaceScope.back()--;
			if (p_Repos->usingNamespaceScope.back() == -1)
			{
				p_Repos->usingNamespaceScope.pop_back();
				p_Repos->usingNamespaceScope.back()--;
				p_Repos->usingNamespace.pop_back();
			}

			p_Repos->scopePair.back()--;
			if (!p_Repos->scopePair.back())
			{
				if (p_Repos->myNamespace.size() > 1) p_Repos->scopePair.pop_back();
				if (p_Repos->myNamespace.size() > 1) p_Repos->myNamespace.pop_back();
			}

		}
	}
};
#endif
