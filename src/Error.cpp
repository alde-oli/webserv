#include "Error.hpp"
#include "parsing.hpp"

//////////////////////////////
//contructors and destructor//
//////////////////////////////

Error::Error()
	: _errorPages()
{
}

Error::Error(const Error &other)
{
	*this = other;
}

Error::~Error()
{
}


//////////////////////
//operators overload//
//////////////////////

Error	&Error::operator=(const Error &other)
{
	if (this != &other)
	{
		_errorPages = other._errorPages;
	}
	return (*this);
}

std::ostream	&operator<<(std::ostream &out, const Error &error)
{
	out << "________Error pages________" << std::endl;
	for (std::map<int, std::string>::const_iterator it = error._errorPages.begin(); it != error._errorPages.end(); ++it)
	{
		out << "-Error code: " << it->first << std::endl;
		out << "\tError page: " << it->second << std::endl;
	}
	out << "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾" << std::endl;
	return (out);
}


///////////
//setters//
///////////

void	Error::addPage(int errorCode, const std::string &errorPage)
{
	if (errorCode < 400 || errorCode > 599)
	{
		std::cerr << "Error: Error code must be between 100 and 599" << std::endl;
		exit(EXIT_FAILURE);
	}
	for (std::map<int, std::string>::const_iterator it = _errorPages.begin(); it != _errorPages.end(); ++it)
		if (it->first == errorCode)
		{
			std::cerr << "Error: Error code already exists" << std::endl;
			exit(EXIT_FAILURE);
		}
	_errorPages[errorCode] = errorPage;
}


///////////
//getters//
///////////

const std::string	&Error::getPage(int errorCode) const
{
	std::map<int, std::string>::const_iterator it = _errorPages.find(errorCode);
	return (it->second);
}
