#pragma once
/////////////////////////////////////////////////////////////////////////
// MsgServer.cpp - Demonstrates simple one-way HTTP style messaging    //
//                 and file transfer                                   //
//                                                                     //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a server that receives HTTP style messages and
* files from multiple concurrent clients and simply displays the messages
* and stores files.
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
* - pull the receiving code into a Receiver class
* - Receiver should own a BlockingQueue, exposed through a
*   public method:
*     HttpMessage msg = Receiver.GetMessage()
* - You will start the Receiver instance like this:
*     Receiver rcvr("localhost:8080");
*     ClientHandler ch;
*     rcvr.start(ch);
*/
#include "../RemoteSupport/HttpMessage.h"
#include "../RemoteSupport/Sockets.h"
#include "../FileMgr/FileSystem.h"
#include "../ThreadPool/BlockingQueue.h"
#include "../RemoteSupport/Logger.h"
#include "../Utilities/Utilities.h"
#include "ServerRepository.h"
#include <string>
#include <iostream>

using Show = StaticLogger<1>;
using namespace Scanner;

/////////////////////////////////////////////////////////////////////
// ClientHandler class
/////////////////////////////////////////////////////////////////////
// - instances of this class are passed by reference to a SocketListener
// - when the listener returns from Accept with a socket it creates an
//   instance of this class to manage communication with the client.
// - You need to be careful using data members of this class
//   because each client handler thread gets a reference to this 
//   instance so you may get unwanted sharing.
// - I may change the SocketListener semantics (this summer) to pass
//   instances of this class by value.
// - that would mean that all ClientHandlers would need either copy or
//   move semantics.
//
class ClientHandler
{
	using EndPoint = std::string;
public:
	ClientHandler(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	HttpMessage makeMessage(size_t n, const std::string& body, const EndPoint& ep)
	{
		HttpMessage msg;
		HttpMessage::Attribute attrib;
		EndPoint myEndPoint = "localhost:8080";  // ToDo: make this a member of the sender
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
	void sendMessage(HttpMessage& msg, Socket& socket)
	{
		std::string msgString = msg.toString();
		socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
	}
	bool sendFile(const std::string& directory, const std::string& filename, Socket& socket)
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
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
};
//----< this defines processing to frame messages >------------------

HttpMessage ClientHandler::readMessage(Socket& socket){
	connectionClosed_ = false;
	HttpMessage msg;
	while (true){
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1){
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		} else
			break;
	}
	if (msg.attributes().size() == 0){
		connectionClosed_ = true;
		return msg;
	}
	if (msg.attributes()[0].first == "POST"){
		std::string filename = msg.findValue("file");
		if (filename != ""){
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;
			readFile(filename, contentSize, socket);
		}
		if (filename != ""){
			msg.removeAttribute("content-length");
			std::string bodyString = "<file>" + filename + "</file>";
			std::string sizeString = Converter<size_t>::toString(bodyString.size());
			msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
			msg.addBody(bodyString);
		} else {
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size()) {
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}
		}
	}
	return msg;
}
//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/
bool ClientHandler::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string fqname = "../ServerRcv/" + filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		Show::write("\n\n  can't open file " + fqname);
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}
//----< receiver functionality is defined by this function >---------

void ClientHandler::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\n  clienthandler thread is terminating");
			break;
		}
		msgQ_.enQ(msg);
	}
}

//----< test stub >--------------------------------------------------
#ifdef TEST_MSGSERVER
int main()
{
	::SetConsoleTitle("Server");

	Show::attach(&std::cout);
	Show::start();
	Show::title("\n  HttpMessage Server started");

	BlockingQueue<HttpMessage> msgQ;

	try
	{
		SocketSystem ss;
		SocketListener sl(8080, Socket::IP6);
		ClientHandler cp(msgQ);
		sl.start(cp);

		ServerRep server("repository");
		server.initRep();
		time_t timev(0);
		time(&timev);
		cout << server.timeConverter(timev) << endl;

		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			Show::write("\n\n  server recvd message contents:\n" + msg.bodyString());
			string message = msg.bodyString();
			if (message[1] == 'm') //msg, not file
			{
				size_t pst = message.find_first_of("-");
				std::string command = message;
				std::string data = message;
				command.erase(pst, command.size() - 1);
				command.erase(0, 5);
				data.erase(0, pst + 1);
				data.erase(data.size() - 6, data.size() - 1);
				::Sleep(200);
				cout << command << endl;
				cout << data << endl;

				if (command == "check in done")
				{
					Show::write("\n\nin check in\n");
					server.checkinDone(data);
				}
				if (command == "cancel check in")
					server.cancelCheckin(data);
				if (command == "get package list")
				{
					SocketConnecter si;
					while (!si.connect("localhost", 8081))
					{
						Show::write("\n client waiting to connect");
						::Sleep(100);
					}
					HttpMessage msg;
					for (auto pkg : server.getAllPkgs()) {
						std::string msgBody = "<msg>package list-" + pkg.fullName + "</msg>";
						msg = cp.makeMessage(1, msgBody, "localhost:8081");
						std::cout << "\ntry to send back" << std::endl;
						cp.sendMessage(msg, si);
					}
					std::string msgBody = "<msg>package list end-all</msg>";
					msg = cp.makeMessage(1, msgBody, "localhost:8081");
					std::cout << "\ntry to send back" << std::endl;
					cp.sendMessage(msg, si);
				}
				if (command == "download1")
				{
					SocketConnecter si;
					while (!si.connect("localhost", 8081))
					{
						Show::write("\n client waiting to connect");
						::Sleep(100);
					}
					FileMgr fm("repository\\" + data);
					fm.addPattern(data + ".*");
					fm.search();

					std::vector<std::string> ds = fm.getStores();

					for (auto f : ds)
					{
						std::string directory = f;
						std::string file = f;
						size_t slsh = f.find_last_of("\\");
						directory.erase(slsh + 1, directory.size() - 1);
						file.erase(0, slsh + 1);
						cp.sendFile(directory, file, si);

					}
					::Sleep(200);
					HttpMessage msg;
					std::string msgBody = "<msg>download1 end-" + data + "</msg>";
					msg = cp.makeMessage(1, msgBody, "localhost:8081");
					cp.sendMessage(msg, si);
				}
				if (command == "download2")
				{
					SocketConnecter si;
					while (!si.connect("localhost", 8081))
					{
						Show::write("\n client waiting to connect");
						::Sleep(100);
					}

					HttpMessage msg;
					for (auto pkg : server.getAllPkgs()) {
						if (pkg.fullName == data)
						{
							for (auto dep : pkg.depPkgs)
							{
								std::string msgBody = "<msg>download2-" + dep + "</msg>";
								msg = cp.makeMessage(1, msgBody, "localhost:8081");
								cp.sendMessage(msg, si);
							}
						}
					}
					std::string msgBody = "<msg>download2 end-all</msg>";
					msg = cp.makeMessage(1, msgBody, "localhost:8081");
					cp.sendMessage(msg, si);
				}
			}
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
#endif