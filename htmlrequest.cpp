#include "htmlrequest.hpp"

#include <sstream>
#include <iostream>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <errno.h>
#include <map>
#include <ctime>
#include <signal.h>



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
        if (separatorPos != std::string::npos) {
            std::string key = headerLine.substr(0, separatorPos);
            std::string value = headerLine.substr(separatorPos + 2);
            this->headers[key] = value;
        }
    }

    // Lire le corps (si présent)
    if (this->headers.find("Content-Length") != this->headers.end())
	{
        std::getline(requestStream, this->body);
    }
}

std::string getCgiArgs(std::string uri)
{
    std::string args = "";
    size_t pos = uri.find("?");
    if (pos != std::string::npos)
        args = uri.substr(pos + 1);
    return args;
}

int handleCgi(HttpRequest& request, const std::string& args)
{
    std::istringstream argStream(args);
    std::string arg;
    int pid;
    int pipefd[2];
    if (pipe(pipefd))
    {
        perror("pipe");
        return(EXIT_FAILURE);
    }
    request.uri = "/Users/alde-oli/Desktop/webserv/www" + request.uri;
    request.uri = request.uri.substr(0, request.uri.find("?"));
    std::cout << std::endl << std::endl << "cgi is " << request.uri << std::endl << std::endl;

    std::cout << "args" << args << std::endl;
    //make args become env by splitting on &
    std::vector<std::string> envp;
    while (std::getline(argStream, arg, '&'))
    {
        envp.push_back(arg);
    }
    // transfer envp to char **envp
    char **env = new char*[envp.size() + 1];
    for (size_t i = 0; i < envp.size(); i++)
    {
        env[i] = new char[envp[i].size() + 1];
        strcpy(env[i], envp[i].c_str());
    }
    

    // [Préparer le pipe et le fork comme dans l'exemple précédent]
    pid = fork();

    if (pid == 0)
    {
        // Processus enfant
        // [Configurer le pipe]
        close(pipefd[0]); // Fermer le côté lecture du pipe
        dup2(pipefd[1], STDOUT_FILENO); // Rediriger stdout vers le pipe
        close(pipefd[1]); // Fermer le côté écriture du pipe

        // Exécuter le script CGI avec le nouvel environnement
        execve(request.uri.c_str(), NULL, env);
        // Si execve échoue
        perror("execve");
        exit(EXIT_FAILURE);
    } else
    {
        // Processus parent
        // [Lire la sortie du CGI, attendre le processus enfant, et envoyer la réponse]
        std::string cgiOutput;

        // Processus parent
        close(pipefd[1]); // Fermer le côté écriture du pipe

        const int bufferSize = 4096;
        char buffer[bufferSize];
        ssize_t bytesRead;
    
        while ((bytesRead = read(pipefd[0], buffer, bufferSize)) > 0)
        {
            cgiOutput.append(buffer, bytesRead);
        }

        close(pipefd[0]); // Fermer le côté lecture du pipe
        waitpid(pid, NULL, 0); // Attendre la fin du processus enfant
        if (!cgiOutput.empty())
        {
            std::string httpResponse = "HTTP/1.1 200 OK\r\n";
            httpResponse += "Content-Type: text/html\r\n"; // Ajustez le Content-Type si nécessaire
            httpResponse += "Content-Length: " + std::to_string(cgiOutput.size()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += cgiOutput;
            send(request.client_fd, httpResponse.c_str(), httpResponse.size(), 0);
            std::cout << httpResponse << std::endl;
        }
        else
        {
            // En cas d'erreur ou de sortie vide du CGI, envoyez une réponse d'erreur
            std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
            send(request.client_fd, errorResponse.c_str(), errorResponse.size(), 0);
            std::cout << "CGI response error" << std::endl;
        }
    }

    return 0;
}

int handleGet(HttpRequest& request)
{
    if (request.uri.length() > 4 && !request.uri.find("/cgi"))
         return handleCgi(request, getCgiArgs(request.uri));
    std::string resourcePath = "www" + request.uri;  // Chemin de la ressource demandée
    std::ifstream file(resourcePath);

    if (!file.is_open()) {
        // Renvoyer une erreur 404 si le fichier n'existe pas
        std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(request.client_fd, response.c_str(), response.length(), 0);
        return -1;
    }

    // Lire le contenu du fichier
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Construire et envoyer la réponse
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
    send(request.client_fd, response.c_str(), response.length(), 0);

    return 0;
}


int handlePost(HttpRequest& request)
{
    std::cout << "POST" << std::endl;
    return 0;
}

int handleDelete(HttpRequest& request)
{
    std::cout << "DELETE" << std::endl;
    return 0;
};

int HttpRequest::HandleRequest()
{
    std::string requests[3] = {"GET", "POST", "DELETE"};
    int (*handlers[3])(HttpRequest&) = {handleGet, handlePost, handleDelete};
    for (int i = 0; i < 3; i++)
        if (this->method == requests[i])
            return handlers[i](*this);
    return 1;
}

HttpRequest::~HttpRequest(){}