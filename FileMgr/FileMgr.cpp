/////////////////////////////////////////////////////////////////////
//  FileMgr.cpp - find files matching specified patterns           //
//                on a specified path                              //
//  ver 2.1                                                        //
//  Language:    Visual Studio 2016                                //
//  Platform:    MacBook Pro, Windows 10                           //
//  Author:      Cunyang Shui, Computer Engineering,			   //
//			     Syracuse University, cushui@syr.edu	           //
//  Source:      Jim Fawcett, Syracuse University, CST 4-187       //
//               jfawcett@twcny.rr.com                             //
/////////////////////////////////////////////////////////////////////

#include "FileMgr.h"
#include <iostream>

using patterns = std::vector<std::string>;

//----< add one search pattern >----------------------------
void FileMgr::addPattern(const std::string& patt){
	if (patterns_.size() == 1 && patterns_[0] == "*.*")
		patterns_.pop_back();
	patterns_.push_back(patt);
}

//----< process search >----------------------------
void FileMgr::search(){
	find(path_);
}

//----< find files >----------------------------
void FileMgr::find(const std::string& path){
	std::string fpath = path;
	//std::cout << "\n  ++ " << fpath;
	for (auto patt : patterns_)  // the scope of this for loop was
	{                            // changed to improve efficiency
		std::vector<std::string> files = FileSystem::Directory::getFiles(fpath, patt);
		for (auto f : files)
		{
			//std::cout << "\n  --   " << f;
			std::string full_f = fpath + "\\" + f;
			stores_.push_back(full_f);
		}
	}
	std::vector<std::string> dirs = FileSystem::Directory::getDirectories(fpath);
	for (auto d : dirs)
	{
		if (d == "." || d == "..")
			continue;
		std::string dpath = fpath + "\\" + d;  // here's the fix
		find(dpath);
	}
  }

//----< return find result >----------------------------
patterns FileMgr::getStores() {
	return stores_;
}

#ifdef TEST_FILEMGR

// ------< test stub >--------
int main()
{
  std::cout << "\n  Testing FileMgr";
  std::cout << "\n =================";

  //FileMgr fm("Z:\\Desktop\\CSE-687\\Project2 - Mac");
  FileMgr fm("..");

  fm.addPattern("*.h");
  fm.addPattern("*.cpp");
  fm.search();

  std::cout << "\n\n  contents of DataStore";
  std::cout << "\n -----------------------";
  std::vector<std::string> ds = fm.getStores();
  for (auto fs : ds)
  {
    std::cout << "\n  " << fs;
  }
  std::cout << "\n\n";
  return 0;
}
#endif
