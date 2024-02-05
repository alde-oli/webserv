#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"
#include "../include/Route.hpp"
#include "../include/Cgi.hpp"
#include "../include/MultipartFormData.hpp"
#include "../include/HtmlRequest.hpp"

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

//temporary
static std::string BuildtHttpResponse(std::string cgiOutput)
{
    std::string httpResponse = "HTTP/1.1 200 OK\r\n";
    httpResponse += "Content-Type: text/html\r\n";
	httpResponse += "Content-Length: " + std::to_string(cgiOutput.size()) + "\r\n";
	httpResponse += "\r\n";
	httpResponse += cgiOutput;

    return (httpResponse);
}

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

static bool pipe_error(char *msg, int boolean)
{
    perror(msg);
    return (boolean);
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

void handleCgi(HttpRequest& request, const std::string& args, HttpResponse &response, ServerConfig &server)
{
    int pid;
    int pipefd[2];

    if (pipe(pipefd))
        pipe_error("pipe", false);

    //separate cgi path , cgi filename and args
    // add root path to filename
    
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
            response.build(200, cgiOutput, servConfig);
        else
            response.build(500 , "", servConfig);
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

void handleGet(HttpRequest& request, std::map<int, HttpResponse &response, int clientFd, ServerConfig &server)
{
    if (request.uri.length() > 4 && !request.uri.find("/cgi"))
        return handleCgi(request, getCgiArgs(request.uri));

    std::string resourcePath;
    std::string contentType;

    resourcePath = "www/webpages" + request.uri;
    if (request.uri.find("/download") == 0)
        contentType = extensionType(request);
    else
        contentType = "text/html";

    std::ifstream file(resourcePath, std::ios::binary);

    if (!file.is_open())
    {
        std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(clientFd, response.c_str(), response.length(), 0);
        return true;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();
    
    std::string contentDisposition = request.uri.find("/download") == 0 ? "attachment; filename=" : "inline; filename=" + resourcePath.substr(resourcePath.find_last_of("/") + 1);
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + contentDisposition + contentType + "\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
    
    dToSend[clientFd] = response;

    return true;
}


void handlePost(HttpRequest& request, std::map<int, HttpResponse &response, int clientFd, ServerConfig &server)
{
    std::string response;

    if (request.uri.length() > 4 && !request.uri.find("/cgi"))
        return handleCgi(request, request.rawBody);

    if (request.headers["Content-Type"].find("multipart/form-data") != std::string::npos)
    {
        request.formattedBody = MultipartFormData(request.headers["Content-Type"], request.rawBody);
    
        for (size_t i = 0; i < request.formattedBody.content.size(); i++)
        {
            if (request.formattedBody.content[i].filename.empty())
                continue;

            std::string filePath = "www/webpages/kittenland/" + request.formattedBody.content[i].filename;
            std::ofstream file(filePath, std::ios::out | std::ios::trunc | std::ios::binary);

            if (file.is_open())
            {
                file.write(request.formattedBody.content[i].content.data(), request.formattedBody.content[i].content.size());
                file.close();

                if (!file)
                {
                    response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
                    break;
                }
                response = "HTTP/1.1 200 OK\r\n\r\n";
            }
            else
            {
                response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
                break;
            }
        }
    }
    else
        response = "HTTP/1.1 400 Bad Request\r\n\r\n";

    dToSend[clientFd] = response;
    return true;
}


void handleDelete(HttpRequest& request, std::map<int, HttpResponse &response, int clientFd, ServerConfig &server)
{
	std::string response;

	if (request.uri.length() <= 4 || request.uri.find("/kittenland/"))
		return false;
    
    std::string resourcePath = "www/webpages/" + request.uri;

	if (remove(resourcePath.c_str()) != 0)
    	response = "HTTP/1.1 404 Not Found\r\n\r\n";
    else
        response = "HTTP/1.1 200 OK\r\n\r\n";

	dToSend[clientFd] = response;
	return true;
};

void HttpRequest::HandleRequest(std::map<int, HttpResponse &response, int clientFd, ServerConfig &server)
{
	std::string requests[3] = {"GET", "POST", "DELETE"};
	bool (*handlers[3])(HttpRequest&, std::map<int, std::string>& dToSend, int clientFd) = {handleGet, handlePost, handleDelete};
	
    for (int i = 0; i < 3; i++)
	{
        	if (this->method == requests[i])
			    return handlers[i](*this, dToSend, clientFd);
    }
    return false;
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
