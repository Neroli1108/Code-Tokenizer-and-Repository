#pragma once
/////////////////////////////////////////////////////////////////////////
// MsgClient.cpp - Demonstrates simple one-way HTTP messaging          //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages and stores files.
*
* It's purpose is to provide a very simple illustration of how to use
* the Socket Package provided for Project #4.
*/
/*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*/
/*
* ToDo:
* - pull the sending parts into a new Sender class
* - You should create a Sender like this:
*     Sender sndr(endPoint);  // sender's EndPoint
*     sndr.PostMessage(msg);
*   HttpMessage msg has the sending adddress, e.g., localhost:8080.
*/

#include "../RemoteSupport/HttpMessage.h"
#include "../RemoteSupport/Sockets.h"
#include "../FileMgr/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../RemoteSupport/Logger.h"
#include "../Utilities/Utilities.h"
#include "ReceiveHandle.h"
#include <string>
#include <iostream>
#include <thread>

using Show = StaticLogger<1>;
using namespace Scanner;
using Utils = StringHelper;

/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;

/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
class MsgClient
{
public:
	using EndPoint = std::string;
	void execute(std::string command, std::string data, BlockingQueue <Message> *aQ);
private:
	bool connectionClosed_;
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& directory, const std::string& fqname, Socket& socket);
	void download1(){
		BlockingQueue<HttpMessage> msgQ;
		SocketSystem ss;
		SocketListener sl(8081, Socket::IP6);
		ReceiveHandler cp(msgQ);
		sl.start(cp);
		while (true){
			HttpMessage httpMsg = msgQ.deQ();
			std::string msg = httpMsg.bodyString();
			if (msg[1] == 'm') {
				size_t pst = msg.find_first_of("-");
				std::string command2 = msg;
				std::string data2 = msg;
				command2.erase(pst, command2.size() - 1);
				command2.erase(0, 5);
				data2.erase(0, pst + 1);
				data2.erase(data2.size() - 6, data2.size() - 1);
				if (command2 == "download1 end"){
					std::string directory = "..\\ClientRcv\\" + data2;
					std::cout << directory << std::endl;
					const char *dir = directory.c_str();
					if (!CreateDirectory(dir, NULL)){
						std::string deleFile1 = "..\\ClientRcv\\" + data2 + ".h";
						std::string deleFile2 = "..\\ClientRcv\\" + data2 + ".cpp";
						std::string deleFile3 = "..\\ClientRcv\\" + data2 + ".xml";
						std::remove(deleFile1.c_str());
						std::remove(deleFile2.c_str());
						std::remove(deleFile3.c_str());
					}

					FileMgr fm("..\\ClientRcv");
					fm.addPattern(data2 + ".*");
					fm.search();
					std::vector<std::string> ds = fm.getStores();
					for (auto fromFile : ds){
						std::string affix = fromFile;
						size_t pos = fromFile.find_last_of(".");
						affix.erase(0, pos);
						std::string toFile = "..\\ClientRcv\\" + data2 + "\\" + data2 + affix;
						MoveFile(fromFile.c_str(), toFile.c_str());
					}
					break;
				}
			}
		}
	}
};
//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage MsgClient::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8081";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}
//----< send message using socket >----------------------------------

void MsgClient::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< send file using socket >-------------------------------------
/*
* - Sends a message to tell receiver a file is coming.
* - Then sends a stream of bytes until the entire file
*   has been sent.
* - Sends in binary mode which works for either text or binary.
*/
bool MsgClient::sendFile(const std::string& directory, const std::string& filename, Socket& socket)
{
	// assumes that socket is connected

	std::string fqname = directory + filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost::8080");
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}
//----< this defines the behavior of the client >--------------------

void MsgClient::execute(std::string command, std::string data, BlockingQueue <Message> *aQ)
{
	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Scanner::Converter<size_t>::toString(myCount);
	Show::attach(&std::cout);
	Show::start();

	try{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080)){
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		HttpMessage msg;
		std::string msgBody = "<msg>" + command + "-" + data + "</msg>";
		msg = makeMessage(1, msgBody, "localhost:8080");
		sendMessage(msg, si);
		Show::write("\n\n  client" + myCountString + " sent\n" + msg.toIndentedString());

		if (command == "check in"){
			std::string directory = data;
			std::string file = data;
			size_t slsh = data.find_last_of("\\");
			directory.erase(slsh + 1, directory.size() - 1);
			file.erase(0, slsh + 1);
			std::cout << "\n\n" << directory << std::endl;
			std::cout << file << std::endl;
			Show::write("\n\n  sending file " + file);
			sendFile(directory, file, si);
		}
		if (command == "get package list") {
			BlockingQueue<HttpMessage> msgQ;
			SocketSystem ss;
			SocketListener sl(8081, Socket::IP6);
			ReceiveHandler cp(msgQ);
			sl.start(cp);
			while (true)
			{
				HttpMessage httpMsg = msgQ.deQ();
				std::string msg = httpMsg.bodyString();
				size_t pst = msg.find_first_of("-");
				std::string command2 = msg;
				std::string data2 = msg;
				command2.erase(pst, command2.size() - 1);
				command2.erase(0, 5);
				data2.erase(0, pst + 1);
				data2.erase(data2.size() - 6, data2.size() - 1);

				if (command2 == "package list end") break;
				std::cout << "receive message in Client Agent :" << msg <<std::endl;
				aQ->enQ(command2 + "-" + data2);
 			}
			// hold on and waiting for message
		}
		if (command == "download1") {
			download1();
		}
		if (command == "download2") {
			BlockingQueue<HttpMessage> msgQ;
			SocketSystem ss;
			SocketListener sl(8081, Socket::IP6);
			ReceiveHandler cp(msgQ);
			sl.start(cp);
			while (true)
			{
				HttpMessage httpMsg = msgQ.deQ();
				std::string msg = httpMsg.bodyString();
				size_t pst = msg.find_first_of("-");
				std::string command2 = msg;
				std::string data2 = msg;
				command2.erase(pst, command2.size() - 1);
				command2.erase(0, 5);
				data2.erase(0, pst + 1);
				data2.erase(data2.size() - 6, data2.size() - 1);

				if (command2 == "download2 end") break;
				std::cout << "receive message in Client Agent :" << msg << std::endl;
				aQ->enQ("download1-" + data2);
			}
		}
		if (command == "message")
		{
			msgBody = "<msg>I am Http style message body</msg>";
			msg = makeMessage(1, msgBody, "localhost:8080");
			aQ->enQ("message-" + msg.toString());
		}
		Show::write("\n");
		Show::write("\n  All done folks");
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}