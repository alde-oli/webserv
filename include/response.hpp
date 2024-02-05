#pragma once

#include "publiclib.hpp"
#include "Route.hpp"
#include "Error.hpp"
#include "Cgi.hpp"
#include "ServerConfig.hpp"

class Response
{
	public:
		void 		build(int code_erreur, std::string body, ServerConfig servConfig, std::string ContentType);
		void 		setResponse(std::string response);
		std::string getResponse(void);
		std::string caring200(int code_erreur);
		std::string caring300(int code_erreur);
		std::string caring400(int code_erreur);
		std::string caring500(int code_erreur);

	private:
		std::string response;
};