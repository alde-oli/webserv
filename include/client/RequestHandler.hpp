#pragma once

#include "../config/ServConfig.hpp"
#include "../client/Request.hpp"
#include "../client/Response.hpp"

class RequestHandler
{
	public:
		static bool	rGet(Request &request, Response &response, ServConfig &server);
		static bool	rPost(Request &request, Response &response, ServConfig &server);
		static bool	rDel(Request &request, Response &response, ServConfig &server);

	private:
		RequestHandler() 	{};
		~RequestHandler()	{};
};
