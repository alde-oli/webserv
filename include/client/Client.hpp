#pragma once

#include "../libs.hpp"

#include "../SmartTypes.hpp"
#include "Request.hpp"
#include "Response.hpp"

//Client class to store client information and handle client events
class Client
{
	public:
		///////////////////////////////
		//Constructors and destructor//
		///////////////////////////////
		Client(){};
		Client(int clientFd, sockaddr_in ClientAddr, int serverFd);
		~Client();

		//////////////////////
		//operators overload//
		//////////////////////

		Client					&operator=(const Client &c);
		friend std::ostream&	operator<<(std::ostream& os, const Client& c);

		///////////
		//setters//
		///////////

		void					updateActivity();
		void					setClientFd(int fd);

		///////////
		//getters//
		///////////

		int						getServFd();
		sockaddr_in				getAddr();
		std::time_t				getLastActivity();
		void					printRequest();

		////////////////////
		//member functions//
		////////////////////

	bool					read(ServConfig &server, std::vector<struct pollfd> &pollfds);
	bool					write(std::vector<struct pollfd> &pollfds);

	bool					setWriteEvent(std::vector<struct pollfd> &pollfds);
	bool					unsetWriteEvent(std::vector<struct pollfd> &pollfds);

		void					handleCookies(sockaddr_in addr, std::string hostname);

		bool					isResponse();


	private:
		AutoFD					_clientFd;
		int						_serverFd;
		sockaddr_in				_clientAddr;
		std::time_t				_lastActivity;

		std::string				_rawRequest;
		bool					_EOHFound;
		size_t					_bodyToRead;
		

		Request					_request;

		Response				_response;

		bool					_writeEventSet;
};
