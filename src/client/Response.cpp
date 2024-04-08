#include "../../include/client/Response.hpp"
#include "../../include/logs.hpp"

///////////////////////////////
//Constructors and destructor//
///////////////////////////////

Response::Response()
	: _code(0), _keepAlive(false), _cookie(""), _contentDisposition(""), _contentType(""), _contentLength(""), _content(""), _notBuilt(true)
{}

Response::~Response()
{}

//////////////////////
//operators overload//
//////////////////////

Response	&Response::operator=(Response const &src)
{
	_code = src._code;
	_keepAlive = src._keepAlive;
	_cookie = src._cookie;
	_contentDisposition = src._contentDisposition;
	_contentType = src._contentType;
	_contentLength = src._contentLength;
	_content = src._content;
	_notBuilt = src._notBuilt;
	return *this;
}

std::ostream	&operator<<(std::ostream &os, Response const &src)
{
	os << "#---------Response----------#" << std::endl;
	os << "Code: " << src._code << std::endl;
	os << "Connection: " << (src._keepAlive ? "keep-alive" : "close") << std::endl;
	os << "Cookie: " << src._cookie << std::endl;
	os << "Content-Disposition: " << src._contentDisposition << std::endl;
	os << "Content-Type: " << src._contentType << std::endl;
	os << "Content-Length: " << src._contentLength << std::endl;
	os << "Content: "<< std::endl << src._content << std::endl;
	os << "#---------------------------#" << std::endl;
	return os;
}

///////////
//setters//
///////////

void	Response::setCode(int code)
	{_code = code;}

void	Response::setCodes(Codes codes)
	{_codes = codes;}

void	Response::setKeepAlive(bool keepAlive)
	{_keepAlive = keepAlive;}

void	Response::setCookie(std::string cookie)
	{_cookie = cookie;}

void	Response::setLocation(std::string location)
	{_location = location;}

void	Response::setContentDisposition(std::string contentDisposition)
	{_contentDisposition = contentDisposition;}

void	Response::setContentType(std::string contentType)
	{_contentType = contentType;}

void	Response::setContentLength(int contentLength)
{
	std::stringstream ss;
	ss << contentLength;
	_contentLength = ss.str();
}

void	Response::setContent(std::string content)
	{_content = content;}

///////////
//getters//
///////////

int		Response::getCode()
	{return _code;}

bool	Response::getKeepAlive()
	{return _keepAlive;}

std::string	Response::getCookie()
	{return _cookie;}

std::string	Response::getLocation()
	{return _location;}

std::string	Response::getContentDisposition()
	{return _contentDisposition;}

std::string	Response::getContentType()
	{return _contentType;}

std::string	Response::getContentLength()
	{return _contentLength;}

std::string	Response::getContent()
	{return _content;}

////////////////////
//member functions//
////////////////////

int	Response::deliver(int socket)
{
	std::string response = "HTTP/1.1 " + std::to_string(_code) + " " + _codes.getMsgCode(_code) + "\r\n";
	WRITELOG(*this)

	if (_notBuilt && _code > 399)
	{
		std::string content = _codes.getErrPage(_code);
		response += "Connection: close\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: "
				+ std::to_string(content.size()) + "\r\n\r\n"
				+ content;
		_notBuilt = false;
		_toSnd = response.size();
		_sentTotal = 0;
		_sent = 0;
	}
	else if (_notBuilt)
	{
		response += "Connection: ";
		if (_keepAlive)
			response += "keep-alive\r\n";
		else
			response += "close\r\n";
		if (_cookie != "")
			response += "Set-Cookie: " + _cookie + "; Max-Age=1800\r\n";
		if (_location != "")
			response += "Location: " + _location + "\r\n";
		if (_contentDisposition != "")
			response += "Content-Disposition: " + _contentDisposition + "\r\n";
		if (_contentType != "")
			response += "Content-Type: " + _contentType + "\r\n";
		if (_contentLength != "")
			response += "Content-Length: " + _contentLength + "\r\n";
		response += "\r\n";
		response += _content;
		_notBuilt = false;
		_toSnd = response.size();
		_sentTotal = 0;
		_sent = 0;
	}
	_sent = send(socket, response.c_str() + _sentTotal, _toSnd, 0);

	if (_sent < 0)
		return 1;

	_sentTotal += _sent;
	_toSnd -= _sent;

	if (_toSnd == 0)
	{
		_sentTotal = 0;
		_sent = 0;
		clear();
		return 1;
	}
	return 0;
}

void	Response::clear()
	{*this = Response();}
