#pragma once

#include "../libs.hpp"
#include "Data.hpp"
#include "../config/ServConfig.hpp"
#include "Response.hpp"

//Class to store the parsed request from the client
class	Request
{
	public:
		///////////////////////////////
		//Constructors and destructor//
		///////////////////////////////

		Request();
		~Request();

		//////////////////////
		//operators overload//
		//////////////////////

		Request								&operator=(Request const &src);
		friend std::ostream					&operator<<(std::ostream &out, Request const &src);
		std::string							operator[](std::string key);

		///////////
		//getters//
		///////////

		Data								&getBody();

		////////////////////
		//member functions//
		////////////////////

		bool								buildHeader(std::string rawHeader);
		bool								buildBody(std::string rawBody);
		bool								isHeader(std::string key);
		void								setCgiArgs(std::string key);
		bool								handle(ServConfig &server, Response &response);
		void								clear();


	private:
		std::string							_method;
		std::string							_uri;
		std::string							_httpVersion;

		std::map<std::string, std::string>	_headers;

		std::string							_rawBody;
		Data								_refinedBody;
};


//requests examples:

// GET /exemple.html HTTP/1.1
// Host: www.exemple.com
// User-Agent: MonNavigateur/1.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
// Accept-Language: fr,en-US;q=0.7,en;q=0.3
// Connection: keep-alive

// 	GET /cgi-bin/exemple.cgi?param1=valeur1&param2=valeur2 HTTP/1.1
// Host: www.exemple.com
// User-Agent: MonNavigateur/1.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
// Accept-Language: fr,en-US;q=0.7,en;q=0.3
// Connection: keep-alive
