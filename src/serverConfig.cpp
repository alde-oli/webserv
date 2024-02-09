#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"

#define LOCALHOST "127.0.0.1"
#define EXTERNAL_IP "11.10.14.5"

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
		this->_id = other._id;
		this->_serverName = other._serverName;
		this->_serverAddr = other._serverAddr;
		this->_isDefault = other._isDefault;
		this->_defaultPage = other._defaultPage;
		this->_maxBodySize = other._maxBodySize;
		this->_routes = other._routes;
		this->_errors = other._errors;
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
	out << "-Server cookies: " << server._cookies << std::endl;
	out << "-Server Routes: " << std::endl;
	for (std::map<std::string, Route>::const_iterator it = server._routes.begin(); it != server._routes.end(); it++)
		out << "\t" << it->second << std::endl;
	out << "-Server Errors: " << std::endl;
	out << server._errors << std::endl;
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
		std::cerr << "Error: invalid server ID line" << std::endl;
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
			this->_maxBodySize = setBodySize(line.substr(16));
		else if (!line.find("default = "))
			this->_isDefault = setBool(line.substr(10));
		else if (!line.find("cookies = "))
			this->_cookies = setBool(line.substr(10));
		else if (!line.find("name = "))
			this->_serverName = line.substr(7);
		else if (!line.find("default_error_page = "))
			this->_defaultPage = setFileStr(line.substr(21));
		else
		{
			std::cerr << "Error: invalid server line: " << line << std::endl;
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
			std::cerr << "Error: invalid error code: " << line << std::endl;
			exit (1);
		}
		this->_errors.addPage(std::atoi(line.substr(0, 3).c_str()), line.substr(6));
	}
}

void	ServerConfig::setRoute(std::fstream &file, std::string &line)
{
	std::string		key;
	std::string		value;

	if (line[0] != '[')
	{
		std::cerr << "Error: unexpected line" << line << std::endl;
		exit (1);
	}
	Route route = Route();
	route.setId(line.substr(line.find_last_of(':') + 1, line.length() - line.find_last_of(':') - 2));

	while (std::getline(file, line) && line[0] != '[')
	{
		if (line.empty())
			continue ;
		else if (!line.find("route = "))
			route.setRoute(line.substr(8));
		else if (!line.find("root = "))
			route.setRoot(line.substr(7));
		else if (!line.find("default_page = "))
			route.setPage(line.substr(15));
		else if (!line.find("methods = "))
			route.setMethods(line.substr(10));
		else if (!line.find("listing = "))
			route.setListing(line.substr(10));
		else if (!line.find("upload = "))
			route.setUpload(line.substr(9));
		else if (!line.find("force_upload = "))
			route.setForceUpload(line.substr(15));
		else if (!line.find("download = "))
			route.setDownload(line.substr(11));
		else if (!line.find("download_dir = "))
			route.setDownloadDir(line.substr(15));
		else if (!line.find("redir = "))
			route.setRedir(line.substr(8));
		else if (!line.find("redir_route = "))
			route.setRedirDir(line.substr(14));
		else if (!line.find("cgi = "))
			route.setCgi(line.substr(6));
		else
		{
			std::cerr << "Error: invalid route line: " << line << std::endl;
			exit (1);
		}
	}
	this->_routes[route.getRoute()] = route;

	std::string newRouteFormat = "[" + this->_id + ":ROUTES:";
	if (line.find(newRouteFormat) == 0 && line[line.length() - 1] == ']')
	{
		int colonCount = 0;
		for (size_t i = 0; i < line.length(); i++)
		{
			if (line[i] == ':')
				colonCount++;
		}
		if (colonCount == 2 && line.find_last_of(':') < line.length() - 2)
			setRoute(file, line);
		else
		{
			std::cerr << "Error: invalid route format" << std::endl;
			exit (1);
		}
	}
} 

void	ServerConfig::setFd(int fd)
{
	this->_fd = fd;
}


///////////
//getters//
///////////

const std::string	&ServerConfig::getId() const
{
	return this->_id;
}

int					ServerConfig::getFd() const
{
	return this->_fd;
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
		std::cerr << "Error: route not found: " << route << std::endl;
		exit (1);
	}
	return it->second;
}

bool				ServerConfig::hasRoute(const std::string &route) const
{
	return this->_routes.find(route) != this->_routes.end();
}

const std::string	&ServerConfig::getErrorPage(int errorCode) const
{
	return this->_errors.getPage(errorCode);
}


//////////////////
// verification //
//////////////////

void printErrorAndExit(std::string msg, int code)
{
	std::cerr << "Error: " << msg << std::endl;
	if (code)
		exit (code);
}

bool isLocalOrExternal(struct sockaddr_in addr) {
    // Convertit l'adresse IP de network byte order à dotted-decimal string
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ipstr, INET_ADDRSTRLEN);

    // Vérifie si l'adresse IP est localhost ou l'adresse IP externe connue
    if (strcmp(ipstr, LOCALHOST) == 0 || strcmp(ipstr, EXTERNAL_IP) == 0) {
        return true;
    }
    return false;
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
	if (isLocalOrExternal(_serverAddr) == false)
		printErrorAndExit("ServerAddr is not valid please insert a valid ServerAddr", 1);
	
	std::map<std::string, Route>::iterator it;

    for (it = _routes.begin(); it != _routes.end(); ++it)
	{
		it->second.verif();
    }
}
