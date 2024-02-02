#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Cgi.hpp"
#include "../include/Route.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"

static bool hasDuplicateAddress(const std::map<std::string, ServerConfig>& servers)
{
	std::vector<sockaddr_in> addresses;
	std::map<std::string, ServerConfig>::const_iterator it;
	for (it = servers.begin(); it != servers.end(); ++it)
	{
		const sockaddr_in& serverAddr = it->second.getServerAddr();
		bool duplicateAddress = false;
		std::vector<sockaddr_in>::const_iterator addressIt;
		for (addressIt = addresses.begin(); addressIt != addresses.end(); ++addressIt)
		{
			if (memcmp(&(*addressIt), &serverAddr, sizeof(sockaddr_in)) == 0)
			{
				duplicateAddress = true;
				break;
			}
		}
		if (duplicateAddress)
			return true;
		addresses.push_back(serverAddr);
	}
	return false;
}


std::map<std::string, ServerConfig> getConfig(std::string configFile)
{
	std::fstream file(configFile.c_str());
	if (!file.is_open())
		cerr_and_exit("Error: Failed to open file: ", configFile, 1);
	
	std::map<std::string, ServerConfig> servers;

	std::string line;
	while (getline(file, line))
	{
		ServerConfig server = ServerConfig();
		if (line.empty())
			continue;
		else if (line[0] == '[' && line[line.size() - 1] == ']')
		{
			if (line.find(":") != std::string::npos)
				cerr_and_exit("Error: bad server name: ", line, 1);
			while (line[0] == '[' && countDeuxPoints(line) == 0)
			{
				server = ServerConfig();
				setServer(file, line, server);
				bool duplicateId = false;
				for (std::map<std::string, ServerConfig>::iterator it = servers.begin(); it != servers.end(); ++it)
				{
					if (it->first == server.getId())
					{
						duplicateId = true;
						break;
					}
				}
				if (duplicateId)
					cerr_and_exit("Error: duplicate server ID: ", server.getId(), 1);
				servers[server.getId()] = server;
			}
		}
		else
			cerr_and_exit("Error: bad line: ", line, 1);
	}
	file.close();
	if (hasDuplicateAddress(servers))
		cerr_and_exit("Error: duplicate server address", "", 1);
	return (servers);
}

void setServer(std::fstream &file, std::string &line, ServerConfig &server)
{
	server.setMain(file, line);
	if (line[0] == '[' && countDeuxPoints(line) > 0 && line.find_first_of(server.getId()) != line.npos)
	{
		if (countDeuxPoints(line) == 1 && line.find("ERROR") == line.find_first_of(":") + 1 && line.find("ERROR") != line.npos)
			server.setError(file, line);
		else if (countDeuxPoints(line) == 1 && line.find("CGI") == line.find_first_of(":") + 1 && line.find("CGI") != line.npos)
			server.setCgi(file, line);
		else if (countDeuxPoints(line) == 2 && line.find("ROUTE") == line.find_first_of(":") + 1 && line.find("ROUTE") != line.npos)
			server.setRoute(file, line);
		else if (countDeuxPoints(line) == 2 && line.find("UPLOAD") == line.find_first_of(":") + 1 && line.find("UPLOAD") != line.npos)
			server.setUpload(file, line);
		else
			cerr_and_exit("Error: bad line: ", line, 1);
	}
	while (line[0] == '[' || (getline(file, line) && line[0] != '['))
	{
		if (line[0] == '[' && countDeuxPoints(line) == 0)
			break ;
		if (line.empty())
			continue;
		if (countDeuxPoints(line) == 1 && line.find("ERROR") == line.find_first_of(":") + 1 && line.find("ERROR") != line.npos)
			server.setError(file, line);
		else if (countDeuxPoints(line) == 1 && line.find("CGI") == line.find_first_of(":") + 1 && line.find("CGI") != line.npos)
			server.setCgi(file, line);
		else if (countDeuxPoints(line) == 2 && line.find("ROUTE") == line.find_first_of(":") + 1 && line.find("ROUTE") != line.npos)
			server.setRoute(file, line);
		else if (countDeuxPoints(line) == 2 && line.find("UPLOAD") == line.find_first_of(":") + 1 && line.find("UPLOAD") != line.npos)
			server.setUpload(file, line);
		else
			cerr_and_exit("Error: bad line: ", line, 1);
	}
	server.checkValidity();
}

int	countDeuxPoints(std::string &line)
{
	int count = 0;
	for (unsigned int i = 0; i < line.size(); i++)
		if (line[i] == ':')
			count++;
	return (count);
}