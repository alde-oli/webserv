#include "Route.hpp"
#include "parsing.hpp"

//////////////////////////////
//contructors and destructor//
//////////////////////////////

Route::Route()
	: _id(""), _route(""), _root(""), _page(""), _methods(), _listing(false), _isDownload(false), _downloadDir(""), _isRedir(false), _redirDir("")
{
}

Route::Route(std::string id)
	: _id(id), _route(""), _root(""), _page(""), _methods(), _listing(false), _isDownload(false), _downloadDir(""), _isRedir(false), _redirDir("")
{
}

Route::Route(const Route &other)
{
	*this = other;
}

Route::~Route()
{
}


//////////////////////
//operators overload//
//////////////////////

Route	&Route::operator=(const Route &other)
{
	if (this != &other)
	{
		this->_route = other._route;
		this->_root = other._root;
		this->_page = other._page;
		this->_methods = other._methods;
		this->_listing = other._listing;
		this->_isDownload = other._isDownload;
		this->_downloadDir = other._downloadDir;
		this->_isRedir = other._isRedir;
		this->_redirDir = other._redirDir;
	}
	return (*this);
}

std::ostream	&operator<<(std::ostream &out, const Route &route)
{
	out << "____ Id: " << route._id << " ____"<< std::endl;
	out << "Route: " << route._route << std::endl;
	out << "Root: " << route._root << std::endl;
	out << "Page: " << route._page << std::endl;
	out << "Methods: ";
	for (std::vector<std::string>::const_iterator it = route._methods.begin(); it != route._methods.end(); it++)
		out << *it << " ";
	out << std::endl;
	out << "Listing: " << route._listing << std::endl;
	out << "IsDownload: " << route._isDownload << std::endl;
	out << "DownloadDir: " << route._downloadDir << std::endl;
	out << "IsRedir: " << route._isRedir << std::endl;
	out << "RedirDir: " << route._redirDir << std::endl;
	out << "‾‾‾‾‾‾‾‾‾‾‾‾" << std::endl;
	for (int i = 0; i < route._id.length(); i++)
		out << "‾";
	out << std::endl;
	return (out);
}


///////////
//setters//
///////////

void	Route::setRoute(const std::string &route)
{
	if (!isRouteValid(route))
	{
		std::cerr << "Error: Invalid route: " << route << std::endl;
		exit(EXIT_FAILURE);
	}
	this->_route = route;
}

void	Route::setRoot(const std::string &root)
{
	this->_root = setDirStr(root);
}

void	Route::setPage(const std::string &page)
{
	this->_page = setFileStr(page);
}

void	Route::setMethods(const std::string &methods)
{
	this->_methods = setMethod(methods);
}

void	Route::setListing(const std::string listing)
{
	this->_listing = setBool(listing);
}

void	Route::setDownload(const std::string download)
{
	this->_isDownload = setBool(download);
}

void 	Route::setDownloadDir(const std::string downloadDir)
{
	this->_downloadDir = setDirStr(downloadDir);
}

void	Route::setRedir(const std::string redir)
{
	this->_isRedir = setBool(redir);
}

void	Route::setRedirDir(const std::string redirDir)
{
	this->_redirDir = setDirStr(redirDir);
}


///////////
//getters//
///////////

const std::string	&Route::getRoute() const
{
	return (this->_route);
}

const std::string	&Route::getRoot() const
{
	return (this->_root);
}

const std::string	&Route::getPage() const
{
	return (this->_page);
}

bool	Route::isMethodAllowed(std::string requestMethod) const
{
	for (std::vector<std::string>::const_iterator it = this->_methods.begin(); it != this->_methods.end(); it++)
	{
		if (*it == requestMethod)
			return (true);
	}
	return (false);
}

bool	Route::isListing() const
{
	return (this->_listing);
}

std::string	&Route::listRoute() const
{
	std::string		html;
	DIR*			dir;
	struct dirent*	ent;

	html += "<!DOCTYPE html><html><head><title>Liste des Fichiers</title></head><body>";
	html += "<h1>Liste des Fichiers pour " + this->_root + "</h1><ul>";
	if ((dir = opendir(this->_root.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			std::string fileName = ent->d_name;
			if (fileName != "." && fileName != "..")
				html += "<li><a href='" + fileName + "'>" + fileName + "</a></li>";
		}
		closedir(dir);
	}
	else
		html += "<li>Erreur lors de l'ouverture du répertoire.</li>";
	html += "</ul></body></html>";
	return html;
}

bool	Route::isDownload() const
{
	return (this->_isDownload);
}

const std::string	&Route::getDownloadDir() const
{
	return (this->_downloadDir);
}

bool	Route::isRedir() const
{
	return (this->_isRedir);
}

const std::string	&Route::getRedirDir() const
{
	return (this->_redirDir);
}