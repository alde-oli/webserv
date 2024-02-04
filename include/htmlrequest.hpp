#pragma once

#include "MultipartFormData.hpp"
#include "publiclib.hpp"

class HttpRequest
{
	public:		
		HttpRequest(){};
		HttpRequest(const std::string& requestString, int client_fd);
		HttpRequest(const HttpRequest &other) {};
		~HttpRequest();

		bool								HandleRequest(std::map<int, std::string>& clientDataToSend, int client_fd);
		
		void	setMethod(std::string method);
		void	setUri(std::string uri);
		void	setHttpVersion(std::string httpVersion);
		void	setClientFd(int clientFd);
		void	setHeaders(std::map<std::string, std::string> headers);
		void 	setFormattedBody(MultipartFormData formattedBody);

		std::string	getMethod(void);
		std::string	getUri(void);
		std::string	getHttpVersion(void);
		int			getClientFd(void);
		std::map<std::string, std::string>	getheaders(void);
		MultipartFormData					getformattedBody(void);

		private:
			std::string							method;
			std::string							uri;
			std::string							httpVersion;
			std::map<std::string, std::string>	headers;
			std::string							rawBody;
			MultipartFormData					formattedBody;
			int									client_fd;
};
