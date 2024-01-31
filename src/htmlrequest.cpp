#include "../include/htmlrequest.hpp"
#include "../include/MultipartFormData.hpp"

// Constructor
// Setup all of the http request infos inside the class
HttpRequest::HttpRequest(const std::string& requestString, int client_fd) : client_fd(client_fd)
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
	{
		std::getline(requestStream, this->rawBody);
	}
}

std::string getCgiArgs(const std::string& uri)
{
    // Se positionner sur les args du CGI
    size_t pos = uri.find("?");

    if (pos != std::string::npos && pos + 1 < uri.size())
    {
        // Retourner string a partir des args CGI
        return uri.substr(pos + 1);
    }
    return "";
}

bool handleCgi(HttpRequest& request, const std::string& args)
{
    int pid;
    int pipefd[2];

    if (pipe(pipefd))
    {
        perror("pipe");
        return false;
    }

    // Creation du chemin pour la CGI resquest sans les args
    std::string cgiPath = "/Users/alde-oli/Desktop/ws_old/www/webpages" + request.uri.substr(0, request.uri.find("?"));

    // Préparation des arguments et des variables d'environnement
    std::vector<std::string> envp;
    std::istringstream argStream(args);
    std::string arg;
    
    // Pushback dans le vector envp char args du CGI
    while (std::getline(argStream, arg, '&'))
    {
        envp.push_back(arg);
    }

    // Creation et allocation d'un tab env
    // De la taille du vector envp
    char **env = new char*[envp.size() + 1];
    
    // On rempli env des elem envp
    // Avec allocation de chaques strings
    for (size_t i = 0; i < envp.size(); ++i)
    {
        env[i] = new char[envp[i].size() + 1];
        std::strcpy(env[i], envp[i].c_str());
    }
    
    // Dernier elem du tab mis à NULL
    env[envp.size()] = NULL;

    // Aucun argument de ligne de commande supplémentaire n'est transmis
    char *argv[] = {NULL};

    pid = fork();

    // Processus enfant
    if (pid == 0)
    {
        // Fermer le côté lecture du pipe
        close(pipefd[0]);
        // Rediriger stdout vers le pipe
        dup2(pipefd[1], STDOUT_FILENO);
        // Fermer le côté écriture du pipe
        close(pipefd[1]);

        // Execution du chemin CGI avec args vides + env var
        execve(cgiPath.c_str(), argv, env);
        // Si execve fail
        perror("execve");
        exit(EXIT_FAILURE);
    } 
    else
    {
		// Processus parent
		// [Lire la sortie du CGI, attendre le processus enfant, et envoyer la réponse]
		std::string cgiOutput;

        // Fermer le côté écriture du pipe
		close(pipefd[1]);

		const int bufferSize = 4096;
		char buffer[bufferSize];
		ssize_t bytesRead;
	
        // Lit le retour d'execve
		while ((bytesRead = read(pipefd[0], buffer, bufferSize)) > 0)
		{
            // Ajouter les elements au fur et a mesure dans le string
			cgiOutput.append(buffer, bytesRead);
		}

        // Fermer le côté lecture du pipe
		close(pipefd[0]);
        // Attendre la fin du processus enfant
		waitpid(pid, NULL, 0);

        // Si CGI a bien émit une réponse
		if (!cgiOutput.empty())
		{
			std::string httpResponse = "HTTP/1.1 200 OK\r\n";
			httpResponse += "Content-Type: text/html\r\n";
			httpResponse += "Content-Length: " + std::to_string(cgiOutput.size()) + "\r\n";
			httpResponse += "\r\n";
			httpResponse += cgiOutput;
			send(request.client_fd, httpResponse.c_str(), httpResponse.size(), 0);
			std::cout << httpResponse << std::endl;
		}
		else
		{
			// Cas d'erreur / sortie vide du CGI
            // Affiche mess d'erreur
			std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
			send(request.client_fd, errorResponse.c_str(), errorResponse.size(), 0);
			std::cout << "CGI response error" << std::endl;
		}
	}

    // Desalloc de l'environnement
    for (size_t i = 0; i < envp.size(); ++i)
    {
        delete[] env[i];
    }
    
    delete[] env;

    return true;
}

bool handleGet(HttpRequest& request, std::map<int, std::string>& dToSend, int clientFd)
{
    std::cout << "[ GET request received ]" << std::endl;

    if (request.uri.length() > 4 && !request.uri.find("/cgi"))
    {
        return handleCgi(request, getCgiArgs(request.uri));
    }

    std::string resourcePath;
    std::string contentType;

    if (request.uri.find("/download") == 0)
	{
        resourcePath = "www/webpages" + request.uri;
        // Déterminer le type de contenu basé sur l'extension du fichier
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
    }
    else
	{
        resourcePath = "www/webpages" + request.uri;
        contentType = "text/html";
    }

    std::ifstream file(resourcePath, std::ios::binary);

    if (!file.is_open())
    {
        std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(clientFd, response.c_str(), response.length(), 0);
        return true;
    }

    // Creation de 2 iterateurs (debut et fin)
    // lit le contenu du fichier et le stocke dans content
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    file.close();
    
    std::string contentDisposition = request.uri.find("/download") == 0 ? "attachment; filename=" : "inline; filename=" + resourcePath.substr(resourcePath.find_last_of("/") + 1);
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + contentDisposition + contentType + "\r\nContent-Length: " + std::to_string(content.length()) + "\r\n\r\n" + content;
    
    dToSend[clientFd] = response;

    // Afficher seulement les informations essentielles pour le débogage
    std::cout << "Sending file: " << resourcePath << " with Content-Type: " << contentType << std::endl;

    return true;
}


bool handlePost(HttpRequest& request, std::map<int, std::string>& dToSend, int clientFd)
{
    std::cout << "[ POST request ]" << std::endl;
    std::string response;

    if (request.uri.length() > 4 && !request.uri.find("/cgi"))
    {
        return handleCgi(request, request.rawBody);
    }
    if (request.headers["Content-Type"].find("multipart/form-data") != std::string::npos)
    {
        request.formattedBody = MultipartFormData(request.headers["Content-Type"], request.rawBody);
    
        for (size_t i = 0; i < request.formattedBody.content.size(); i++)
        {
            if (request.formattedBody.content[i].filename.empty())
                continue;

            // Chemin complet du fichier
            std::string filePath = "www/webpages/kittenland/" + request.formattedBody.content[i].filename;
            std::ofstream file(filePath, std::ios::out | std::ios::trunc | std::ios::binary);

            if (file.is_open())
            {
                file.write(request.formattedBody.content[i].content.data(), request.formattedBody.content[i].content.size());
                file.close();

                // Vérifier si l'écriture a réussi
                if (!file)
                {
                    std::cout << "Erreur lors de l'écriture du fichier: " << filePath << std::endl;
                    response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
                    break;
                }
                response = "HTTP/1.1 200 OK\r\n\r\n";
            }
            else
            {
                std::cout << "Impossible d'ouvrir le fichier: " << filePath << std::endl;
                response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
                break;
            }
        }
    }
    else
    {
        response = "HTTP/1.1 400 Bad Request\r\n\r\n";
    }

    dToSend[clientFd] = response;
    return true;
}


bool handleDelete(HttpRequest& request, std::map<int, std::string>& dToSend, int clientFd)
{
	std::cout << "[ DELETE request ]" << std::endl;
	std::string response;

	if (request.uri.length() <= 4 || request.uri.find("/kittenland/"))
    {
		return false;
    }
    
    std::string resourcePath = "www/webpages/" + request.uri;

	if (remove(resourcePath.c_str()) != 0)
	{
    	response = "HTTP/1.1 404 Not Found\r\n\r\n";
    }
    else
    {
        response = "HTTP/1.1 200 OK\r\n\r\n";
    }
	dToSend[clientFd] = response;
	return true;
};

bool HttpRequest::HandleRequest(std::map<int, std::string>& dToSend, int clientFd)
{
	std::string requests[3] = {"GET", "POST", "DELETE"};
	bool (*handlers[3])(HttpRequest&, std::map<int, std::string>& dToSend, int clientFd) = {handleGet, handlePost, handleDelete};
	
    for (int i = 0; i < 3; i++)
	{
        	if (this->method == requests[i])
            {
			    return handlers[i](*this, dToSend, clientFd);
            }
    }
    return false;
}

HttpRequest::~HttpRequest(){}