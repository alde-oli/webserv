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
		friend std::ostream				&operator<<(std::ostream &out, const ServerConfig &server);

		void							setMain(std::fstream &file, std::string &line);
		void							setError(std::fstream &file, std::string &line);
		void							setCgi(std::fstream &file, std::string &line);
		void							setRoute(std::fstream &file, std::string &line);

		void							checkValidity();

		const std::string				&getId() const;
		const std::string				&getServerName() const;
		const sockaddr_in				&getServerAddr() const;
		bool							&isDefault() const;
		const std::string				&getDefaultPage() const;
		long long int					&getMaxBodySize() const;
		const Route						&getRoute(const std::string &route) const;
		const std::string				&getErrorPage(int errorCode) const;
		bool							isValidCgi(const std::string &extension) const;
		std::string						getCgiPath() const;

	private:
		const std::string				_id;
		std::string						_serverName;
		sockaddr_in						_serverAddr;
		bool							_isDefault;
		std::string						_defaultPage;
		long long int					_maxBodySize;

		std::map<std::string, Route>	_routes;
		Error							_errors;
		CGI								_cgi;
};