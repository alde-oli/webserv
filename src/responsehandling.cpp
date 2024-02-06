#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"
#include "../include/Route.hpp"
#include "../include/Cgi.hpp"
#include "../include/MultipartFormData.hpp"
#include "../include/htmlrequest.hpp"

//changer pour juste deconnecter le client si erreur

int registerWriteEvent(int kq, int fd)
{
	struct kevent kev;
	EV_SET(&kev, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1)
		cerr_and_exit("registerWriteEvent: kevent", NULL, 0);
	return 0;
}

int handleClientWrite(int fd, std::map<int, Response>& clientActivity)
{
	if (clientActivity.find(fd) == clientActivity.end() || clientActivity[fd].empty())
		return -1;

	std::string dataToSend = clientActivity[fd];
	ssize_t bytesSent = send(fd, dataToSend.c_str(), dataToSend.size(), 0);

	if (bytesSent < 0)
		cerr_and_exit("send", NULL, 0);
	if (bytesSent < static_cast<ssize_t>(dataToSend.size()))
	{
		clientActivity[fd] = dataToSend.substr(bytesSent);
		return 0;
	}
	else
	{
		clientActivity.erase(fd);
		return -1;
	}
}

int unregisterWriteEvent(int kq, int fd)
{
	struct kevent kev;
	EV_SET(&kev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

	if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1)
		cerr_and_exit("unregisterWriteEvent: kevent", NULL, 0);
	return 0;
}