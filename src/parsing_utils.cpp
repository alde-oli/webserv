#include "publiclib.hpp"

in_addr_t	setIPv4(const std::string &addr)
{
	in_addr_t	inetAddr;

	inetAddr = inet_addr(addr.c_str());
	if (inetAddr == INADDR_NONE)
	{
		std::cerr << "Error: setIPv4() failed for: " << addr << std::endl;
		exit (1);
	}

	return inetAddr;
}

in_port_t	setPort(const std::string &port)
{
	in_port_t	inetPort;

	inetPort = htons(atoi(port.c_str()));
	if (inetPort == 0)
	{
		std::cerr << "Error: setPort() failed for: " << port << std::endl;
		exit (1);
	}

	return inetPort;
}

std::string setFileStr(const std::string &file)
{
	std::ifstream inputFile(file.c_str());
	if (!inputFile)
	{
		std::cerr << "Error: Failed to open file: " << file << std::endl;
		exit(1);
	}
	inputFile.close();

	return file;
}

std::string setDirStr(const std::string &dir)
{
	DIR	*dirp;

	dirp = opendir(dir.c_str());
	if (!dirp)
	{
		std::cerr << "Error: Failed to open directory: " << dir << std::endl;
		exit(1);
	}
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
		{
			std::cerr << "Error: Invalid CGI extension: " << cgiExtensionStr << std::endl;
			exit(1);
		}

		std::vector<std::string> validExtensions = {".php", ".py", ".rb", ".pl", ".sh", ".cgi", ".rbw", ".tcl"};
		for (std::vector<std::string>::iterator it = validExtensions.begin(); it != validExtensions.end(); ++it)
		{
			if (*it == cgiExtensionStr)
				break ;
			if (it == validExtensions.end() - 1)
			{
				std::cerr << "Error: Invalid CGI extension: " << cgiExtensionStr << std::endl;
				exit(1);
			}
		}

		cgiExtensionVec.push_back(cgiExtensionStr);
	}

	return cgiExtensionVec;
}

static int check_method(const std::string& single_method)
{
    if (single_method == "GET" || single_method == "POST" || single_method == "DELETE")
	{
        return 1;
    }
	else
	{
        return 0;
    }
}

std::vector<std::string> set_method(const std::string& method)
{
    std::vector<std::string> stock;
    std::stringstream read_method(method);
    std::string tmp;

	if (method.empty())
	{
			std::cout << "Error: method is empty... :( " << std::endl;
            exit(1);
	}
    while (std::getline(read_method, tmp, ' '))
	{
        if (check_method(tmp) == 1)
		{
            // Vérifier si la méthode est déjà dans le vecteur
            if (std::find(stock.begin(), stock.end(), tmp) != stock.end())
			{
                std::cout << "Error: Tu as mis deux fois la meme methode, la shouma sur toi et sur toute ta famille\n #looooooser \n #ouloulou" << std::endl;
                exit(1);
            }
			else
			{
                stock.push_back(tmp);
            }
        }
		else
		{
            std::cout << "Error: Bad method input: " << method << std::endl;
            exit(1);
        }
    }
    return stock;
}

bool set_bool(std::string boolean)
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
	{
		std::cout << "Error: bad boolean type: " << boolean << std::endl;
		exit (1);
	}
}

static long long int valid_size(std::string size)
{
	int i = 0;
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

long long int set_body_size(std::string size)
{
	int		i = 0;
	if (!size.empty()) 
	{
		int i = 0;
		while (i < size.length() && isdigit(size[i]))
		{
			i++;
		}
		if (i < size.length() && !isdigit(size[i]) && (size[i] == 'M' || size[i] == 'G'))
		{
			if (i + 1 == size.length())
			{
				return (valid_size(size));
			}
			else
			{
				std::cout << "Error: bad body size input: " << size << std::endl;
				exit (1);
			}
		}
		else if (i == size.length())
		{
			return (valid_size(size));
		}
		else
		{
			std::cout << "Error: bad body size input: " << size << std::endl;
			exit (1);
		}
	}
	std::cout << "Error: bad body size input: " << size << std::endl;
	exit (1);
}