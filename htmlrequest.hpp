#pragma once

#include <string>
#include <map>

class HttpRequest
{
	public:
	    std::string method;
	    std::string uri;
	    std::string httpVersion;
	    std::map<std::string, std::string> headers;
	    std::string body;
		int client_fd;
		
		HttpRequest(const std::string& requestString, int client_fd);
		int	HandleRequest();
		~HttpRequest();
};
