#pragma once

#include "../libs.hpp"
#include "../SmartTypes.hpp"
#include "Codes.hpp"
#include "Route.hpp"

class Route;


// class to store the subserver configuration
// also used to store the socket fd of the subserver
class ServConfig
{
	public:
		/////////////////////////////////
		// Constructors and destructor //
		/////////////////////////////////

		ServConfig() {};
		~ServConfig() {};

		///////////////////////
		// operatos overload //
		///////////////////////

		friend std::ostream				&operator<<(std::ostream &out, ServConfig const &src);					
		ServConfig						&operator=(ServConfig const &src);

		/////////////
		// setters //
		/////////////

		void							setId(std::string id);
		void							setName(std::string name);
		in_port_t						setPort(const std::string &port);
		void							setHost(std::string host);
		void							setSocketFd(int socketFd);

		void							setIsDefault(std::string isDefault);
		void							setDefaultPage(std::string defaultPage);

		void							addRoute(Route &route);
		void							setErrors(Codes &errors);
		void							setDefaultErrorPage(std::string error);

		void							setMaxBodySize(int maxBodySize);
		void							setCookies(bool cookies);

		/////////////
		// getters //
		/////////////

		std::string						getId() const;
		std::string						getName() const;
		const sockaddr_in				&getAddr() const;
		int								getSocketFd() const;

		bool							getIsDefault() const;
		std::string						getDefaultPage() const;

		int								getMaxBodySize() const;
		bool							isCookies() const;

		bool							isRoute(std::string path) const;
		Route							getRoute(std::string path) const;

		std::map<std::string, Route>	getRoutes() const;
		Codes							getCodes() const;
		std::string						getErrorPage(int code) const;

		/////////////
		// checker //
		/////////////

		//bool							isValid();

		/////////////////////
		// member function //
		/////////////////////
		
		void						checkValidity();
		void						setMain(std::fstream &file, std::string &line);
		void						setError(std::fstream &file, std::string &line);
		void						setRoute(std::fstream &file, std::string &line);

	private:
		std::string						_id;
		std::string						_name;
		sockaddr_in						_addr;
		AutoFD							_socketFd;

		bool							_isDefault;
		std::string						_defaultPage;

		std::map<std::string, Route>	_routes;
		Codes							_errors;

		int								_maxBodySize;
		bool							_cookies;

		////////////////////////
		// forgoten variables //
		////////////////////////
		std::string				  		_port;
		std::string						_host;
		std::string						_defaultErrorPage;
};

void 						CerrExit(const char* message, std::string precision);
std::vector<ServConfig>		ServerParsing(std::string filename);
bool 						hasDuplicateAddress(const std::vector<ServConfig>&servers);
void						setServer(std::fstream &file, std::string &line, ServConfig &server);
