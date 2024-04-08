#include "../../include/client/Data.hpp"

///////////////////////////////
//Constructors and destructor//
///////////////////////////////

Data::Data()
	: _files(), _cgiArgs()
{}

Data::~Data()
{}

//////////////////////
//operators overload//
//////////////////////

Data	&Data::operator=(Data const &src)
{
	if (this != &src)
	{
		_files = src._files;
		_cgiArgs = src._cgiArgs;
	}
	return *this;
}

std::ostream	&operator<<(std::ostream &out, Data const &src)
{
	out << "-----------Data------------" << std::endl;
	if (src._files.size() > 0)
	{
		out << "Files: " << std::endl;
		for (std::vector<contentData>::const_iterator it = src._files.begin(); it != src._files.end(); it++)
		{
			out << "Name: " << it->_fileName << std::endl;
			out << "Type: " << it->_contentType << std::endl;
			out << "Length: " << it->_data.size() << std::endl;
		}
	}
	else if (src._cgiArgs.size() > 0)
	{
		out << "CgiArgs: " << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = src._cgiArgs.begin(); it != src._cgiArgs.end(); it++)
			out << it->first << ": " << it->second << std::endl;
	}
	return out;
}

///////////
//setters//
///////////

#include <string>
#include <vector>
#include <sstream>

//parse body for files download
bool Data::setFiles(std::string boundary, std::string rawContent)
{
	std::string separator = "--" + boundary;
	std::string endSeparator = "--" + boundary + "--";
	size_t start = 0;
	size_t end;

	while ((start = rawContent.find(separator, start)) != std::string::npos)
	{
		start += separator.length();
		if (rawContent.substr(start, 2) == "--")
			break;
		start += 2;
		end = rawContent.find(separator, start);
		if (end == std::string::npos)
			return true;
		std::string part = rawContent.substr(start, end - start);
		start = end;
		size_t headerEndPos = part.find("\r\n\r\n");
		if (headerEndPos == std::string::npos)
			return true;
		std::string headerPart = part.substr(0, headerEndPos);
		std::string dataPart = part.substr(headerEndPos + 4);
		std::string fileName, contentType;
		std::istringstream headerStream(headerPart);
		std::string line;
		while (std::getline(headerStream, line))
		{
			size_t namePos = line.find("filename=\"");
			if (namePos != std::string::npos)
			{
				namePos += 10;
				size_t endPos = line.find("\"", namePos);
				if (endPos != std::string::npos)
					fileName = line.substr(namePos, endPos - namePos);
				else
					return true;
			}
			else if (line.find("Content-Type:") != std::string::npos)
				contentType = line.substr(line.find(":") + 2);
		}
		if (fileName.empty() || contentType.empty())
			return true;
		else
		{
			contentData data;
			data._fileName = fileName;
			data._contentType = contentType;
			data._data = dataPart;
			_files.push_back(data);
		}
	}
	return false;
}

//decode cgi args
std::string urlDecode(const std::string& encoded)
{
	std::string result;
	for (size_t i = 0; i < encoded.length(); ++i)
	{
		if (encoded[i] == '+')
			result += ' ';
		else if (encoded[i] == '%' && i + 2 < encoded.length())
		{
			std::istringstream hexStream(encoded.substr(i + 1, 2));
			int value;
			hexStream >> std::hex >> value;
			result += static_cast<char>(value);
			i += 2;
		}
		else
			result += encoded[i];
	}
	return result;
}

//parse body for cgi args
bool Data::setCgiArgs(std::string rawContent)
{
	std::istringstream stream(rawContent);
	std::string pair;

	while (std::getline(stream, pair, '&'))
	{
		size_t delimPos = pair.find('=');
		if (delimPos != std::string::npos)
		{
			std::string key = pair.substr(0, delimPos);
			std::string value = pair.substr(delimPos + 1);
			_cgiArgs[urlDecode(key)] = urlDecode(value);
	}	}
	return false;
}

///////////
//getters//
///////////

std::vector<contentData>	Data::getFiles()
	{return _files;}

std::map<std::string, std::string>	Data::getCgiArgs()
	{return _cgiArgs;}
