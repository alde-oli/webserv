#pragma once

#include "publiclib.hpp"
#include "Route.hpp"
#include "Error.hpp"
#include "Cgi.hpp"

class ServerConfig
{
	public:
		ServerConfig(std::string id);
		ServerConfig(const ServerConfig &other);
		~ServerConfig();

		ServerConfig					&operator=(const ServerConfig &other);

		void							setMain(std::fstream &file);
		void							setError(std::fstream &file);
		void							setCgi(std::fstream &file);
		void							setRoute(std::fstream &file);

	private:
		const std::string				_id;
		std::string						_serverName;
		sockaddr_in						_serverAddr;
		bool							_isDefault;
		std::string						_defaultPage;
		int								_maxBodySize;

		std::map<std::string, Route>	_routes;
		Error							_errors;
		CGI								_cgi;
};