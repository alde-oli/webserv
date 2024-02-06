#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"
#include "../include/Route.hpp"
#include "../include/Cgi.hpp"
#include "../include/MultipartFormData.hpp"
#include "../include/htmlrequest.hpp"
#include "../include/response.hpp"

// Constructor
// Setup all of the http request infos inside the class
HttpRequest::HttpRequest(const std::string& requestString, int client_fd)
	: client_fd(client_fd)
{
	std::istringstream requestStream(requestString);

	// Lire la ligne de requête
   	std::getline(requestStream, this->method, ' ');
	std::getline(requestStream, this->uri, ' ');
	std::getline(requestStream, this->httpVersion);
	// Lire les en-têtes
	std::string headerLine;
	while (std::getline(requestStream, headerLine) && headerLine != "\r")
	{
		size_t separatorPos = headerLine.find(": ");
		if (separatorPos != std::string::npos)
		{
			std::string key = headerLine.substr(0, separatorPos);
			std::string value = headerLine.substr(separatorPos + 2);
			this->headers[key] = value;
		}
	}
	// Lire le corps (si présent)
	if (this->headers.find("Content-Length") != this->headers.end())
		std::getline(requestStream, this->rawBody);
}

HttpRequest::~HttpRequest(){}

// [ METHODS ] //

static void deleteTab(char **env)
{
	for (size_t i = 0; env[i] != nullptr; ++i)
	{
		delete[] env[i];
	}
	delete[] env;
}

std::string getCgiArgs(const std::string& uri)
{
	size_t pos = uri.find("?");

	if (pos != std::string::npos && pos + 1 < uri.size())
	{
		return uri.substr(pos + 1);
	}
	return "";
}

static void perrorAndExit(char *msg, int code)
{
	perror(msg);
	if (code)
		exit (code);
}

static void closeAndDup(int fd1, int fd2, int dupFrom, int dupTo)
{
	close (fd1);
	dup2(dupFrom, dupTo);
	close(fd2);
}

void handleCgi(HttpRequest& request, Response &response, int clientFd, ServerConfig &server, std::string args)
{
	int pid;
	int pipefd[2];

	if (pipe(pipefd))
	{response.build(500, "", server, "text/html"); return ;}
	
	std::string cgiPath = server.getRoute(request.uri.substr(0, request.uri.find_last_of('/') + 1)).getRoot();
	std::vector<std::string>	envp;
	std::stringstream argStream(args);
	std::string arg;
	while (std::getline(argStream, arg, '&'))
		envp.push_back(arg);

	char **env = new char*[envp.size() + 1];

	for (size_t i = 0; i < envp.size(); ++i)
	{
		env[i] = new char[envp[i].size() + 1];
		std::strcpy(env[i], envp[i].c_str());
	}
	env[envp.size()] = NULL;
	char *argv[] = {NULL};

	pid = fork();
	if (pid == 0)
	{
		closeAndDup(pipefd[0], pipefd[1], pipefd[1], STDOUT_FILENO);
		execve(cgiPath.c_str(), argv, env);
		perrorAndExit("execve", EXIT_FAILURE);
	} 
	else if (pid < 0)
		{response.build(500, "", server, "text/html"); return ;}
	else
	{
		std::string cgiOutput;

		close(pipefd[1]);
		const int bufferSize = 4096;
		char buffer[bufferSize];
		ssize_t bytesRead;
	
		while ((bytesRead = read(pipefd[0], buffer, bufferSize)) > 0)
			cgiOutput.append(buffer, bytesRead);

		close(pipefd[0]);
		waitpid(pid, NULL, 0);
		if (!cgiOutput.empty())
			response.build(200, cgiOutput, server, "text/html");
		else
			response.build(500 , "", server, "text/html");
	}
	deleteTab(env);
}

std::string extensionType(HttpRequest &request)
{
	std::string contentType;

	contentType = request.uri.find(".html") != std::string::npos ? "text/html" :
					request.uri.find(".css") != std::string::npos ? "text/css" :
					request.uri.find(".js") != std::string::npos ? "application/javascript" :
					request.uri.find(".png") != std::string::npos ? "image/png" :
					request.uri.find(".jpg") != std::string::npos ? "image/jpeg" :
					request.uri.find(".jpeg") != std::string::npos ? "image/jpeg" :
					request.uri.find(".gif") != std::string::npos ? "image/gif" :
					request.uri.find(".svg") != std::string::npos ? "image/svg+xml" :
					request.uri.find(".ico") != std::string::npos ? "image/x-icon" :
					request.uri.find(".mp3") != std::string::npos ? "audio/mpeg" :
					request.uri.find(".mp4") != std::string::npos ? "video/mp4" :
					request.uri.find(".avi") != std::string::npos ? "video/x-msvideo" :
					request.uri.find(".pdf") != std::string::npos ? "application/pdf" :
					request.uri.find(".zip") != std::string::npos ? "application/zip" :
					request.uri.find(".tar") != std::string::npos ? "application/x-tar" :
					request.uri.find(".gz") != std::string::npos ? "application/gzip" :
					request.uri.find(".dmg") != std::string::npos ? "application/x-apple-diskimage" :
					request.uri.find(".xml") != std::string::npos ? "application/xml" :
					request.uri.find(".json") != std::string::npos ? "application/json" :
					request.uri.find(".csv") != std::string::npos ? "text/csv" :
					request.uri.find(".txt") != std::string::npos ? "text/plain" :
					"application/octet-stream";
	return (contentType);
}

void handleGet(HttpRequest& request, Response &response, int clientFd, ServerConfig &server)
{
	std::string path = request.uri.substr(0, request.uri.find_last_of('/') + 1);
	if (server.hasRoute(path) == false)
		{response.build(404, "", server, "text/html"); return ;}

	Route route = server.getRoute(path);
	if (route.isRedir() == true)
		{response.build(301, route.getRedirDir(), server, "text/html"); return ;}
	if (route.isMethodAllowed("GET") == false)
		{response.build(405, "", server, "text/html"); return ;}

	std::string ressource = request.uri.substr(request.uri.find_last_of('/') + 1);
	if (ressource.empty())
	{
		if (route.isListing() == true)
			{response.build(200, route.listRoute(), server, "text/html"); return ;}
		else if ( route.getPage() != "")
			std::string resourcePath = path + route.getPage();
		else
			{response.build(404, "", server, "text/html"); return ;}
	}
	else
	{
		std::string extension = ressource.substr(ressource.find_last_of('.') + 1, ressource.find_first_of('?'));
		if (route.isCgi(extension) == true)
			handleCgi(request, response, clientFd, server, getCgiArgs(request.uri));
		else
			ressource = route.getRoot() + ressource;
	}
	
	std::string contentType = extensionType(request);
	if (route.isUpload() == true || route.isForceUpload() == true)
		contentType = "Content-Disposition: attachment; filename=" + request.uri.substr(request.uri.find_last_of('/') + 1) + ";" + "\n" + contentType;
	if (access(ressource.c_str(), F_OK) == -1)
		{response.build(404, "", server, "text/html"); return ;}
	std::ifstream file(ressource, std::ios::binary);
	if (!file.is_open())
		{response.build(500, "", server, "text/html");return ;}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	response.build(200, content, server, contentType);
}


void handlePost(HttpRequest& request, Response &response, int clientFd, ServerConfig &server)
{
	std::vector<contentData>    content = request.formattedBody.getContent();

	std::string path = request.uri.substr(0, request.uri.find_last_of('/') + 1);

	if (server.hasRoute(path) == false)
		{response.build(404, "", server, "text/html"); return ;}
	
	Route route = server.getRoute(path);

	if (route.isRedir() == true)
		{response.build(301, route.getRedirDir(), server, "text/html"); return ;}

	if (route.isMethodAllowed("POST") == false)
		{response.build(405, "", server, "text/html"); return ;}

	std::string ressource = request.uri.substr(request.uri.find_last_of('/') + 1);

	if (ressource.empty() == false)
	{
		std::string extension = ressource.substr(ressource.find_last_of('.') + 1);
		if (route.isCgi(extension) == true)
			{handleCgi(request, response, clientFd, server, request.rawBody); return ;}
		else
			{response.build(404, "", server, "text/html"); return ;}
	}

	if (route.isDownload() == false)
		{response.build(405, "", server, "text/html"); return ;}
	
	if (request.headers["Content-Type"].find("multipart/form-data") != std::string::npos)
	{
		request.formattedBody = MultipartFormData(request.headers["Content-Type"], request.rawBody);
	
		for (size_t i = 0; i < content.size(); i++)
		{
			if (content[i].filename.empty())
				continue;

			std::string filePath = route.getDownloadDir() + content[i].filename;

			std::ofstream file(filePath, std::ios::out | std::ios::trunc | std::ios::binary);
			if (file.is_open())
			{
				file.write(content[i].content.data(), content[i].content.size());
				file.close();

				if (!file)
					{response.build(500, "", server, "text/html"); return ;}
				response.build(200, "", server, "text/html");
			}
			else
				{response.build(500, "", server, "text/html"); return ;}
		}
	}
	else
		response.build(400, "", server, "text/html");
}


void handleDelete(HttpRequest& request, Response &response, int clientFd, ServerConfig &server)
{
	std::string path = request.uri.substr(0, request.uri.find_last_of('/') + 1);

	if (server.hasRoute(path) == false)
	{
		response.build(404, "", server, "text/html");
		return ;
	}

	Route route = server.getRoute(path);
	if (route.isMethodAllowed("DELETE") == false)
	{
		response.build(405, "", server, "text/html");
		return ;
	}

	std::string resourcePath = route.getRoot() + request.uri.substr(request.uri.find_last_of('/') + 1);

	if (access(resourcePath.c_str(), F_OK) == -1)
		response.build(404, "", server, "text/html");
	else if (remove(resourcePath.c_str()) != 0)
		response.build(500, "", server, "text/html");
	else
		response.build(200, "", server, "text/html");
};

void HttpRequest::HandleRequest(Response &response, int clientFd, ServerConfig &server)
{
	std::string requests[3] = {"GET", "POST", "DELETE"};
	void (*handlers[3])(HttpRequest&, Response&, int, ServerConfig&) = {handleGet, handlePost, handleDelete};
	
	for (int i = 0; i < 3; i++)
	{
			if (this->method == requests[i])
				handlers[i](*this, response, clientFd, server);
	}
}


// [ SETTERS ] //

void	HttpRequest::setMethod		(std::string method)
{
	this->method = method;
}

void	HttpRequest::setUri			(std::string uri)
{
	this->uri = uri;
}

void	HttpRequest::setHttpVersion	(std::string httpVersion)
{
	this->httpVersion = httpVersion;
}

void	HttpRequest::setClientFd		(int clientFd)
{
	this->client_fd = clientFd;
}

void	HttpRequest::setHeaders(std::map<std::string, std::string> headers)
{
	this->headers = headers;
}

void 	HttpRequest::setFormattedBody(MultipartFormData formattedBody)
{
	this->formattedBody = formattedBody;
}


// [ GETTER ] //

std::string	HttpRequest::getMethod(void)
{
	return (this->method);
}

std::string	HttpRequest::getUri(void)
{
	return (this->uri);
}

std::string	HttpRequest::getHttpVersion(void)
{
	return (this->httpVersion);
}

int			HttpRequest::getClientFd(void)
{
	return (this->client_fd);
}

std::map<std::string, std::string>	HttpRequest::getheaders(void)
{
	return (this->headers);
}

MultipartFormData   HttpRequest::getformattedBody(void)
{
	return (this->formattedBody);
}
