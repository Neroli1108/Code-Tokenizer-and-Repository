///////////////////////////////////////////////////////////////////////
// TypeAnalysis.h - analysis type definition of a file               //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
//  Author:		Cunyang Shui, Computer Engineering   		         //
//				cushui@syr.edu								         //
///////////////////////////////////////////////////////////////////////

#include "TypeAnalysis.h"
using namespace Scanner;

// ------< doing dependency analysis >-----
void TypeAnalysis::doAnalysis() {
	ConfigParseToConsole configure;
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
		pParser->parseType();

	_typeTable = pParser->getTypeMap();
}

// ------< test stub >--------
#ifdef TEST_TYPEANALYSIS
#include "../Display/Display.h"
int main(int argc, char* argv[]) {
	StringHelper *ut = new StringHelper();
	ut->Title(" Type analysis Package ", '=');
	ut->putline();
	TypeAnalysis typeAna("..\\Test\\Test_type.h");
	typeAna.doAnalysis();
	map<string, TypeValue> *table = typeAna.getTable();
	Display display;
	ut->title("  type table in ..\\Test\\Test_type.h");
	display.showTypeTable(*table);
	ut->putline();
	ut->putline();
	return 0;
}
#endif