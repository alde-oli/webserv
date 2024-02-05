#include "../include/response.hpp"

void Response::build(int code_erreur, std::string body, ServerConfig servConfig)
{
	servConfig.getErrorPage(code_erreur);
	this->setResponse(body);
}

void Response::setResponse(std::string response)
{
	this->response = response;
}

std::string Response::getResponse(void)
{
	return (response);
}