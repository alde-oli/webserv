#include "../../include/config/Route.hpp"
#include "../../include/config/ParsingUtils.hpp"
#include "../../include/config/ParsingExec.hpp"

#include <iostream>

///////////////////////////////
//Constructors and destructor//
///////////////////////////////

Route::Route()
{
	_id = "";
	_path = "";
	_root = "";
	_defaultPage = "";
	_methods = std::vector<std::string>();
	_listing = false;
	_isUpload = false;
	_forceUpload = false;
	_isDownload = false;
	_downloadDir = "";
	_isRedir = false;
	_redirDir = "";
	_cgiExt = std::vector<std::string>();
}

/////////////
// [ SET ] //
/////////////

void	Route::setId(std::string id)
{
	this->_id = id;
}

void	Route::setPath(std::string path)
{
	if (!isRouteValid(path))
		{CERRANDEXIT CerrExit("Error: Invalid route: ", path);}
	this->_path = path;
}

void	Route::setRoot(std::string root)
{
	this->_root = setDirStr(root);
}

void	Route::setDefaultPage(std::string defaultPage)
{
	this->_defaultPage = setFileStr(defaultPage);
}

void	Route::setMethods(std::string methods)
{
	this->_methods = setMethod(methods);
}

void	Route::setListing(std::string listing)
{
	this->_listing = setBool(listing);
}

void	Route::setIsUpload(std::string isUpload)
{
	this->_isUpload = setBool(isUpload);
}

void	Route::setForceUpload(std::string forceUpload)
{
	this->_forceUpload = setBool(forceUpload);
}

void	Route::setIsDownload(std::string isDownload)
{
	this->_isDownload = setBool(isDownload);
}

void 	Route::setDownloadDir(std::string downloadDir)
{
	this->_downloadDir = setDirStr(downloadDir);
}

void	Route::setIsRedir(std::string isRedir)
{
	this->_isRedir = setBool(isRedir);
}

void	Route::setRedirDir(std::string redirDir)
{
	this->_redirDir = setDirStr(redirDir);
}

void	Route::setCgiExt(std::string cgiExt)
{
	this->_cgiExt = setCgiExtension(cgiExt);
}

/////////////
// [ GET ] //
/////////////

std::string	Route::getId()
{
	return this->_id;
}

std::string	Route::getPath()
{
	return this->_path;
}

std::string	Route::getRoot()
{
	return this->_root;
}

std::vector<std::string>	Route::getMethods()
{
	return this->_methods;
}

bool	Route::isRedir()
{
	return this->_isRedir;
}

std::string	Route::getRedirDir()
{
	return this->_redirDir;
}

bool	Route::isListing()
{
	return this->_listing;
}

std::string	Route::list()
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
				html += "<li>" + fileName + "</a></li>";
		}
		closedir(dir);
	}
	else
		html += "<li>Erreur lors de l'ouverture du répertoire.</li>";
	html += "</ul></body></html>";
	return html;
}

std::string	Route::getDefaultPage()
{
	return this->_defaultPage;
}

bool	Route::isDownload()
{
	return this->_isDownload;
}

std::string	Route::getDownloadDir()
{
	return this->_downloadDir;
}

bool	Route::isUpload()
{
	return this->_isUpload;
}

bool	Route::isCgi(std::string path)
{
	for (std::vector<std::string>::const_iterator it = this->_cgiExt.begin(); it != this->_cgiExt.end(); it++)
		if (*it == path)
			return (true);
	return (false);
}

/////////////////////
/// [ SURCHARGE ] ///
/////////////////////

std::ostream	&operator<<(std::ostream &out, Route const &src)
{
	out << "Route ID: " << src._id << std::endl;
	out << "Route: " << src._path << std::endl;
	out << "Root: " << src._root << std::endl;
	out << "DefaultPage: " << src._defaultPage << std::endl;
	out << "Methods: ";
	for (std::vector<std::string>::const_iterator it = src._methods.begin(); it != src._methods.end(); it++)
		out << *it << " ";
	out << std::endl;
	out << "IsRedir: " << src._isRedir << std::endl;
	out << "RedirDir: " << src._redirDir << std::endl;
	out << "Listing: " << src._listing << std::endl;
	out << "IsDownload: " << src._isDownload << std::endl;
	out << "DownloadDir: " << src._downloadDir << std::endl;
	out << "IsUpload: " << src._isUpload << std::endl;
	out << "ForceUpload: " << src._forceUpload << std::endl;
	out << "CgiExt: ";
	for (std::vector<std::string>::const_iterator it = src._cgiExt.begin(); it != src._cgiExt.end(); it++)
		out << *it << " ";
	out << std::endl;
	return out;
}

Route	&Route::operator=(Route const &src)
{
	this->_id = src._id;
	this->_path = src._path;
	this->_root = src._root;
	this->_defaultPage = src._defaultPage;
	this->_methods = src._methods;
	this->_isRedir = src._isRedir;
	this->_redirDir = src._redirDir;
	this->_listing = src._listing;
	this->_isDownload = src._isDownload;
	this->_downloadDir = src._downloadDir;
	this->_isUpload = src._isUpload;
	this->_forceUpload = src._forceUpload;
	this->_cgiExt = src._cgiExt;
	return *this;
}

//////////////////////////
/// [Member functions] ///
//////////////////////////

void	Route::verif() const
{
	int count = 0;

	if (_id.empty())
		{CERRANDEXIT CerrExit("Route ID is empty please insert a valid Route ID", "");}
	else if (_path.empty())
		{CERRANDEXIT CerrExit("Route is empty please insert a valid Route", "");}
	else if (_root.empty())
		{CERRANDEXIT CerrExit("Root is empty please insert a valid Root", "");}
	else if (_methods.empty())
		{CERRANDEXIT CerrExit("Methods are empty please insert valids Methods", "");}
	else if (_isDownload && _downloadDir.empty())
		{CERRANDEXIT CerrExit("DownloadDir is empty please insert a valid DownloadDir", "");}
	else if (_isRedir && _redirDir.empty())
		{CERRANDEXIT CerrExit("RedirDir is empty please insert a valid RedirDir", "");}
	else if (_listing)
		count++;
	else if (_isRedir)
		count++;
	if (count == 2 && !_defaultPage.empty())
		{CERRANDEXIT CerrExit("Multiple requests type not supported", "");}
}
