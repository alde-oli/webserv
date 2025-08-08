#include "../include/webserv.hpp"

//main function to initiate the server, handle events, and manage clients
void	ServRunner::run(std::vector<ServConfig> &servers)
{
	//initiate server sockets
	setSockets(servers);
	//map to store clients bound to their fd
	std::map<int, Client> clients;
	//vector to store pollfd structures
	std::vector<struct pollfd> pollfds;
	//setup initial poll structures with server sockets
	setupPoll(pollfds, servers);
	
	//main loop, everything happens here
	std::cout << "✅ [Server is running...]" << std::endl;
	while (true)
	{
		int event = poll(&pollfds[0], pollfds.size(), POLL_TO);
		if (event < 0)
			{ERRLOG("poll() fail") continue;}
		if (event == 0)
			{ServRunner::checkTimeouts(clients); continue;} // timeout, check for inactive clients
		
		for (size_t i = 0; i < pollfds.size(); i++)
		{
			if (pollfds[i].revents == 0)
				continue;
				
			EXECLOG("event " << i << " fd:" << pollfds[i].fd << " revents:" << pollfds[i].revents)

			if (pollfds[i].revents & (POLLHUP | POLLERR)) //client closed connection or error
			{
				if (clients.find(pollfds[i].fd) != clients.end())
				{
					clients.erase(pollfds[i].fd);
					CONNECTLOG("client " << pollfds[i].fd << " closed")
				}
				pollfds.erase(pollfds.begin() + i);
				i--; // adjust index after erase
				continue;
			}
			//analysing events
			else if (pollfds[i].revents & POLLIN)
			{
				EXECLOG("read event")
				int newClient = 0;
				//find out if new client or existing client then accept new client or read client requests
				for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); it++)
				{
					if (pollfds[i].fd == it->getSocketFd())
					{
						ServRunner::acceptNew(pollfds, it->getSocketFd(), clients, *it); 
						newClient = 1; 
						break;
					}
				} //accept new client
				if (!newClient)
				{
					for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); it++)
					{
						if (it->getSocketFd() == clients[pollfds[i].fd].getServFd())
						{
							if (clients[pollfds[i].fd].read(*it, pollfds)) //read client request, return 1 if client needs to be closed
							{
								if (clients[pollfds[i].fd].isResponse())
								{
									clients[pollfds[i].fd].setWriteEvent(pollfds);
								}
								else
								{
									clients.erase(pollfds[i].fd); 
									CONNECTLOG("client " << pollfds[i].fd << " closed")
									pollfds.erase(pollfds.begin() + i);
									i--; // adjust index after erase
									break;
								}
							}
							break;
						}
					}
				}
			}
			else if (pollfds[i].revents & POLLOUT)
			{
				if (clients[pollfds[i].fd].write(pollfds))
				{
					clients.erase(pollfds[i].fd); 
					CONNECTLOG("client " << pollfds[i].fd << " closed")
					pollfds.erase(pollfds.begin() + i);
					i--; // adjust index after erase
				}
				EXECLOG("write event") //write client response
			}
			else
				ERRLOG("unknown event")
		}
		ServRunner::checkTimeouts(clients); //check last clients activity
	}
}


//accepts new client and adds it to the clients map
void	ServRunner::acceptNew(std::vector<struct pollfd> &pollfds, int serverFd, std::map<int, Client> &clients, ServConfig &server)
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

	//add client read event to poll
	struct pollfd clientPollfd;
	clientPollfd.fd = clientFd;
	clientPollfd.events = POLLIN; // initially only interested in read events
	clientPollfd.revents = 0;
	pollfds.push_back(clientPollfd);

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
			{ perror("bind() failed"); std::cerr << "bind() failed for " << it->getId() << ". exiting program"  << std::endl; exit(1);}
		if (listen(it->getSocketFd(), 20) < 0)
			{CERRANDEXIT std::cerr << "listen() failed for " << it->getId() << ". exiting program" << std::endl; exit(1);}
}	}


//initiates poll structures and adds server sockets to it
void	ServRunner::setupPoll(std::vector<struct pollfd> &pollfds, std::vector<ServConfig> &servers)
{
	//add server sockets to poll
	for (std::vector<ServConfig>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		struct pollfd serverPollfd;
		serverPollfd.fd = it->getSocketFd();
		serverPollfd.events = POLLIN; // only interested in read events for server sockets (new connections)
		serverPollfd.revents = 0;
		pollfds.push_back(serverPollfd);
	}
}
