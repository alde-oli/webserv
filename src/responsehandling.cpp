#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"
#include "../include/Route.hpp"
#include "../include/Cgi.hpp"
#include "../include/MultipartFormData.hpp"
#include "../include/HtmlRequest.hpp"

int registerWriteEvent(int kq, int fd)
{
	struct kevent kev;
	EV_SET(&kev, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1)
		return (close_and_perror("registerWriteEvent: kevent", 0));
	return 0;
}

int handleClientWrite(int fd, std::map<int, std::string>& clientActivity)
{
	if (clientActivity.find(fd) == clientActivity.end() || clientActivity[fd].empty())
		return -1;

	std::string dataToSend = clientActivity[fd];
	ssize_t bytesSent = send(fd, dataToSend.c_str(), dataToSend.size(), 0);

	if (bytesSent < 0)
		return (close_and_perror("send", 0));
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
		return (close_and_perror("unregisterWriteEvent: kevent", 0));
	return 0;
}