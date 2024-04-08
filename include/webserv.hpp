#pragma once

#include "libs.hpp"
#include "SmartTypes.hpp"
#include "config/ServConfig.hpp"
#include "config/Route.hpp"
#include "config/Codes.hpp"
#include "client/Data.hpp"
#include "client/Request.hpp"
#include "client/Response.hpp"
#include "client/Client.hpp"
#include "ServRunner.hpp"
void signalHandler(int signum);
#include "logs.hpp"

//timeouts
//acceptable to for inactive are 60-120s
#define INACTIVE_TO 360
//acceptable to for kevent are 3-10s
#define KEVENT_TO 60
//acceptable to for sread and write are 5-30s
#define SND_TO 60
#define RECEPT_TO 60
#define ACCEPT_TO 60

//memory|perfs limits
#define MAX_EVENTS 4096
#define BUF_SIZE 4096

#define LOCALHOST "127.0.0.1"
#define EXTERNAL_IP "11.10.14.5"
