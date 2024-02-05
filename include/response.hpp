#pragma once

#include "publiclib.hpp"
#include "Route.hpp"
#include "Error.hpp"
#include "Cgi.hpp"
#include "ServerConfig.hpp"

class Response
{
	public:
		void build(int code_erreur, std::string body, ServerConfig servConfig);
		void setResponse(std::string response);
		std::string getResponse(void);

	private:
		std::string response;
};