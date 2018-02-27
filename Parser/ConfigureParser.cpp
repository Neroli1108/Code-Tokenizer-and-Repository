/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - inherite IBuide interface, provide class //
//						to connect parser, type table, all rules,  //
//						and all actions                            //
//  ver 2.3                                                        //
//  Language:    Visual Studio 2016                                //
//  Platform:    MacBook Pro, Windows 10                           //
//  Author:      Yunding Li, Computer Engineering,			       //
//			     Syracuse University, yli327@syr.edu	           //
//  Source:      Jim Fawcett, Syracuse University, CST 4-187       //
//               jfawcett@twcny.rr.com                             //
/////////////////////////////////////////////////////////////////////

#include <fstream>
#include "ConfigureParser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "../RulesAndActions/RulesAndActions.h"

using namespace Scanner;

//----< destructor releases all parts >------------------------------
ConfigParseToConsole::~ConfigParseToConsole()
{
	delete pScopeRule;
	delete pNamespaceCheck;
	delete pClassType;
	delete pStructType;
	delete pEnumType;
	delete pTypedefType;
	delete pAliasType;
	delete pFunctionType;

	delete pScopeAction;
	delete pNamespaceCheckAction;
	delete pClassTypeAction;
	delete pStructTypeAction;
	delete pEnumTypeAction;
	delete pTypedefTypeAction;
	delete pAliasTypeAction;
	delete pFunctionTypeAction;

	delete pDepRule;
	delete pDepAction;
	delete pClassCheck;
	delete pClassCheckAction;
	delete pUsingNamespaceCheck;
	delete pUsingNamespaceCheckAction;
	delete pScopeDepRule;
	delete pScopeDepAction;

	delete pRepo;
	delete pParser;
	delete pSemi;
	delete pToker;
	pIn->close();
	delete pIn;
}

//----< attach toker to a file stream or stringstream >------------
bool ConfigParseToConsole::Attach(const std::string& name, bool isFile)
{
	if (pToker == 0)
		return false;
	pFileName = name;
	pIn = new std::ifstream(name);
	if (!pIn->good())
		return false;
	return pToker->attach(pIn);
}

//----< new pointer of each instance>----------------
void ConfigParseToConsole::BuildNew() {
	pNamespaceCheck = new namespaceCheck();
	pClassType = new classType();
	pStructType = new structType();
	pEnumType = new enumType();
	pTypedefType = new typedefType();
	pAliasType = new aliasType();
	pFunctionType = new functionType();
	pScopeRule = new scopeRule();
	pDepRule = new depRule();
	pClassCheck = new classCheck();
	pUsingNamespaceCheck = new usingNamespaceCheck();
	pScopeDepRule = new scopeDepRule();

	pNamespaceCheckAction = new namespaceCheckAction(pRepo);
	pClassTypeAction = new classTypeAction(pRepo);
	pStructTypeAction = new structTypeAction(pRepo);
	pEnumTypeAction = new enumTypeAction(pRepo);
	pTypedefTypeAction = new typedefTypeAction(pRepo);
	pAliasTypeAction = new aliasTypeAction(pRepo);
	pFunctionTypeAction = new functionTypeAction(pRepo);
	pScopeAction = new scopeAction(pRepo);
	pDepAction = new depAction(pRepo);
	pClassCheckAction = new classCheckAction(pRepo);
	pUsingNamespaceCheckAction = new usingNamespaceCheckAction(pRepo);
	pScopeDepAction = new scopeDepAction(pRepo);
}

//----< Here's where all the parts get assembled >----------------
Parser* ConfigParseToConsole::Build(){
	try{
		pToker = new Toker;
		pToker->returnComments(false);
		pSemi = new SemiExp(pToker);
		pParser = new Parser(pSemi);
		pRepo = new Repository(pToker);

		pParser->setTypeMap(pRepo->getTypeMap());
		pParser->setDepTable(pRepo->getDepMap());
		pRepo->fileName = &pFileName;
		pRepo->mergedTypeTable = pMergedTypeTable;
		BuildNew();
		pNamespaceCheck->addAction(pNamespaceCheckAction);
		pEnumType->addAction(pEnumTypeAction);
		pClassType->addAction(pClassTypeAction);
		pStructType->addAction(pStructTypeAction);
		pTypedefType->addAction(pTypedefTypeAction);
		pAliasType->addAction(pAliasTypeAction);
		pFunctionType->addAction(pFunctionTypeAction);
		pScopeRule->addAction(pScopeAction);

		pDepRule->addAction(pDepAction);
		pClassCheck->addAction(pClassCheckAction);
		pUsingNamespaceCheck->addAction(pUsingNamespaceCheckAction);
		pScopeDepRule->addAction(pScopeDepAction);

		pParser->addTypeRule(pNamespaceCheck);	//-----add types rules
		pParser->addTypeRule(pEnumType);
		pParser->addTypeRule(pClassType);
		pParser->addTypeRule(pStructType);
		pParser->addTypeRule(pTypedefType);
		pParser->addTypeRule(pAliasType);	
		pParser->addTypeRule(pFunctionType);
		pParser->addTypeRule(pScopeRule);
		pParser->addDepRule(pDepRule);			//-----add Dep rules
		pParser->addDepRule(pClassCheck);
		pParser->addDepRule(pUsingNamespaceCheck);
		pParser->addDepRule(pScopeRule);
		pParser->addDepRule(pScopeDepRule);
		return pParser;
	} catch (std::exception& ex) { 
		std::cout << "\n\n  " << ex.what() << "\n\n"; 
		return 0;
	}
}

#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

// ------< test stub >--------
int main(int argc, char* argv[])
{
	std::cout << "\n  Testing ConfigureParser module\n "
		<< std::string(32, '=') << std::endl;

	// collecting tokens from files, named on the command line

	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	for (int i = 1; i<argc; ++i)
	{
		std::cout << "\n  Processing file " << argv[i];
		std::cout << "\n  " << std::string(16 + strlen(argv[i]), '-');

		ConfigParseToConsole configure;
		Parser* pParser = configure.Build();
		try
		{
			if (pParser)
			{
				if (!configure.Attach(argv[i]))
				{
					std::cout << "\n  could not open file " << argv[i] << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;
			}
			// now that parser is built, use it

			while (pParser->next())
				pParser->parse();
			std::cout << "\n\n";
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
		std::cout << "\n\n";
	}
}

#endif
