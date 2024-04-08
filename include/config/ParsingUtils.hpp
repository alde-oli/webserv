#pragma once

#include "../libs.hpp"
#include "ServConfig.hpp"

#define LOCALHOST "127.0.0.1"
#define EXTERNAL_IP ""

void	CerrExit(const char* message, std::string precision);
bool	isLocalOrExternal(struct sockaddr_in addr);
bool	checkIPServer(ServConfig server);