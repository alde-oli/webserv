#pragma once

#include "libs.hpp"
#include "config/ServConfig.hpp"
#include "client/Client.hpp"

class ServRunner
{
	public:
		///////////////////////////////
		//Constructors and destructor//
		///////////////////////////////

		//////////////////////
		//operators overload//
		//////////////////////

		////////////////////
		//member functions//
		////////////////////

	static void	run(std::vector<ServConfig> &servers);
	static void	acceptNew(std::vector<struct pollfd> &pollfds, int serverFd, std::map<int, Client> &clients, ServConfig &server);
	static void	setSockets(std::vector<ServConfig> &servers);
	static void	setupPoll(std::vector<struct pollfd> &pollfds, std::vector<ServConfig> &servers);
	static void	checkTimeouts(std::map<int, Client> &clients);
};