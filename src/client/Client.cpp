#include "../../include/client/Client.hpp"
#include "../../include/config/ServConfig.hpp"
#include "../../include/client/Request.hpp"
#include "../../include/client/Response.hpp"
#include "../../include/webserv.hpp"


//////////////////////////////
//Constructor and destructor//
//////////////////////////////

//input: the fd returned by accept(), the client address, and the server socketFd it is connected to
//creates a client ready to handle events
Client::Client(int clientFd, sockaddr_in clientAddr, int serverFd)
{
	_clientFd.set(clientFd);
	_serverFd = serverFd;
	_clientAddr = clientAddr;
	_lastActivity = std::time(0);

	_rawRequest = "";
	_EOHFound = false;
	_bodyToRead = 0;
	_writeEventSet = false;

}

//client socket is automatically closed if open
Client::~Client()
{}

//////////////////////
//operators overload//
//////////////////////

//copy assignment operator
Client	&Client::operator=(const Client &c)
{
	_clientFd = c._clientFd;
	_serverFd = c._serverFd;
	_clientAddr = c._clientAddr;
	_lastActivity = c._lastActivity;
	_rawRequest = c._rawRequest;
	_EOHFound = c._EOHFound;
	_bodyToRead = c._bodyToRead;
	_request = c._request;
	_response = c._response;
	return (*this);
}

std::ostream	&operator<<(std::ostream &os, const Client &c)
{
	os << "[][]#-----Client identity-----#[][]" << std::endl;
	os << "Address: " << inet_ntoa(c._clientAddr.sin_addr) << ":" << ntohs(c._clientAddr.sin_port) << std::endl;
	os << "socket fd: " << c._clientFd.get() << " | ";
	os << "last activity: " << std::ctime(&c._lastActivity) << std::endl;
	//os << c._request << std::endl;
	//os << c._response << std::endl;
	os << "[][]#--------------------------#[][]" << std::endl;

	return (os);
}

///////////
//setters//
///////////

//updates the last activity time to the current time
void	Client::updateActivity()
	{_lastActivity = std::time(0);}

//sets the client socket fd
void	Client::setClientFd(int fd)
	{_clientFd.set(fd);}

///////////
//getters//
///////////

//returns the server socket fd the client is connected to
int		Client::getServFd()
	{return (_serverFd);}


//returns the client address
sockaddr_in	Client::getAddr()
	{return (_clientAddr);}

//returns the last activity time
std::time_t	Client::getLastActivity()
	{return (_lastActivity);}

void	Client::printRequest()
	{READLOG(_request)}

////////////////////
//member functions//
////////////////////

bool isCorrectHost(sockaddr_in addr, std::string servHostname, std::string host)
{
	std::string ip = inet_ntoa(addr.sin_addr);
	std::stringstream ss;
	ss << ntohs(addr.sin_port);
	ip += ":" + ss.str();
	if (host == servHostname || host == ip)
		return true;
	
	
	return false;
}

//reads the client request and processes it into _response
//return: true if client need to be closed else false
bool Client::read(ServConfig &server, std::vector<struct pollfd> &pollfds)
{
	updateActivity();

	const std::string EOHeader = "\r\n\r\n"; // End of header
	char buff[BUF_SIZE]; // Stores data read by recept
	ssize_t bytesRead;
	READLOG("Reading header")
	READLOG("EOHFound: " << _EOHFound)
	// Try to find end of header if not already found
	if (!_EOHFound) {
		bytesRead = recv(_clientFd.get(), buff, BUF_SIZE, 0);
		if (bytesRead <= 0) {
			if (bytesRead == 0) std::cout << "Client closed connection" << std::endl;
			else READLOG("recept fail, closing client")
			return true; // True indicates an error or closed connection
		}
		_rawRequest.append(buff, bytesRead);

		std::string::size_type pos = _rawRequest.find(EOHeader);
		if (pos != std::string::npos) {
			_EOHFound = true;
			if (_request.buildHeader(_rawRequest.substr(0, pos)))
			{
				_response.setCode(400); // Bad request
				return true;
			}
			_rawRequest.erase(0, pos + EOHeader.length());

			std::string host = _request["Host"];
			if (host.empty())
				{_response.setCode(400); return true;}
			else
			{
				if (!isCorrectHost(server.getAddr(), server.getName(), host))
					{_response.setCode(0); return true;}
			}

			// If request is POST, check for Content-Length
			if (_request["method"] == "POST" && _request.isHeader("Content-Length")) {
				_bodyToRead = std::atoi(_request["Content-Length"].c_str());
				if (_bodyToRead > static_cast<size_t>(server.getMaxBodySize())) {
					_response.setCode(413); // Payload Too Large

					return true;
				}
			}
			else if (_request["method"] == "POST") {
				std::cerr << "POST request without Content-Length" << std::endl;
				_response.setCode(411); // Length Required
				return true;
			}
			else {
				// No body expected, handle request
				if (server.isCookies())
					handleCookies(server.getAddr(), server.getName());

				bool ret = !_request.handle(server, _response);
				_request.clear();
				setWriteEvent(pollfds);
				return ret;
			}
		}
		else
			return false; // Continue reading
	}

	READLOG("Reading body")
	READLOG("EOHFound: " << _EOHFound)
	// Handle POST requests with expected body
	if (_EOHFound && _bodyToRead > 0)
	{

		if (_rawRequest.size() >= _bodyToRead) {
			if (_request.buildBody(_rawRequest.substr(0, _bodyToRead)))
				{ _response.setCode(400); return true;}
			// Request has been fully read, handle it
			_EOHFound = false; // Reset for the next request
			_bodyToRead = 0; // Reset body length for the next request
			bool ret = !_request.handle(server, _response);
			_rawRequest = _rawRequest.substr(_bodyToRead);
			_request.clear();
			setWriteEvent(pollfds);
			return ret;
		}
		bytesRead = recv(_clientFd.get(), buff, BUF_SIZE, 0);
		if (bytesRead <= 0) {
			if (bytesRead == 0) {CERRANDEXIT std::cout << "Client closed connection during body read" << std::endl;}
			else {CERRANDEXIT std::cerr << "recept fail during body read, closing client" << std::endl;}
			return true; // Error or closed connection
		}
		_rawRequest.append(buff, bytesRead);

		if (_rawRequest.size() >= _bodyToRead) {
			if (_request.buildBody(_rawRequest.substr(0, _bodyToRead)))
				{_response.setCode(400); // Bad request
				return false;}
			// Request has been fully read, handle it
			_EOHFound = false; // Reset for the next request
			_bodyToRead = 0; // Reset body length for the next request
			bool ret = !_request.handle(server, _response);
			_rawRequest.clear();
			_request.clear();
			setWriteEvent(pollfds);
			return ret;
		}
	}
	return false; // Continue reading
}
 

//writes the response to the client
//return: true if client needs to be closed, else false
bool	Client::write(std::vector<struct pollfd> &pollfds)
{
	WRITELOG(_response)
	updateActivity();

	//if error(1) return true
	//if fully sent(1) unset write
	//else return false
	int ret = _response.deliver(_clientFd.get());
	if (ret)
	{
		unsetWriteEvent(pollfds);
		_response.clear();
		return true;
	}
	return false;
}

//set WriteEvent to notify poll we will send data to client in next poll loop
bool Client::setWriteEvent(std::vector<struct pollfd> &pollfds)
{
	// Find the pollfd entry for this client
	for (size_t i = 0; i < pollfds.size(); i++)
	{
		if (pollfds[i].fd == _clientFd.get())
		{
			// Add POLLOUT to the events we're interested in
			pollfds[i].events |= POLLOUT;
			_writeEventSet = true;
			return false;
		}
	}
	// If we get here, the client fd wasn't found in pollfds
	{CERRANDEXIT std::cerr << "error while registering write event, client fd not found" << std::endl << *this;}
	return true; // Signal an error
}

//end write event
bool Client::unsetWriteEvent(std::vector<struct pollfd> &pollfds)
{
	_response.clear();
	
	// Find the pollfd entry for this client and remove POLLOUT
	for (size_t i = 0; i < pollfds.size(); i++)
	{
		if (pollfds[i].fd == _clientFd.get())
		{
			// Remove POLLOUT from the events we're interested in
			pollfds[i].events &= ~POLLOUT;
			_writeEventSet = false;
			return true;
		}
	}
	
	// If we get here, the client fd wasn't found in pollfds
	if (_writeEventSet == true)
	{
		{CERRANDEXIT std::cerr << "error while unregistering write event, client fd not found" << std::endl << *this;}
		return false;
	}
	_writeEventSet = false;
	return true;
}

// Parse cookies from the Cookie header
static std::map<std::string, std::string> parseCookies(const std::string& cookieHeader) {
	std::map<std::string, std::string> cookies;
	std::istringstream stream(cookieHeader);
	std::string token;

	if (cookieHeader.empty()) {
		return cookies;
	}

	while (getline(stream, token, ';')) {
		size_t equalPos = token.find('=');
		if (equalPos != std::string::npos) {
			std::string key = token.substr(0, equalPos);
			std::string value = token.substr(equalPos + 1);
			if (!key.empty() && key[0] == ' ') {
				key.erase(0, 1);
			}
			cookies[key] = value;
		}
	}
	return cookies;
}

// Get current time as a string
static std::string getTimeStr() {
	std::time_t now = std::time(NULL);
	struct tm *timeStruct = std::localtime(&now);
	std::stringstream sstime;
	sstime << (timeStruct->tm_year + 1900) << '-' << (timeStruct->tm_mon + 1) << '-' << timeStruct->tm_mday << ' ' << timeStruct->tm_hour << ':' << timeStruct->tm_min << ':' << timeStruct->tm_sec;
	return sstime.str();
}

// Simple hash function
std::string simpleHash(std::string data) {
	data += getTimeStr();

	unsigned long hash = 5381;
	for (std::string::const_iterator it = data.begin(); it != data.end(); ++it) {
		char c = *it;
		hash = ((hash << 5) + hash) + c; // hash * 33 + c
	}
	std::stringstream ss;
	ss << hash;
	return ss.str();
}

// Find or create cookie data
std::string findCookieData(const std::string& serverHostname, const sockaddr_in& server_ip, const sockaddr_in& client_ip) {
	std::ifstream file("cookies.data");
	if (!file.is_open()) {
		std::ofstream createFile("cookies.data");
		if (!createFile.is_open()) {
			std::cerr << "Unable to create file" << std::endl;
			return "";
		}
		createFile.close();
		file.open("cookies.data");
		if (!file.is_open()) {
			std::cerr << "Unable to open file for reading" << std::endl;
			return "";
		}
	}
	std::string line, result;

	// Convert IP addresses to strings
	char serverIpStr[INET_ADDRSTRLEN], clientIpStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(server_ip.sin_addr), serverIpStr, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &(client_ip.sin_addr), clientIpStr, INET_ADDRSTRLEN);

	std::ostringstream serverIpStream, clientIpStream;
	serverIpStream << serverIpStr << ":" << ntohs(server_ip.sin_port);
	clientIpStream << clientIpStr;

	std::string serverIpWithPort = serverIpStream.str();
	std::string clientIpWithPort = clientIpStream.str();

	std::string lineToFind = "[Host=" + serverHostname + "|" + std::string(serverIpWithPort) + "] Client=" + std::string(clientIpWithPort) + " Hash=";

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			if (line.find(lineToFind) != std::string::npos)
				{result = line;
				file.close();
				return result;}
		}
		file.clear();
		file.close();
	}
	else
		{std::cerr << "Unable to open file" << std::endl; return "";}

	if (result.empty())
	{
		result = lineToFind + simpleHash(clientIpWithPort);
		std::ofstream outFile("cookies.data", std::ios::app);
		if (outFile.is_open())
			{outFile << result << std::endl;
			outFile.close();}
		else
		{std::cerr << "Unable to open file for writing" << std::endl;}
	}

	return result;
}

// Handle cookies in the client
void Client::handleCookies(sockaddr_in addr, std::string hostname)
{
	std::string cookieDough;
	if (_request.isHeader("Cookie"))
		cookieDough = _request["Cookie"];

	std::map<std::string, std::string> cookies = parseCookies(cookieDough);
	if (cookies.empty())
		{std::string cookieData = findCookieData(hostname, addr, _clientAddr);
		std::string id = cookieData.substr(cookieData.find("Hash=") + 5);
		_response.setCookie("id=" + id);}
		//_response.setCookie("id=" + id + "; Last-Modified=" + getTimeStr());}
	else
		{//cookies["Last-Modified"] = getTimeStr();
		if (cookies.find("id") == cookies.end())
			{std::string cookieData = findCookieData(hostname, addr, _clientAddr);
			std::string id = cookieData.substr(cookieData.find("Hash=") + 5);
			cookies["id"] = id;}
		else
			{std::string storedId = findCookieData(hostname, addr, _clientAddr);
			std::string id = storedId.substr(storedId.find("Hash=") + 5);
			if (cookies["id"] != id)
				{cookies["id"] = id;}}

		_response.setCookie("id=" + cookies["id"]);}
}

//returns true if the client has a response to snd
bool	Client::isResponse()
	{if (_response.getCode() != 0)
		return true;
	return false;}
