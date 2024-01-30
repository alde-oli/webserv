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

		void							setServerName(const std::string &serverName);
		void							setServerAddr(const sockaddr_in &serverAddr);
		void							setIsDefault(bool isDefault);
		void							setDefaultPage(const std::string &defaultPage);
		void							setMaxBodySize(int maxBodySize);
		void							addRoute(const Route &route, const std::string &routeName);
		void							addError(int errorCode, const std::string &errorPage);
		void							addCgiExtension(const std::string &extension);
		void							addCgiPath(const std::string &path);

		const std::string				&getServerName() const;
		const sockaddr_in				&getServerAddr() const;
		bool							getIsDefault() const;
		const std::string				&getDefaultPage() const;
		int								getMaxBodySize() const;

		std::string						getRouteRoot(std::string route) const;
		std::string						getRoutePage(std::string route) const;
		std::vector<std::string>		getRouteMethods(std::string route) const;

		std::string						getErrorPage(int errorCode) const;

		const std::vector<std::string	&getCgiExtensions() const;
		const std::vector<std::string>	&getCgiPaths() const;


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