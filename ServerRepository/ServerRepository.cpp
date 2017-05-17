#ifdef TEST_SERVERREP
#include "ServerRepository.h"

int main(int argc, char* argv[]) {
	StringHelper *ut = new StringHelper();
	
	CreateDirectory("repository\\dir1", NULL);
	ServerRep server("repository");
	vector<string> depPkgs;
	depPkgs.push_back("dependent_f1");
	depPkgs.push_back("dependent_f2");

	time_t timev(0);
	time(&timev);
	//server.createMetadateXML("repository", "pkg1", depPkgs, timev, "2016-01-01-01-23-45", "client1");
	cout << server.timeConverter(timev) << endl;

	server.initRep();
	cout << "\n\n" << endl;

	//rename("repository\\oldname", "repository\\newname");
	
	//ut->Title("Testing XmlParser", '=');
	//std::cout << "\n";
	//std::string src = "repository/Test_1_(time)/Test_1.xml";
	//XmlParser parser(src);
	//parser.verbose(); //false to not display details
	//ut->title("Compressed form of formatted XML:");
	//std::cout << "\n" << src << "\n";
	//ut->title("Parsing compressed XML:");
	//XmlDocument* pDoc = parser.buildDocument();
	//ut->title("Resulting XML Parse Tree:");
	//std::cout << "\n" << pDoc->toString();
	//std::cout << "\n\n";

	//pDoc->save("repository/xmlOutput.xml");

	return 0;
}

#endif