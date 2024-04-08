#include "../include/webserv.hpp"

//main function to initiate the server, handle events, and manage clients
void	ServRunner::run(std::vector<ServConfig> &servers)
{
	AutoFD kq;
	//initiate server sockets and kqueue
	setSockets(servers);
	setKqueue(kq, servers);
	//map to store clients bound to their fd
	std::map<int, Client> clients;
	//set timeout for kqueue
	timespec kqTimeout = {0, KEVENT_TO};
	//stores events
	struct kevent events[MAX_EVENTS];
	//main loop, everything happens here
	std::cout << "✅ [Server is running...]" << std::endl;
	while (true)
	{
		int event = kevent(kq.get(), NULL, 0, events, MAX_EVENTS, &kqTimeout);
		if (event < 0)
			{ERRLOG("kevent() fail") continue;}
		for (int i = 0; i < event; i++)
		{
			EXECLOG("event " << i << " " << events[i].ident << " " << events[i].filter << " " << events[i].flags)

			if (events[i].flags & EV_EOF) //client closed connection
				{clients.erase(events[i].ident);
					CONNECTLOG("client " << events[i].ident << " closed") continue;}
			//analysing events
			else if (events[i].filter == EVFILT_READ)
			{
				EXECLOG("read event")
				int newClient = 0;
				//find out if new client or existing client then accept new client or read client requets
				for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); it++)
					{if (events[i].ident == static_cast<uintptr_t>(it->getSocketFd()))
						{ServRunner::acceptNew(kq.get(), it->getSocketFd(), clients, *it); newClient = 1; break;}} //accept new client
				if (!newClient)
					{for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); it++)
						{if (it->getSocketFd() == clients[events[i].ident].getServFd())
						{
							if (clients[events[i].ident].read(*it, kq.get())) //read client request, return 1 if client needs to be closed
								{if (clients[events[i].ident].isResponse())
									{clients[events[i].ident].setWriteEvent(kq.get());}
								else
									{clients.erase(events[i].ident); CONNECTLOG("client " << events[i].ident << " closed") break;}}
							break;
			}	}	}	}
			else if (events[i].filter == EVFILT_WRITE)
				{if (clients[events[i].ident].write(kq.get()))
					{clients.erase(events[i].ident); CONNECTLOG("client " << events[i].ident << " closed")}
				EXECLOG("write event")} //write client response
			else
				ERRLOG("unknown event")
			ServRunner::checkTimeouts(clients); //check last clients activity
}	}	}


//accepts new client and adds it to the clients map
void	ServRunner::acceptNew(int kq, int serverFd, std::map<int, Client> &clients, ServConfig &server)
{
	(void)server;
	EXECLOG("new connection request")
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	//accept new client
	int clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (clientFd < 0)
		{ERRLOG("accept() failed") return;}
	//set client fd to non-blocking
	int flags = fcntl(clientFd, F_GETFL, 0);
	if (flags < 0 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) < 0)
		{ERRLOG("fcntl() failed") close(clientFd); return;}

	//set reception timeout
	struct timeval receptTimeout; receptTimeout.tv_sec = RECEPT_TO; receptTimeout.tv_usec = 0;
	if (setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&receptTimeout, sizeof(receptTimeout)) < 0)
		{ERRLOG("setsockopt(SO_RCVTIMEO) failed") close(clientFd); return;}
	//set snd timeout
	struct timeval sndTimeout; sndTimeout.tv_sec = SND_TO; sndTimeout.tv_usec = 0;
	if (setsockopt(clientFd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&sndTimeout, sizeof(sndTimeout)) < 0)
		{ERRLOG("setsockopt(SO_SNDTIMEO) failed") close(clientFd); return;}

	//add client read event to kqueue
	struct kevent ev;
	EV_SET(&ev, clientFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(kq, &ev, 1, NULL, 0, NULL) < 0)
		{ERRLOG("kevent() failed") close(clientFd); return;}
	//add client write event to kqueue
	EV_SET(&ev, clientFd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
	if (kevent(kq, &ev, 1, NULL, 0, NULL) < 0)
		{ERRLOG("kevent() failed") close(clientFd); return;}

	//add client to clients map
	clients[clientFd] = Client(-1, clientAddr, serverFd);
	clients[clientFd].setClientFd(clientFd);
	CONNECTLOG("new client accepted")
	CONNECTLOG(clients[clientFd])
}


//checks if clients have been inactive for duration INACTIVE_TO and closes them if they have
void ServRunner::checkTimeouts(std::map<int, Client>& clients)
{
	std::time_t now = std::time(NULL);

	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end();)
	{
		if (now - it->second.getLastActivity() > INACTIVE_TO)
			{CONNECTLOG("client timed out") clients.erase(it++);}
		else
			++it;
}	}


//initiates server sockets
void	ServRunner::setSockets(std::vector<ServConfig> &servers)
{
	for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		//init socket
		it->setSocketFd(socket(AF_INET, SOCK_STREAM, 0));
		if (it->getSocketFd() < 0)
			{CERRANDEXIT std::cerr << "socket() failed for " << it->getId() << ". exiting program" << std::endl; exit(1);}
		//set socket as reusing address
		int opt = 1;
		if (setsockopt(it->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			{CERRANDEXIT std::cerr << "setsockopt() failed for " << it->getId() << ". exiting program" << std::endl; exit(1);}
		//set socket to non-blocking
		int flags = fcntl(it->getSocketFd(), F_GETFL, 0);
		if (flags < 0 || fcntl(it->getSocketFd(), F_SETFL, flags | O_NONBLOCK) < 0)
			{CERRANDEXIT std::cerr << "fcntl() failed for " << it->getId() << ". exiting program" << std::endl; exit(1);}
		//set recept timeout
		struct timeval receptTimeout; receptTimeout.tv_sec = ACCEPT_TO; receptTimeout.tv_usec = 0;
		if (setsockopt(it->getSocketFd(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&receptTimeout, sizeof(receptTimeout)) < 0)
			{CERRANDEXIT std::cerr << "setsockopt(SO_RCVTIMEO) failed for " << it->getId() << ". Exiting program." << std::endl; exit(1);}
		//bind and listen
		if (bind(it->getSocketFd(), reinterpret_cast<const struct sockaddr*>(&it->getAddr()), sizeof(it->getAddr())) < 0)
			{ std::cerr << "bind() failed for " << it->getId() << ". exiting program"  << std::endl; exit(1);}
		if (listen(it->getSocketFd(), 20) < 0)
			{CERRANDEXIT std::cerr << "listen() failed for " << it->getId() << ". exiting program" << std::endl; exit(1);}
}	}


//initiates kqueue for read and write events and adds server sockets to it
void	ServRunner::setKqueue(AutoFD &kq, std::vector<ServConfig> &servers)
{
	//init kqueue
	kq.set(kqueue());
	if (kq.get() < 0)
		{CERRANDEXIT std::cerr << "kqueue() failed. exiting program" << std::endl; exit(1);}
	//add server sockets to kqueue
	for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		struct kevent ev;
		//add read event only because servers_fd just accept new clients
		EV_SET(&ev, it->getSocketFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		if (kevent(kq.get(), &ev, 1, NULL, 0, NULL) < 0)
			{CERRANDEXIT std::cerr << "kevent() failed for " << it->getId() << ". exiting program" << std::endl; exit(1);}
}	}
