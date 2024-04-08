#include "../../include/config/Codes.hpp"
#include "../../include/config/ParsingUtils.hpp"

Codes	&Codes::operator=(Codes const &src)
{
	this->_errPages = src._errPages;
	return (*this);
}

std::ostream	&operator<<(std::ostream &out, Codes const &src)
{
	out << "Error pages: " << std::endl;
	for (std::map<int, std::string>::const_iterator it = src._errPages.begin(); it != src._errPages.end(); it++)
	{
		out << "Code: " << it->first << " Page: " << it->second << std::endl;
	}
	return out;
}

void	Codes::addErrPage(int code, std::string page)
{
	if (code < 400 || code > 599)
		{CERRANDEXIT CerrExit("Error: Error code must be between 100 and 599", "");}
	for (std::map<int, std::string>::const_iterator it = _errPages.begin(); it != _errPages.end(); ++it)
		if (it->first == code)
			{CERRANDEXIT CerrExit("Error: Error code already exists: ", page);}
	_errPages[code] = page;
}

const std::string	Codes::getErrPage(int code) const
{
	std::string errPage;
	if (_errPages.find(code) == _errPages.end())
	{
		errPage = DEFAULT_ERRORS;
		std::ostringstream oss;
		oss << code;
		errPage += oss.str();
		errPage += ".html";
	}
	else
		errPage = _errPages.find(code)->second;

	std::fstream file(errPage.c_str());
	if (!file.is_open())
		{CERRANDEXIT std::cerr << "Failed to open file: " << errPage << std::endl;}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	return content;
}

bool 	Codes::checkValidity(std::string dir)
{
	DIR	*dirp;

	dirp = opendir(dir.c_str());
	if (!dirp)
		{CERRANDEXIT CerrExit("Error: Failed to open directory: ", dir);}
	closedir(dirp);

	return false; // zero si le dossier est valide
}

std::string Codes::Msg500(int code_erreur)
{
	if (code_erreur == 500)
		return ("Internal Server Error");
	else if (code_erreur == 501)
		return ("Not Implemented");
	else if (code_erreur == 502)
		return ("Bad Gateway");
	else if (code_erreur == 503)
		return ("Service Unavailable");
	else if (code_erreur == 504)
		return ("Gateway Timeout");
	else if (code_erreur == 505)
		return ("HTTP Version Not Supported");
	else if (code_erreur == 506)
		return ("Variant Also Negotiates");
	else if (code_erreur == 507)
		return ("Insufficient Storage (WebDAV)");
	else if (code_erreur == 508)
		return ("Loop Detected (WebDAV)");
	else if (code_erreur == 510)
		return ("Not Extended");
	else if (code_erreur == 511)
		return ("Network Authentication Required");
	else
		return ("[KO]");
}

std::string Codes::Msg400(int code_erreur)
{
	if (code_erreur == 400)
		return ("Bad Request");
	else if (code_erreur == 401)
		return ("Unauthorized");
	else if (code_erreur == 402)
		return ("Payment");
	else if (code_erreur == 403)
		return ("Forbidden");
	else if (code_erreur == 404)
		return ("Not Found");
	else if (code_erreur == 405)
		return ("Method Not Allowed");
	else if (code_erreur == 406)
		return ("Not Acceptable");
	else if (code_erreur == 407)
		return ("Proxy Authentication Required");
	else if (code_erreur == 408)
		return ("Request Timeout");
	else if (code_erreur == 409)
		return ("Conflict");
	else if (code_erreur == 410)
		return ("Gone");
	else if (code_erreur == 411)
		return ("Length Required");
	else if (code_erreur == 412)
		return ("Precondition Failed");
	else if (code_erreur == 413)
		return ("Payload Too Large");
	else if (code_erreur == 414)
		return ("URI Too Long");
	else if (code_erreur == 415)
		return ("Unsupported Media Type");
	else if (code_erreur == 416)
		return ("Range Not Satisfiable");
	else if (code_erreur == 417)
		return ("Expectation Failed");
	else
		return ("[KO]");
}

std::string Codes::Msg300(int code_erreur)
{
	if (code_erreur == 300)
		return ("Multiple Choices");
	else if (code_erreur == 301)
		return ("Moved Permanently");
	else if (code_erreur == 302)
		return ("Found");
	else if (code_erreur == 303)
		return ("See Other");
	else if (code_erreur == 304)
		return ("Not Modified");
	else if (code_erreur == 305)
		return ("Use Proxy (Obsolète dans HTTP/1.1)");
	else if (code_erreur == 306)
		return ("(Unused)");
	else if (code_erreur == 307)
		return ("Temporary Redirect");
	else if (code_erreur == 308)
		return ("Permanent Redirect");
	else
		return ("[KO]");
}

std::string Codes::Msg200(int code_erreur)
{
	if (code_erreur == 200)
		return ("OK");
	else if (code_erreur == 201)
		return ("Created");
	else if (code_erreur == 202)
		return ("Accepted");
	else if (code_erreur == 203)
		return ("Non-Authoritative Information");
	else if (code_erreur == 204)
		return ("No Content");
	else if (code_erreur == 205)
		return ("Reset Content");
	else if (code_erreur == 206)
		return ("Partial Content");
	else
		return ("[OK]");
}

std::string	Codes::getMsgCode(int code)
{
	if (code >= 200 && code < 300)
		return (Msg200(code));
	else if (code >= 300 && code < 400)
		return (Msg300(code));
	else if (code >= 400 && code < 500)
		return (Msg400(code));
	else if (code >= 500 && code < 600)
		return (Msg500(code));
	else
		return ("[KO] : Code inconnu");
}