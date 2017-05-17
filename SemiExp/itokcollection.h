#ifndef ITOKCOLLECTION_H
#define ITOKCOLLECTION_H
/////////////////////////////////////////////////////////////////////
//  itokcollection.h - package for the ITokCollection interface    //
//  ver 1.3                                                        //
//  Language:      Visual Studio 2016							   //
//  Platform:      MacBook Pro, Windows 10						   //
//  Application:   CSE687 Pr3, Sp16								   //
//  Author:		   Cunyang Shui, Computer Engineering   		   //
//				   cushui@syr.edu								   //
//  Source:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Module Purpose:
  ===============
  ITokCollection is an interface that supports substitution of different
  types of scanners for parsing. 

  Maintenance History:
  ====================
  ver 3.0 :  8 Apr 16
  - modify prologe
  ver 1.3 : 14 Mar 16
  - add getBeginLine() and getEndLine() function
  ver 1.2 : 09 Fer 16
  - remove merge, 
  ver 1.1 : 02 Jun 11
  - added merge, remove overload, and default param in get
  ver 1.0 : 17 Jan 09
  - first release
*/
#include <string>

struct ITokCollection
{
  virtual bool get(bool clear=true)=0;
  virtual size_t length()=0;
  virtual std::string& operator[](size_t n)=0;
  //virtual SemiExp& operator=(const SemiExp&)=0;
  virtual size_t find(const std::string& tok)=0;
  virtual void push_back(const std::string& tok)=0;
  //virtual bool merge(const std::string& firstTok, const std::string& secondTok)=0;
  virtual bool remove(const std::string& tok)=0;
  virtual bool remove(size_t i)=0;
  virtual void toLower()=0;
  virtual void trimFront()=0;
  virtual void clear()=0;
  virtual std::string show(bool showNewLines=false)=0;
  virtual size_t getBeginLine() = 0;
  virtual size_t getEndLine() = 0;

  //virtual void show()=0;
  virtual ~ITokCollection() {};
};

#endif
