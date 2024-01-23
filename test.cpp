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

#define PORT 8080


#include "htmlrequest.hpp"

// Prototypes des fonctions

int setupServerSocket();
void handleNewConnection(int server_fd, int kq, std::map<int, time_t>& clientActivity);
void handleClientRequest(int client_fd, const std::string& httpResponse, std::map<int, time_t>& clientActivity);
std::string prepareHttpResponse();
void checkClientTimeouts(std::map<int, time_t>& clientActivity, int timeout);

int main()
{
	int server_fd = setupServerSocket();
	if (server_fd < 0) return -1;

	signal(SIGPIPE, SIG_IGN);

	struct timeval sendTimeout;
	sendTimeout.tv_sec = 300;  // Timeout en secondes
	sendTimeout.tv_usec = 0; // Timeout en microsecondes

	// Définir le timeout d'envoi sur le socket
	int val = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_NOSIGPIPE, &val, sizeof(val));
	if (setsockopt(server_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&sendTimeout, sizeof(sendTimeout)) < 0)
	{
		perror("setsockopt");
		return -1;
	}

	std::string httpResponse = prepareHttpResponse();

	// Créer une kqueue
	int kq = kqueue();
	if (kq == -1)
	{
		perror("kqueue");
		return -1;
	}

	struct kevent change_event;
	EV_SET(&change_event, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

	// Ajouter l'événement à la kqueue
	if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1)
	{
		perror("kevent add");
		return -1;
	}

	struct timespec timeout;
	timeout.tv_sec = 30;  // Réduire pour une vérification plus fréquente
	timeout.tv_nsec = 0;

	std::map<int, time_t> clientActivity;
	const int idle_timeout = 10;  // Timeout d'inactivité en secondes

	while (true) {
		struct kevent events[32];
		int nev = kevent(kq, NULL, 0, events, 32, &timeout);
		std::cout << "new event" << std::endl;
		if (nev == -1)
		{
			perror("kevent wait");
			return -1;
		}

		for (int i = 0; i < nev; i++) {
			if (events[i].ident == server_fd)
			{
				handleNewConnection(server_fd, kq, clientActivity);
			} else
			{
				handleClientRequest(events[i].ident, httpResponse, clientActivity);
			}
		}

		checkClientTimeouts(clientActivity, idle_timeout);
	}

	// Fermer le socket serveur
	close(server_fd);

	return 0;
}

int setupServerSocket()
{
	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Créer le socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		return -1;
	}

	// Définir l'adresse du serveur
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(PORT);

	// Lier le socket
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		return -1;
	}

	// Mettre le serveur en mode écoute
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		return -1;
	}

	std::cout << "Server listening on port 8080" << std::endl;
	return server_fd;
}

std::string prepareHttpResponse()
{
	std::ifstream file("index.html");
	if (!file.is_open())
	{
		perror("file open");
		return "";
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	std::string contentLength = std::to_string(content.length());

	return "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + contentLength + "\r\n\r\n" +
		content;
}

void handleNewConnection(int server_fd, int kq, std::map<int, time_t>& clientActivity)
{
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	struct kevent change_event;

	int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	if (new_socket < 0)
	{
		perror("accept");
		return;
	}

	std::cout << "New client connected, socket: " << new_socket << std::endl;

	EV_SET(&change_event, new_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1)
	{
		perror("kevent add new socket");
		close(new_socket);
	}
	else
	{
		clientActivity[new_socket] = time(nullptr);  // Enregistrer l'heure de la dernière activité
	}
}

void handleClientRequest(int client_fd, const std::string& httpResponse, std::map<int, time_t>& clientActivity)
{
	char buffer[1024] = {0};
	std::cout << "Client request, socket: " << client_fd << std::endl;
	ssize_t valread = read(client_fd, buffer, 1024);

	if (valread > 0)
	{
		std::string buf2;
		buf2 = buffer;
		HttpRequest request(buf2, client_fd);



		// Display request in entire
		std::cout << std::endl << "Client request in class : " << std::endl;
		std::cout << "method : " << request.method << std::endl;
		std::cout << "uri : " << request.uri << std::endl;
		std::cout << "httpVersion : " << request.httpVersion << std::endl;
		std::map<std::string, std::string>::iterator it;
		for (it = request.headers.begin(); it != request.headers.end(); it++)
		{
			std::cout << it->first << " <-> " << it->second << std::endl;
		}
		std::cout << "body : " << request.body << std::endl;

		request.HandleRequest();
		clientActivity[client_fd] = time(nullptr);  // Mettre à jour l'heure de la dernière activité

	}
	else if (valread == 0)
	{
		std::cout << "Client disconnected, socket: " << client_fd << std::endl;
		close(client_fd);
		clientActivity.erase(client_fd);  // Supprimer le client de la map
	}
	else
	{
		perror("read");
		close(client_fd);
		clientActivity.erase(client_fd);  // Supprimer le client de la map
	}
}

void checkClientTimeouts(std::map<int, time_t>& clientActivity, int timeout)
{
	time_t now = time(nullptr);
	for (std::__1::map<int, time_t>::iterator it = clientActivity.begin(); it != clientActivity.end(); )
	{
		if (now - it->second > timeout)
		{
			std::cout << "Client timed out, socket: " << it->first << std::endl;
			close(it->first);
			it = clientActivity.erase(it);  // Supprimer le client et avancer l'itérateur
		}
		else
		{
			++it;
		}
	}
}
