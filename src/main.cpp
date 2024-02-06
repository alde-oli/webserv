#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"
#include "../include/Route.hpp"
#include "../include/Cgi.hpp"
#include "../include/MultipartFormData.hpp"
#include "../include/htmlrequest.hpp"

// Prototypes des fonctions

int setupServerSocket(ServerConfig &server);
int handleNewConnection(int server_fd, int kq, std::map<int, time_t>& clientActivity);
bool handleClientRequest(int client_fd, std::map<int, time_t>& clientActivity, std::map<int, Response>& clientDataToSend);
void checkClientTimeouts(std::map<int, time_t>& clientActivity, int timeout);
int registerWriteEvent(int kq, int fd);
int handleClientWrite(int fd, std::map<int, Response>& clientActivity);
int unregisterWriteEvent(int kq, int fd);

static int close_and_perror(char *str, int fd)
{
	perror(str);

	if (fd)
		close (fd);

	return (-1);
}

static timeval ft_timeout(int sec, int usec)
{
	struct timeval sendTimeout;
	
	// Timeout en secondes
	sendTimeout.tv_sec = sec;
	// Timeout en microsecondes
	sendTimeout.tv_usec = usec;
	return (sendTimeout);
}

static timespec precise_ft_timeout(int sec, int nsec)
{
	struct timespec timeout;

	timeout.tv_sec = sec;
	timeout.tv_nsec = nsec;
	return (timeout);
}

int runServer(std::vector<ServerConfig> &servers)
{
	for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		int server_fd = setupServerSocket(*it);
		if (server_fd < 0)
		{
			for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
				close(it->getFd());
			return -1;
		}
		it->setFd(server_fd);
	}

	signal(SIGPIPE, SIG_IGN);

	struct timeval sendTimeout = ft_timeout(300, 0);
	struct timespec timeout = precise_ft_timeout(30, 0);
	std::map<int, time_t> clientActivity;
	const int idle_timeout = 10;

	// Définir le timeout d'envoi sur le socket
	for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		int val = 1;
		setsockopt(it->getFd(), SOL_SOCKET, SO_NOSIGPIPE, &val, sizeof(val));
		if (setsockopt(it->getFd(), SOL_SOCKET, SO_SNDTIMEO, (char *)&sendTimeout, sizeof(sendTimeout)) < 0)
		{
			for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
				close(it->getFd());
			return -1;
		}
	}

	// Créer une kqueue
	int kq = kqueue();
	if (kq == -1)
		return (close_and_perror("kqueue", 0));
	struct kevent kev;
	for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		EV_SET(&kev, it->getFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1)
		{
			for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
				close(it->getFd());
			return (close_and_perror("kevent add server_fd", 0));
		}
	}

	std::map<int, Response> dataToSend;
	std::map<int, int> client_fd_to_server_fd;

	while (true)
	{
		struct kevent events[128];
		int nev = kevent(kq, NULL, 0, events, 128, &timeout);

		if (nev < 0)
			return (close_and_perror("kevent wait", 0));
		for (int i = 0; i < nev; i++)
		{
			int fd = events[i].ident;

			if (events[i].filter == EVFILT_READ)
			{
				bool is_new = false;
				for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
					if (fd == it->getFd())
						is_new = true;
				if (is_new)
				{
					// Gérer une nouvelle connexion
					int client_fd = handleNewConnection(fd, kq, clientActivity);
					client_fd_to_server_fd[client_fd] = fd;
					if (client_fd != -1)
					{
						struct kevent client_event;
						EV_SET(&client_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					}
				}
				else
				{
					bool hasDataToSend = handleClientRequest(client_fd_to_server_fd[fd], clientActivity, dataToSend);
					if (hasDataToSend)
						registerWriteEvent(kq, fd); // Préparer l'événement, appliqué dans le prochain passage
				}
			}
			else if (events[i].filter == EVFILT_WRITE)
			{
				// Gérer l'envoi de données au client
				if (handleClientWrite(fd, dataToSend) == -1)
					unregisterWriteEvent(kq, fd); // Préparer la désinscription, appliquée dans le prochain passage
			}
		}
		checkClientTimeouts(clientActivity, idle_timeout);
	}
	// Fermer le socket serveur
	for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
		close(it->getFd());
	return 0;
}

int setupServerSocket(ServerConfig &server)
{
	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Ignorer SIGPIPE
	signal(SIGPIPE, SIG_IGN);
	// Créer le socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		return (close_and_perror("socket failed", 0));
	// Rendre le socket non bloquant
	int flags = fcntl(server_fd, F_GETFL, 0);
	if (flags == -1)
		return (close_and_perror("fcntl F_GETFL", server_fd));
	flags |= O_NONBLOCK;
	if (fcntl(server_fd, F_SETFL, flags) == -1)
		return (close_and_perror("fcntl F_SETFL O_NONBLOCK", server_fd));
	// Définir l'adresse du serveur
	address = server.getServerAddr();
	// Lier le socket
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		return (close_and_perror("bind failed", server_fd));
	// Mettre le serveur en mode écoute
	if (listen(server_fd, 3) < 0)
		return (close_and_perror("listen", server_fd));
	return server_fd;
}

int handleNewConnection(int server_fd, int kq, std::map<int, time_t>& clientActivity)
{
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	struct kevent change_event;

	int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	
	if (new_socket < 0)
		return (close_and_perror("accept", 0));

	EV_SET(&change_event, new_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	
	if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1)
		return (close_and_perror("kevent add new socket", new_socket));
	else
		clientActivity[new_socket] = time(nullptr);
	return new_socket;
}

void checkClientTimeouts(std::map<int, time_t>& clientActivity, int timeout)
{
	time_t now = time(nullptr);
	for (std::__1::map<int, time_t>::iterator it = clientActivity.begin(); it != clientActivity.end(); )
	{
		if (now - it->second > timeout)
		{
			close(it->first);
			it = clientActivity.erase(it);  // Supprimer le client et avancer l'itérateur
		}
		else
			++it;
	}
}
