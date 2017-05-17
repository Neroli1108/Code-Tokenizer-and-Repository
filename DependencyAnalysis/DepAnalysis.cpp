///////////////////////////////////////////////////////////////////////
// DepAnalysis.cpp - analysis dependency of a file                   //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Cunyang Shui, Computer Engineering   		         //
//				cushui@syr.edu								         //
///////////////////////////////////////////////////////////////////////

#include "DepAnalysis.h"
// ------< doing dependency analysis >-----
void DepAnalysis::doAnalysis(map<string, TypeValue> *typeTable) {
	ConfigParseToConsole configure;
	configure.setMergedTypeTable(typeTable);
	Parser* pParser = configure.Build();
	configure.denyReturnFind();
	if (pParser) {
		if (!configure.Attach(path_))
			std::cout << "\n  could not open file " << path_ << std::endl;
	}
	else {
		std::cout << "\n\n  Parser not built\n\n";
	}

	while (pParser->next())
		pParser->parseDep();

	_depTable = pParser->getDepTable();
}

#ifdef TEST_DEPENDENCYANALYSIS
#include "../Utilities/Utilities.h"
#include "../FileMgr/FileMgr.h"
#include "../ParallelDependencyAnalysis/ParaDepAnalysis.h"

// ------< test stub >--------
int main()
{
	using namespace Scanner;

	StringHelper *ut = new StringHelper();
	ut->Title(" Test Dependency Analysis Package ", '=');

	ut->title(" dependency analysis start ");
	FileMgr fm("..\\Test");
	fm.addPattern("*.h");
	fm.addPattern("*.cpp");
	fm.search();
	std::vector<std::string> ds = fm.getStores();
	ParaDepAnalysis *para = new ParaDepAnalysis(ds);
	para->doParaTypeAnalysis();
	DepAnalysis depAna("..\\Test\\Test_1.h");
	depAna.doAnalysis(&(para->getTypeTable()));
	map<string, DepValue> table = *depAna.getTable();
	Display display;
	ut->title(" dependency result ");
	display.showDepTable(table);
	ut->putline();
}
#endif