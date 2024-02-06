#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"
#include "../include/Route.hpp"
#include "../include/Cgi.hpp"
#include "../include/MultipartFormData.hpp"
#include "../include/htmlrequest.hpp"

bool handleClientRequest(int client_fd, std::map<int, time_t>& clientActivity, std::map<int, Response>& clientDataToSend)
{
	char 		buffer[1024] = {0};

	ssize_t valread = read(client_fd, buffer, 1024);

	static int	to_read = 0;
	static bool	is_post_body  = 0;
	bool		hasDataToSend = false;

	if (valread > 0)
	{
		std::string buf2(buffer, valread);
		static HttpRequest	request;

		if (!is_post_body)
		{
			request = HttpRequest(buf2, client_fd);
			if (request.method == "POST")
			{
				is_post_body = 1;
				to_read = request.headers["Content-Length"].empty() ? 0 : std::atoi(request.headers["Content-Length"].c_str());
				to_read -= request.rawBody.size();
				if (to_read <= 0)
				{
					is_post_body = 0;
					to_read = 0;
				}
			}
		}
		else
		{
			request.rawBody += buf2;
			to_read -= valread;
			if (to_read <= 0)
			{
				is_post_body = 0;
				to_read = 0;
			}
		}
		// determiner le serveur a utiliser
		if (!is_post_body)
			hasDataToSend = request.HandleRequest(clientDataToSend, client_fd);
		clientActivity[client_fd] = time(nullptr);  // Mettre à jour l'heure de la dernière activité

	}
	else if (valread == 0)
	{
		close(client_fd);
		clientActivity.erase(client_fd);  // Supprimer le client de la map
	}
	else
	{
		close_and_perror("read", client_fd);
		clientActivity.erase(client_fd);  // Supprimer le client de la map
	}
	return hasDataToSend;
}