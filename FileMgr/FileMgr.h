#ifndef FILEMGR_H
#define FILEMGR_H
/////////////////////////////////////////////////////////////////////
//  FileMgr.h - find files matching specified patterns             //
//              on a specified path                                //
//  ver 2.1                                                        //
//  Language:    Visual Studio 2016                                //
//  Platform:    MacBook Pro, Windows 10                           //
//  Author:      Yunding li, Computer Engineering,			       //
//			     Syracuse University, yli327@syr.edu	           //
//  Source:      Jim Fawcett, Syracuse University, CST 4-187       //
//               jfawcett@twcny.rr.com                             //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module provides solutions to find files match a specified patttern
under a path.

Public Interface:
=================
FileMgr(const std::string& path)		
void addPattern(const std::string& patt);	
void search();								//after add pattern
void find(const std::string& path);			
patterns getStores();						//get result

Code Example:
==============
FileMgr fm("..");
fm.search();
std::vector<std::string> ds = fm1.getStores();

Build Process:
==============
Required files
- FileMgr.h, FileMgr.cpp, Utilities.h, Utilities.cpp
  FileSystem.h, FileSystem.cpp

 ver 2.1 : 14 Mar 2016
 - delete support file - DataStores
 - add a vector stores_ to store result
 - move functions to cpp file
 ver 2.0 : 12 Mar 2016
 - fixes bug in directory recursion in find(path)
 - reduces scope of for loop in find for efficiency
 ver 1.0 : 19 Feb 2016
 - first release
 */
#include "FileSystem.h"
#include <iostream>

using patterns = std::vector<std::string>;

class FileMgr
{
public:
  FileMgr(const std::string& path) : path_(path){ 
	  patterns_.push_back("*.*");
  }
  void addPattern(const std::string& patt);
  void search();
  void find(const std::string& path);
  patterns getStores();
private:
  std::string path_;
  patterns patterns_;	// search patterns
  patterns stores_;		// stored directory
};

#endif
