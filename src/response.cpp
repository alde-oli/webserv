#include "../include/response.hpp"

std::string Response::caring500(int code_erreur)
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

std::string Response::caring400(int code_erreur)
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

std::string Response::caring300(int code_erreur)
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

std::string Response::caring200(int code_erreur)
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


static int	CountCharsInFile(std::string filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{
		std::cerr << "Error builing the response: File failed to open" << std::endl;
		return (-1);
	}
	int	counter = 0;
	char ch;

	// has get() recup some chars it continue to inc
	while (file.get(ch))
		counter++;
	file.close();
	return (counter);
}

std::string getBody(std::string filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{
		std::cerr << "Error builing the response: File failed to open" << std::endl;
		return (NULL);
	}

	std::ostringstream content;
    std::string line;

	while (std::getline(file, line))
		content << line << '\n';

	file.close();
	return (content.str());
}

void Response::build(int code_erreur, std::string body, ServerConfig servConfig, std::string ContentType)
{
	std::stringstream ss;
	ss << code_erreur;
	std::string code_str = ss.str();

	if (code_erreur < 300)
		this->response = "HTTP/1.1 " + code_str + caring200(code_erreur) + '\n';
	else if (code_erreur < 400)
		this->response = "HTTP/1.1 " + code_str + caring300(code_erreur) + '\n';
	else if (code_erreur < 500)
		this->response = "HTTP/1.1 " + code_str + caring400(code_erreur) + '\n';
	else
		this->response = "HTTP/1.1 " + code_str + caring500(code_erreur) + '\n';
	this ->response = this->response + "Server : " + servConfig.getServerName() + '\n';
	this->response = this->response + "Content-Type: " + ContentType +'\n';
	// si body pas vide definir content length sue la taille de body et ajouter body. sinon chercher page d'erreur avec getErrorPage puis ouvrir fichier et ajouter le contenu 
	ss << CountCharsInFile(body);
	std::string chars_nbr = ss.str();
	this->response = this->response + "Content-Length: " + chars_nbr + '\n';
	this->response = this->response + getBody(body) + '\n';
	std::string errorPage = servConfig.getErrorPage(code_erreur);
}

void Response::setResponse(std::string response)
{
	this->response = response;
}

std::string Response::getResponse(void)
{
	return (response);
}