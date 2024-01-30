#include "publiclib.hpp"
#include "ServerConfig.hpp"
#include "Error.hpp"
#include "parsing.hpp"

void	ServerConfig::setMain(std::fstream &file)
{
	std::string		line;
	std::string		key;
	std::string		value;

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
			this->_maxBodySize = set_body_size(line.substr(16));
		else if (!line.find("is_default = "))
			this->_isDefault = set_bool(line.substr(13));
	}
}

void	ServerConfig::setError(std::fstream &file)
{
	std::string		line;
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

void	ServerConfig::setCgi(std::fstream &file)
{
	std::string		line;
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
	}
}

void	ServerConfig::setRoute(std::fstream &file)
{
	std::string		line;
	std::string		key;
	std::string		value;
	Route			route();

	
	while (std::getline(file, line) && line[0] != '[')
	{

	}
}