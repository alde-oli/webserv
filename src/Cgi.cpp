#include "Cgi.hpp"

//////////////////////////////
//contructors and destructor//
//////////////////////////////

CGI::CGI() : _extensions(), _paths()
{
}

CGI::CGI(const CGI &other)
{
	*this = other;
}

CGI::~CGI()
{
}

//////////////////////
//operators overload//
//////////////////////

CGI &CGI::operator=(const CGI &other)
{
	if (this != &other)
	{
		this->_extensions = other._extensions;
		this->_paths = other._paths;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &out, const CGI &cgi)
{
	out << "_________cgi" << std::endl;
	out << "-Extensions: ";
	for (std::vector<std::string>::const_iterator it = cgi._extensions.begin(); it != cgi._extensions.end(); it++)
		out << *it << " ";
	out << std::endl;
	out << "-Paths: " << cgi._paths << std::endl;
	out << "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾" << std::endl;
	return (out);
}
