#include "publiclib.hpp"
#include "ServerConfig.hpp"
#include "Error.hpp"
#include "parsing.hpp"

///////////////////////////////
//constructors and destructor//
///////////////////////////////

ServerConfig::ServerConfig()
	: _id(""), _serverName(""), _isDefault(false), _defaultPage(""), _maxBodySize(0)
{
}

ServerConfig::ServerConfig(const ServerConfig &other)
{
	*this = other;
}

ServerConfig::ServerConfig(std::string id)
	: _id(id), _serverName(""), _isDefault(false), _defaultPage(""), _maxBodySize(0)
{
	this->_serverAddr.sin_family = AF_INET;
	this->_serverAddr.sin_addr.s_addr = setIPv4("255.255.255.255");
	this->_serverAddr.sin_port = htons(static_cast<uint16_t>(-1));
}

ServerConfig::~ServerConfig()
{
}


//////////////////////
//operators overload//
//////////////////////

ServerConfig	&ServerConfig::operator=(const ServerConfig &other)
{
	if (this != &other)
	{
		this->_serverName = other._serverName;
		this->_serverAddr = other._serverAddr;
		this->_isDefault = other._isDefault;
		this->_defaultPage = other._defaultPage;
		this->_maxBodySize = other._maxBodySize;
		this->_routes = other._routes;
		this->_errors = other._errors;
		this->_cgi = other._cgi;
	}
	return *this;
}

std::ostream	&operator<<(std::ostream &out, const ServerConfig &server)
{
	out << "o~~~~~~~~~~~~~~server~~~~~~~~~~~~~~o" << std::endl;
	out << "-Server ID: " << server._id << std::endl;
	out << "-Server Name: " << server._serverName << std::endl;
	out << "-Server Address: " << inet_ntoa(server._serverAddr.sin_addr) << std::endl;
	out << "-Server Port: " << ntohs(server._serverAddr.sin_port) << std::endl;
	out << "-Server Default Page: " << server._defaultPage << std::endl;
	out << "-Server Max Body Size: " << server._maxBodySize << std::endl;
	out << "-Server is Default: " << server._isDefault << std::endl;
	out << "-Server Routes: " << std::endl;
	for (std::map<std::string, Route>::const_iterator it = server._routes.begin(); it != server._routes.end(); it++)
		out << "\t" << it->second << std::endl;
	out << "-Server Errors: " << std::endl;
	out << server._errors << std::endl;
	out << "-Server CGI: " << std::endl;
	out << server._cgi << std::endl;
	out << "o~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~o" << std::endl;
	return out;
}

//////////////////
// server parser//
//////////////////

void	ServerConfig::setMain(std::fstream &file, std::string &line)
{
	std::string		key;
	std::string		value;

	if (line.empty() || line[0] != '[' || line[line.length() - 1] != ']')
	{
		std::cout << "Error: invalid server ID line" << std::endl;
		exit(1);
	}
	this->_id = line.substr(1, line.length() - 2);

	while (std::getline(file, line) && line[0] != '[')
	{
		if (line.empty())
			continue ;
		if (!line.find("port = "))
			this->_serverAddr.sin_port = setPort(line.substr(7));
		else if (!line.find("host = "))
			this->_serverAddr.sin_addr.s_addr = setIPv4(line.substr(7));
		else if (!line.find("server_name = "))
			this->_serverName = line.substr(14);
		else if (!line.find("default_page = "))
			this->_defaultPage = setFileStr(line.substr(15));
		else if (!line.find("max_body_size = "))
			this->_maxBodySize = setBool(line.substr(16));
		else if (!line.find("is_default = "))
			this->_isDefault = setBool(line.substr(13));
		else
		{
			std::cout << "Error: invalid server line: " << line << std::endl;
			exit (1);
		}
	}
}


void	ServerConfig::setError(std::fstream &file, std::string &line)
{
	std::string		key;
	std::string		value;

	while (std::getline(file, line) && line[0] != '[')
	{
		if (line.empty())
			continue ;
		if (line.length() < 6 || !std::isdigit(line[0]) || !std::isdigit(line[1]) || !std::isdigit(line[2]) || line[3] != ' ' || line[4] != '=' || line[5] != ' ')
		{
			std::cout << "Error: invalid error code: " << line << std::endl;
			exit (1);
		}
		this->_errors.addPage(std::atoi(line.substr(0, 3).c_str()), line.substr(6));
	}
}


void	ServerConfig::setCgi(std::fstream &file, std::string &line)
{
	std::string		key;
	std::string		value;

	while (std::getline(file, line) && line[0] != '[')
	{
		if (line.empty())
			continue ;
		if (!line.find("extension = "))
			this->_cgi.addExtension(line.substr(12));
		else if (!line.find("path = "))
			this->_cgi.addPath(line.substr(7));
		else
		{
			std::cout << "Error: invalid cgi line: " << line << std::endl;
			exit (1);
		}
	}
}


void	ServerConfig::setRoute(std::fstream &file, std::string &line)
{
	std::string		key;
	std::string		value;

	if (line[0] != '[')
	{
		std::cout << "Error: unexpected line" << line << std::endl;
		exit (1);
	}
	Route route(line.substr(line.find_last_of(':') + 1, line.length() - line.find_last_of(':') - 2));

	while (std::getline(file, line) && line[0] != '[')
	{
		if (line.empty())
			continue ;
		else if (!line.find("route = "))
			route.setRoute(line.substr(8));
		else if (!line.find("root = "))
			route.setRoot(line.substr(7));
		else if (!line.find("default_page = "))
			route.setPage(line.substr(9));
		else if (!line.find("methods = "))
			route.setMethods(line.substr(10));
		else if (!line.find("listing = "))
			route.setListing(line.substr(10));
		else if (!line.find("download = "))
			route.setDownload(line.substr(11));
		else if (!line.find("download_dir = "))
			route.setDownloadDir(line.substr(17));
		else if (!line.find("redir  = "))
			route.setRedir(line.substr(14));
		else if (!line.find("redir_route = "))
			route.setRedirDir(line.substr(14));
		else
		{
			std::cout << "Error: invalid route line: " << line << std::endl;
			exit (1);
		}
	}
	this->_routes[route.getRoute()] = route;

	std::string newRouteFormat = "[" + this->_id + ":ROUTES:";
	if (line.find(newRouteFormat) == 0 && line[line.length() - 1] == ']')
	{
		int colonCount = 0;
		for (char c : line)
		{
			if (c == ':')
				colonCount++;
		}
		if (colonCount == 2 && line.find_last_of(':') < line.length() - 2)
			setRoute(file, line);
		else
		{
			std::cout << "Error: invalid route format" << std::endl;
			exit (1);
		}
	}
} 


///////////
//getters//
///////////

const std::string	&ServerConfig::getId() const
{
	return this->_id;
}

const std::string	&ServerConfig::getServerName() const
{
	return this->_serverName;
}

const sockaddr_in	&ServerConfig::getServerAddr() const
{
	return this->_serverAddr;
}

bool				ServerConfig::isDefault() const
{
	return this->_isDefault;
}

const std::string	&ServerConfig::getDefaultPage() const
{
	return this->_defaultPage;
}

long long int		ServerConfig::getMaxBodySize() const
{
	return this->_maxBodySize;
}

const Route			&ServerConfig::getRoute(const std::string &route) const
{
	std::map<std::string, Route>::const_iterator it = this->_routes.find(route);
	if (it == this->_routes.end())
	{
		std::cout << "Error: route not found: " << route << std::endl;
		exit (1);
	}
	return it->second;
}

const std::string	&ServerConfig::getErrorPage(int errorCode) const
{
	return this->_errors.getPage(errorCode);
}

bool				ServerConfig::isValidCgi(const std::string &extension) const
{
	return this->_cgi.isValidCgi(extension);
}

std::string			&ServerConfig::getCgiPath() const
{
	return this->_cgi.getPath();
}

//////////////////
// verification //
//////////////////

void printErrorAndExit(char *msg, int code)
{
	std::cerr << "Error: " << msg << std::endl;
	if (code)
		exit (code);
}

void	ServerConfig::checkValidity()
{
	if (_id.empty())
		printErrorAndExit("ID is empty please insert a valid ID", 1);
	else if (_serverName.empty())
		printErrorAndExit("ServerName is empty please insert a valid ServerName", 1);
	else if (_defaultPage.empty())
		printErrorAndExit("DefaultPage is empty please insert a valid DefaultPage", 1);
	else if (_maxBodySize <= 0)
		printErrorAndExit("MaxBodySize is empty please insert a valid MaxBodySize", 1);
	
	std::map<std::string, Route>::iterator it;

    for (it = _routes.begin(); it != _routes.end(); ++it)
	{
		it->second.verif();
    }
}
