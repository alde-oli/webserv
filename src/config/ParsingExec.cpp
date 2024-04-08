#include "../../include/config/ServConfig.hpp"
#include "../../include/config/ParsingExec.hpp"

// ---------------------------- [ ADDITIONAL FUNCTIONS FOR PARSING ] -------------------------------- //

int	countDeuxPoints(std::string &line)
{
	int count = 0;
	for (unsigned int i = 0; i < line.size(); i++)
		if (line[i] == ':')
			count++;
	return (count);
}

bool hasDuplicateAddress(const std::vector<ServConfig>&servers)
{
	std::vector<sockaddr_in> addresses;
	std::vector<ServConfig>::const_iterator it;
	for (it = servers.begin(); it != servers.end(); ++it)
	{
		sockaddr_in serverAddr = it->getAddr();
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

// ---------------------------- [ MAIN FUNCTIONS FOR PARSING ] -------------------------------- //


in_addr_t	setIPv4(const std::string &addr)
{
	in_addr_t	inetAddr;

	inetAddr = inet_addr(addr.c_str());
	if (inetAddr == INADDR_NONE)
		{CERRANDEXIT CerrExit("Error: setIPv4() failed for: ", addr);}
	return inetAddr;
}

std::string setFileStr(const std::string &file)
{
	std::ifstream inputFile(file.c_str());
	if (!inputFile)
		{CERRANDEXIT CerrExit("Error: Failed to open file: ", file);}
	inputFile.close();

	return file;
}

static long long int setSize(std::string size)
{
	long long nbr;

	if (size.find('M') != std::string::npos)
	{
		size = size.substr(0, size.find('M'));
		nbr = atol(size.c_str());
		nbr *= 1000000;
		return (nbr);
	}
	else if (size.find('G') != std::string::npos)
	{
		size = size.substr(0, size.find('G'));
		nbr = atol(size.c_str());
		nbr *= 1000000000;
		return (nbr);
	}
	else
		return (nbr = atol(size.c_str()));
}

long long int setBodySize(std::string size)
{
	if (!size.empty()) 
	{
		unsigned int i = 0;
		while (i < size.length() && isdigit(size[i]))
			i++;
		if (i < size.length() && !isdigit(size[i]) && (size[i] == 'M' || size[i] == 'G'))
		{
			if (i + 1 == size.length())
				return (setSize(size));
			else
				{CERRANDEXIT CerrExit("Error: bad body size input: ", size);}
		}
		else if (i == size.length())
			return (setSize(size));
		else
			{CERRANDEXIT CerrExit("Error: bad body size input: ", size);}
	}
	{CERRANDEXIT CerrExit("Error: bad body size input: ", size);}
	return (0);
}

bool setBool(std::string boolean)
{
	bool value;

	if (!strcmp(boolean.c_str(), "FALSE") || !strcmp(boolean.c_str(), "false"))
		return (value = false);
	else if (!strcmp(boolean.c_str(), "TRUE") || !strcmp(boolean.c_str(), "true"))
		return (value = true);
	else
		{CERRANDEXIT CerrExit("Error: bad boolean type: ", boolean); return (0);}
}

std::string setDirStr(const std::string &dir)
{
	DIR	*dirp;

	dirp = opendir(dir.c_str());
	if (!dirp)
		{CERRANDEXIT CerrExit("Error: Failed to open directory: ", dir);}
	closedir(dirp);

	return dir;
}

static int check_method(const std::string& single_method)
{
	if (single_method == "GET" || single_method == "POST" || single_method == "DELETE")
		return 1;
	else
		return 0;
}

std::vector<std::string> setCgiExtension(const std::string &cgiExtension)
{
	std::vector<std::string> cgiExtensionVec;
	std::string cgiExtensionStr;
	std::istringstream cgiExtensionStream(cgiExtension);

	while (std::getline(cgiExtensionStream, cgiExtensionStr, ' '))
	{
		if (cgiExtensionStr[0] != '.')
			{CERRANDEXIT CerrExit("Error: Invalid CGI extension: ", cgiExtensionStr);}
		std::vector<std::string> validExtensions;
		validExtensions.push_back(".php");
		validExtensions.push_back(".py");
		validExtensions.push_back(".rb");
		validExtensions.push_back(".pl");
		validExtensions.push_back(".sh");
		validExtensions.push_back(".cgi");
		validExtensions.push_back(".rbw");
		validExtensions.push_back(".tcl");
		for (std::vector<std::string>::iterator it = validExtensions.begin(); it != validExtensions.end(); ++it)
		{
			if (*it == cgiExtensionStr)
				break ;
			if (it == validExtensions.end() - 1)
				{CERRANDEXIT CerrExit("Error: Invalid CGI extension: ", cgiExtensionStr);}
		}
		cgiExtensionVec.push_back(cgiExtensionStr);
	}

	return cgiExtensionVec;
}

std::vector<std::string> setMethod(const std::string& method)
{
	std::vector<std::string> stock;
	std::stringstream read_method(method);
	std::string tmp;

	if (method.empty())
		{CERRANDEXIT CerrExit("Error: method is empty", "");}
	while (std::getline(read_method, tmp, ' '))
	{
		if (check_method(tmp) == 1)
		{
			// Vérifier si la méthode est déjà dans le vecteur
			if (std::find(stock.begin(), stock.end(), tmp) != stock.end())
				{CERRANDEXIT CerrExit("Error: Tu as mis deux fois la meme methode, la shouma sur toi et sur toute ta famille\n#looooooser\n#ouloulou", "");}
			else
				stock.push_back(tmp);
		}
		else
			{CERRANDEXIT CerrExit("Error: Bad method input: ", method);}
	}
	return stock;
}

bool isRouteValid(const std::string& chemin)
{
	const std::string validChars = "abcdefghijklmnopqrstuvwxyz"
									"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
									"0123456789"
									"-._~/";
	if (chemin.empty() || chemin[0] != '/')
		return false;
	if (chemin.find("//") != std::string::npos || chemin.find("/../") != std::string::npos || chemin.find("/./") != std::string::npos)
		return false;
	for (size_t i = 0; i < chemin.length(); ++i)
	{
		char c = chemin[i];
		if (validChars.find(c) == std::string::npos)
			return false;
	}
	return true;
}

void	setServer(std::fstream &file, std::string &line, ServConfig &server)
{
	server.setMain(file, line);
	if (line[0] == '[' && countDeuxPoints(line) > 0 && line.find_first_of(server.getId()) != line.npos)
	{
		if (countDeuxPoints(line) == 1 && line.find("ERROR") == line.find_first_of(":") + 1 && line.find("ERROR") != line.npos)
			server.setError(file, line);
		else if (countDeuxPoints(line) == 2 && line.find("ROUTE") == line.find_first_of(":") + 1 && line.find("ROUTE") != line.npos)
			server.setRoute(file, line);
		else
			{CERRANDEXIT CerrExit("Error: bad line: ", line);}
	}
	while (line[0] == '[' || (getline(file, line) && line[0] != '['))
	{
		if (line[0] == '[' && countDeuxPoints(line) == 0)
			break ;
		if (line.empty())
			continue;
		if (countDeuxPoints(line) == 1 && line.find("ERROR") == line.find_first_of(":") + 1 && line.find("ERROR") != line.npos)
			server.setError(file, line);
		else if (countDeuxPoints(line) == 2 && line.find("ROUTE") == line.find_first_of(":") + 1 && line.find("ROUTE") != line.npos)
			server.setRoute(file, line);
		else
			{CERRANDEXIT CerrExit("Error: bad line: ", line);}
	}
	server.checkValidity();
}