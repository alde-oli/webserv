#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"
#include "../include/Route.hpp"
#include "../include/MultipartFormData.hpp"
#include "../include/htmlrequest.hpp"

void handleClientRequest(ServerConfig server, int client_fd, std::map<int, time_t>& clientActivity, Response &clientDataToSend)
{

	char 		buffer[1024] = {0};

	ssize_t valread = read(client_fd, buffer, 1024);

	static int	to_read = 0;
	static bool	is_post_body  = 0;

	if (valread > 0)
	{
		std::string buf2(buffer, valread);
		static HttpRequest	request;

		if (!is_post_body)
		{
			request = HttpRequest(buf2, client_fd);
		
			int file_fd = open("request.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
			if (file_fd != -1)
			{
				write(file_fd, buf2.c_str(), buf2.size());
				close(file_fd);
			}
			if (request.getMethod() == "POST")
			{
				is_post_body = 1;
				to_read = request.getheaders()["Content-Length"].empty() ? 0 : std::atoi(request.getheaders()["Content-Length"].c_str());
				to_read -= request.getRawBody().size();
				if (to_read <= 0)
				{
					is_post_body = 0;
					to_read = 0;
				}
			}
		}
		else
		{
			request.setRawBody(request.getRawBody() + buf2);
			to_read -= valread;
			if (to_read <= 0)
			{
				is_post_body = 0;
				to_read = 0;
			}
		}
		// determiner le serveur a utiliser
		if (!is_post_body)
			request.HandleRequest(clientDataToSend, client_fd, server); // ligne qui fait tout peter
		clientActivity[client_fd] = time(nullptr);  // Mettre à jour l'heure de la dernière activité

	}
	else if (valread == 0)
	{
		close(client_fd);
		clientActivity.erase(client_fd);  // Supprimer le client de la map
	}
	else
	{
		close (client_fd);
		perror("read");
		clientActivity.erase(client_fd);  // Supprimer le client de la map
	}
}