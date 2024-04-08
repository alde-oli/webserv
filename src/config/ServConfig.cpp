#include "../../include/config/ServConfig.hpp"
#include "../../include/config/ParsingUtils.hpp"
#include "../../include/config/ParsingExec.hpp"

/////////////////
// [ GETTERS ] //
/////////////////

std::string ServConfig::getName() const
{
	return (this->_name);
}

std::string	ServConfig::getId() const
{
	return (this->_id);
}

const sockaddr_in	&ServConfig::getAddr() const
{
	return (this->_addr);
}

int			ServConfig::getSocketFd() const
{
	return (this->_socketFd.get());
}

bool		ServConfig::getIsDefault() const
{
	return (this->_isDefault);
}

std::string	ServConfig::getDefaultPage() const
{
	return (this->_defaultPage);
}

int			ServConfig::getMaxBodySize() const
{
	return (this->_maxBodySize);
}

bool		ServConfig::isCookies() const
{
	return (this->_cookies);
}

// Verifier le chemin 
bool		ServConfig::isRoute(std::string path) const
{
	std::map<std::string, Route>::const_iterator it = this->_routes.find(path);
	if (it == this->_routes.end())
		return (false);
	return (true);
}

Route		ServConfig::getRoute(std::string path) const
{
	std::map<std::string, Route>::const_iterator it = this->_routes.find(path);
	if (it == this->_routes.end())
		{CERRANDEXIT CerrExit("[ ERROR ] Route not found: ", path);}
	return it->second;
}

std::map<std::string, Route>	ServConfig::getRoutes() const
{
	return (this->_routes);
}

Codes	ServConfig::getCodes() const
{
	return (this->_errors);
}

// Return the good path of the error page
std::string		ServConfig::getErrorPage(int code) const
{
	return (this->_errors.getErrPage(code));
}

/////////////////
// [ SETTERS ] //
/////////////////

void	ServConfig::setId(std::string id)
{
	this->_id = id;
}

void	ServConfig::setName(std::string name)
{
	this->_name = name;
}

in_port_t	ServConfig::setPort(const std::string &port)
{
	in_port_t	inetPort;

	inetPort = htons(atoi(port.c_str()));
	if (inetPort == 0)
		{CERRANDEXIT CerrExit("Error: setPort() failed for: ", port);}
	return inetPort;
}

void	ServConfig::setHost(std::string host)
{
	this->_host = host;
}

void	ServConfig::setSocketFd(int socketFd)
{
	this->_socketFd.set(socketFd);
}

void	ServConfig::setIsDefault(std::string isDefault)
{
	if (isDefault == "true" || isDefault == "TRUE")
		this->_isDefault = true;
	else if (isDefault == "false" || isDefault == "FALSE")
		this->_isDefault = false;
	else
		{CERRANDEXIT CerrExit("[ ERROR ] Invalid value for default: ", isDefault);}
}

void	ServConfig::setDefaultPage(std::string defaultPage)
{
	this->_defaultPage = defaultPage;
}

void	ServConfig::addRoute(Route &route)
{
	this->_routes[route.getId()] = route;
}

void	ServConfig::setDefaultErrorPage(std::string error)
{
	this->_defaultErrorPage = error;
}

void	ServConfig::setErrors(Codes &errors)
{
	this->_errors = errors;
}

void	ServConfig::setMaxBodySize(int maxBodySize)
{
	this->_maxBodySize = maxBodySize;
}

void	ServConfig::setCookies(bool cookies)
{
	this->_cookies = cookies;
}

////////////////////////
// operators overload //
////////////////////////

std::ostream	&operator<<(std::ostream &out, ServConfig const &src)
{
	out << "Server ID: " << src.getId() << std::endl;
	out << "Server Name: " << src.getName() << std::endl;
	out << "Server Address: " << inet_ntoa(src.getAddr().sin_addr) << ":" << ntohs(src.getAddr().sin_port) << std::endl;
	out << "Server Socket: " << src.getSocketFd() << std::endl;
	out << "Is Default: " << src.getIsDefault() << std::endl;
	out << "Default Page: " << src.getDefaultPage() << std::endl;
	out << "Max Body Size: " << src.getMaxBodySize() << std::endl;
	out << "Cookies: " << src.isCookies() << std::endl;
	out << "Routes: " << std::endl;
	std::map<std::string, Route> routes = src.getRoutes();
	for (std::map<std::string, Route>::iterator it = routes.begin(); it != routes.end(); ++it)
	{
		out << it->second << std::endl;
	}
	out << "Errors: " << std::endl;
	out << src.getCodes() << std::endl;
	return out;
}

ServConfig	&ServConfig::operator=(const ServConfig &other)
{
	if (this != &other)
	{
		this->_id = other._id;
		this->_name = other._name;
		this->_addr = other._addr;
		this->_socketFd.set(other._socketFd.get());
		this->_isDefault = other._isDefault;
		this->_defaultPage = other._defaultPage;
		this->_routes = other._routes;
		this->_errors = other._errors;
		this->_maxBodySize = other._maxBodySize;
		this->_cookies = other._cookies;
		this->_port = other._port;
		this->_host = other._host;
		this->_defaultErrorPage = other._defaultErrorPage;
	}
	return *this;
}

// ------------------ [ Main functions of the class ] ------------------ //

void	ServConfig::setRoute(std::fstream &file, std::string &line)
{
	std::string		key;
	std::string		value;

	if (line[0] != '[')
		{CERRANDEXIT CerrExit("unexpected line: ",line);}
	Route route = Route();
	route.setId(line.substr(line.find_last_of(':') + 1, line.length() - line.find_last_of(':') - 2));

	while (std::getline(file, line) && line[0] != '[')
	{
		if (line.empty())
			continue ;
		else if (!line.find("path = "))
			route.setPath(line.substr(7));
		else if (!line.find("root = "))
			route.setRoot(line.substr(7));
		else if (!line.find("default_page = "))
			route.setDefaultPage(line.substr(15));
		else if (!line.find("methods = "))
			route.setMethods(line.substr(10));
		else if (!line.find("listing = "))
			route.setListing(line.substr(10));
		else if (!line.find("upload = "))
			route.setIsUpload(line.substr(9));
		else if (!line.find("force_upload = "))
			route.setForceUpload(line.substr(15));
		else if (!line.find("download = "))
			route.setIsDownload(line.substr(11));
		else if (!line.find("download_dir = "))
			route.setDownloadDir(line.substr(15));
		else if (!line.find("redir = "))
			route.setIsRedir(line.substr(8));
		else if (!line.find("redir_route = "))
			route.setRedirDir(line.substr(14));
		else if (!line.find("cgi = "))
			route.setCgiExt(line.substr(6));
		else
			{CERRANDEXIT CerrExit("invalid route line: ", line);}
	}
	this->_routes[route.getPath()] = route;

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
			{CERRANDEXIT CerrExit("Error: invalid route format", "");}
	}
} 

void	ServConfig::setError(std::fstream &file, std::string &line)
{
	std::string		key;
	std::string		value;

	while (std::getline(file, line) && line[0] != '[')
	{
		if (line.empty())
			continue ;
		if (line.length() < 6 || !std::isdigit(line[0]) || !std::isdigit(line[1]) || !std::isdigit(line[2]) || line[3] != ' ' || line[4] != '=' || line[5] != ' ')
			{CERRANDEXIT CerrExit("invalid error code: ", line);}
		this->_errors.addErrPage(std::atoi(line.substr(0, 3).c_str()), line.substr(6));
	}
}

void	ServConfig::setMain(std::fstream &file, std::string &line)
{
	std::string		key;
	std::string		value;

	if (line.empty() || line[0] != '[' || line[line.length() - 1] != ']')
		{CERRANDEXIT CerrExit("Error: invalid server ID line: ", line);}
	this->_id = line.substr(1, line.length() - 2);

	while (std::getline(file, line) && line[0] != '[')
	{
		if (line.empty())
			continue ;
		if (!line.find("port = "))
			this->_addr.sin_port = setPort(line.substr(7));
		else if (!line.find("host = "))
			this->_addr.sin_addr.s_addr = setIPv4(line.substr(7));
		else if (!line.find("server_name = "))
			this->_id = line.substr(14);
		else if (!line.find("default_page = "))
			this->_defaultPage = setFileStr(line.substr(15));
		else if (!line.find("max_body_size = "))
			this->_maxBodySize = setBodySize(line.substr(16));
		else if (!line.find("default = "))
			this->_isDefault = setBool(line.substr(10));
		else if (!line.find("cookies = "))
			this->_cookies = setBool(line.substr(10));
		else if (!line.find("name = "))
			this->_name = line.substr(7);
		else if (!line.find("default_error_page = "))
			this->_defaultPage = setFileStr(line.substr(21));
		else
			{CERRANDEXIT CerrExit("Error: invalid server line: ", line);}
	}
}

void	ServConfig::checkValidity()
{
	if (_id.empty())
		{CERRANDEXIT CerrExit("Error : ID is empty please insert a valid ID", "");}
	else if (_name.empty())
		{CERRANDEXIT CerrExit("Error : ServerName is empty please insert a valid ServerName", "");}
	else if (_defaultPage.empty())
		{CERRANDEXIT CerrExit("Error : DefaultPage is empty please insert a valid DefaultPage", "");}
	else if (_maxBodySize <= 0)
		{CERRANDEXIT CerrExit("Error : MaxBodySize is empty please insert a valid MaxBodySize", "");}
	if (isLocalOrExternal(_addr) == false)
		{CERRANDEXIT CerrExit("Error : ServerAddr is not valid please insert a valid ServerAddr", "");}
	
	std::map<std::string, Route>::iterator it;

    for (it = _routes.begin(); it != _routes.end(); ++it)
	{
		it->second.verif();
	}
}


// Main function to parse the server configuration
std::vector<ServConfig>		ServerParsing(std::string filename)
{
	std::fstream file(filename);
	
	if (!file.is_open())
		{CERRANDEXIT CerrExit("Error: Failed to open file: ", filename);}
	
	std::vector<ServConfig> servers;
	std::string line;
	while (getline(file, line))
	{
		ServConfig server = ServConfig();
		if (line.empty())
			continue;
		else if (line[0] == '[' && line[line.size() - 1] == ']')
		{
			if (line.find(":") != std::string::npos)
				{CERRANDEXIT CerrExit("Error: bad server name: ", line);}
			while (line[0] == '[' && countDeuxPoints(line) == 0)
			{
				server = ServConfig();
				setServer(file, line, server);
				bool duplicateId = false;
				for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); ++it)
				{
					if (it->getId() == server.getId())
					{
						duplicateId = true;
						break;
					}
				}
				if (duplicateId)
					{CERRANDEXIT CerrExit("Error: duplicate server ID: ", server.getId());}
				servers.push_back(server);
			}
		}
		else
			{CERRANDEXIT CerrExit("Error: bad line: ", line);}
	}
	file.close();
	if (hasDuplicateAddress(servers))
		{CERRANDEXIT CerrExit("Error: duplicate server address", "");}
	for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); ++it)
		if (checkIPServer(*it) == false)
			{CERRANDEXIT CerrExit("Error: invalid server address", "");}
	return (servers);
}

void signalHandler(int signum)
{
	(void)signum;
	exit(0);
}

// int main(void)
// {
//     std::vector<ServConfig> servers = ServerParsing("config/config.ini");
// 	for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); ++it)
// 	{
// 		it->checkValidity();
// 		std::cout << *it << std::endl;
// 	}
//     return 0;
// }
