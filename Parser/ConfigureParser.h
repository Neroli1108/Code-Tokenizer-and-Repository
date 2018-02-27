#ifndef CONFIGUREPARSER_H
#define CONFIGUREPARSER_H
/////////////////////////////////////////////////////////////////////
//  ConfigureParser.h - inherite IBuide interface, provide class   //
//						to connect parser, type table, all rules,  //
//						and all actions                            //
//  ver 2.3                                                        //
//  Language:    Visual Studio 2016                                //
//  Platform:    MacBook Pro, Windows 10                           //
//  Author:      Yunding Lu, Computer Engineering,			       //
//			     Syracuse University, yli327@syr.edu	           //
//  Source:      Jim Fawcett, Syracuse University, CST 4-187       //
//               jfawcett@twcny.rr.com                             //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module builds and configures parsers.  It builds the parser
parts and configures them with application specific rules and actions.

Public Interface:
=================
ConfigParseToConsole conConfig;
conConfig.Build();
conConfig.Attach(someFileName);

Build Process:
==============
Required files
- ConfigureParser.h, ConfigureParser.cpp, 
  Parser.h, Parser.cpp, itokecollection.h,
  RulesAndActions.h, RulesAndActions.cpp,
  SemiExp.h, SemiExp.cpp, 
  tokenizer.h, tokenizer.cpp

Build commands
- devenv Project3.sln /rebuild debug

Maintenance History:
====================
ver 2.3 : 8 Apr 2016
 - change ASTree to merged type table
ver 2.2 : 14 Mar 16
- delete all old rules and actions
- add 8 definition rules and 8 actions
- add deny display when detect a scope
ver 2.1 : 19 Feb 16
- Added PrintFunction action to FunctionDefinition rule
ver 2.0 : 01 Jun 11
- Major revisions to begin building a strong code analyzer
ver 1.1 : 01 Feb 06
- cosmetic changes to ConfigureParser.cpp
ver 1.0 : 12 Jan 06
- first release

*/

//
#include <fstream>
#include "Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "../RulesAndActions/RulesAndActions.h"

///////////////////////////////////////////////////////////////
// build parser that writes its output to console
class ConfigParseToConsole : IBuilder
{
public:
	ConfigParseToConsole() {};
	~ConfigParseToConsole();
	bool Attach(const std::string& name, bool isFile = true);	//attach input file path
	void denyReturnFind() { pRepo->showFind = false; }			//deny to display detected information
	void setMergedTypeTable(map<string, TypeValue>* table) { pMergedTypeTable = table; }
	Parser* Build();
private:
	map<string, TypeValue>* pMergedTypeTable;
	string pFileName;
	std::ifstream* pIn;
	Scanner::Toker* pToker;
	Scanner::SemiExp* pSemi;
	Parser* pParser;
	Repository* pRepo;

	scopeRule* pScopeRule;
	namespaceCheck* pNamespaceCheck;
	classType* pClassType;
	structType* pStructType;
	enumType* pEnumType;
	typedefType* pTypedefType;
	aliasType* pAliasType;
	functionType* pFunctionType;

	scopeAction* pScopeAction;
	namespaceCheckAction* pNamespaceCheckAction;
	classTypeAction* pClassTypeAction;
	structTypeAction* pStructTypeAction;
	enumTypeAction* pEnumTypeAction;
	typedefTypeAction* pTypedefTypeAction;
	aliasTypeAction* pAliasTypeAction;
	functionTypeAction* pFunctionTypeAction;

	depRule* pDepRule;
	depAction* pDepAction;
	classCheck* pClassCheck;
	classCheckAction* pClassCheckAction;
	usingNamespaceCheck* pUsingNamespaceCheck;
	usingNamespaceCheckAction* pUsingNamespaceCheckAction;
	scopeDepRule *pScopeDepRule;
	scopeDepAction *pScopeDepAction;

	void BuildNew();
	ConfigParseToConsole(const ConfigParseToConsole&) = delete;
	ConfigParseToConsole& operator=(const ConfigParseToConsole&) = delete;
};


#endif
