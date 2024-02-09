#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"
#include "../include/Route.hpp"
#include "../include/MultipartFormData.hpp"
#include "../include/htmlrequest.hpp"

void	cerr_and_exit(std::string msg, std::string support, int code)
{
	std::cerr << msg << support << std::endl;
	if (code)
		exit (code);
}

in_addr_t	setIPv4(const std::string &addr)
{
	in_addr_t	inetAddr;

	inetAddr = inet_addr(addr.c_str());
	if (inetAddr == INADDR_NONE)
		cerr_and_exit("Error: setIPv4() failed for: ", addr, 1);

	return inetAddr;
}

in_port_t	setPort(const std::string &port)
{
	in_port_t	inetPort;

	inetPort = htons(atoi(port.c_str()));
	if (inetPort == 0)
		cerr_and_exit("Error: setPort() failed for: ", port, 1);

	return inetPort;
}

std::string setFileStr(const std::string &file)
{
	std::ifstream inputFile(file.c_str());
	if (!inputFile)
		cerr_and_exit("Error: Failed to open file: ", file, 1);
	inputFile.close();

	return file;
}

std::string setDirStr(const std::string &dir)
{
	DIR	*dirp;

	dirp = opendir(dir.c_str());
	if (!dirp)
		cerr_and_exit("Error: Failed to open directory: ", dir, 1);
	closedir(dirp);

	return dir;
}

std::vector<std::string> setCgiExtension(const std::string &cgiExtension)
{
	std::vector<std::string> cgiExtensionVec;
	std::string cgiExtensionStr;
	std::istringstream cgiExtensionStream(cgiExtension);

	while (std::getline(cgiExtensionStream, cgiExtensionStr, ' '))
	{
		if (cgiExtensionStr[0] != '.')
			cerr_and_exit("Error: Invalid CGI extension: ", cgiExtensionStr, 1);
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
				cerr_and_exit("Error: Invalid CGI extension: ", cgiExtensionStr, 1);
		}
		cgiExtensionVec.push_back(cgiExtensionStr);
	}

	return cgiExtensionVec;
}

static int check_method(const std::string& single_method)
{
	if (single_method == "GET" || single_method == "POST" || single_method == "DELETE")
		return 1;
	else
		return 0;
}

std::vector<std::string> setMethod(const std::string& method)
{
	std::vector<std::string> stock;
	std::stringstream read_method(method);
	std::string tmp;

	if (method.empty())
		cerr_and_exit("Error: method is empty", "", 1);
	while (std::getline(read_method, tmp, ' '))
	{
		if (check_method(tmp) == 1)
		{
			// Vérifier si la méthode est déjà dans le vecteur
			if (std::find(stock.begin(), stock.end(), tmp) != stock.end())
				cerr_and_exit("Error: Tu as mis deux fois la meme methode, la shouma sur toi et sur toute ta famille\n#looooooser\n#ouloulou", "", 1);
			else
			{
				stock.push_back(tmp);
			}
		}
		else
			cerr_and_exit("Error: Bad method input: ", method, 1);
	}
	return stock;
}

bool setBool(std::string boolean)
{
	bool value;

	if (!strcmp(boolean.c_str(), "FALSE") || !strcmp(boolean.c_str(), "false"))
	{
		return (value = false);
	}
	else if (!strcmp(boolean.c_str(), "TRUE") || !strcmp(boolean.c_str(), "true"))
	{
		return (value = true);
	}
	else
		cerr_and_exit("Error: bad boolean type: ", boolean, 1); return (0);
}

static long long int setSize(std::string size)
{
	long long nbr;

	if (size.find('M') != std::string::npos)
	{
		size = size.substr(0, size.find('M'));
		nbr = atol(size.c_str());
		nbr *= 1000;
		return (nbr);
	}
	else if (size.find('G') != std::string::npos)
	{
		size = size.substr(0, size.find('G'));
		nbr = atol(size.c_str());
		nbr *= 1000000;
		return (nbr);
	}
	else
	{
		return (nbr = atol(size.c_str()));
	}
}

long long int setBodySize(std::string size)
{
	if (!size.empty()) 
	{
		unsigned int i = 0;
		while (i < size.length() && isdigit(size[i]))
		{
			i++;
		}
		if (i < size.length() && !isdigit(size[i]) && (size[i] == 'M' || size[i] == 'G'))
		{
			if (i + 1 == size.length())
			{
				return (setSize(size));
			}
			else
				cerr_and_exit("Error: bad body size input: ", size, 1);
		}
		else if (i == size.length())
		{
			return (setSize(size));
		}
		else
			cerr_and_exit("Error: bad body size input: ", size, 1);
	}
	cerr_and_exit("Error: bad body size input: ", size, 1);
	return (0);
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