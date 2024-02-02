#include "../include/Cgi.hpp"
#include "../include/parsing.hpp"

//////////////////////////////
//contructors and destructor//
//////////////////////////////

CGI::CGI() : _extensions(), _path()
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
		this->_path = other._path;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &out, const CGI &cgi)
{
	out << "_________cgi__________" << std::endl;
	out << "-Extensions: ";
	for (std::vector<std::string>::const_iterator it = cgi._extensions.begin(); it != cgi._extensions.end(); it++)
		out << *it << " ";
	out << std::endl;
	out << "-Paths: " << cgi._path << std::endl;
	out << "‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾" << std::endl;
	return (out);
}


///////////
//setters//
///////////

void	CGI::addExtension(const std::string &extension)
{
	for (std::vector<std::string>::const_iterator it = this->_extensions.begin(); it != this->_extensions.end(); it++)
		if (*it == extension)
		{
			std::cerr << "Error: CGI extension already exists" << extension << std::endl;
			exit(EXIT_FAILURE);
		}
	this->_extensions.push_back(extension);
}

void	CGI::addPath(const std::string &path)
{
	this->_path = setDirStr(path);
}


///////////
//getters//
///////////

const std::string	&CGI::getPath() const
{
	return (this->_path);
}

bool	CGI::isValidExt(const std::string &extension) const
{
	for (std::vector<std::string>::const_iterator it = this->_extensions.begin(); it != this->_extensions.end(); it++)
		if (*it == extension)
			return (true);
	return (false);
}
