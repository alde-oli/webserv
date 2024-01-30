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