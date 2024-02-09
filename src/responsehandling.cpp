#include "../include/publiclib.hpp"
#include "../include/ServerConfig.hpp"
#include "../include/Error.hpp"
#include "../include/parsing.hpp"
#include "../include/Route.hpp"
#include "../include/MultipartFormData.hpp"
#include "../include/htmlrequest.hpp"

//changer pour juste deconnecter le client si erreur

int registerWriteEvent(int kq, int fd)
{
	struct kevent kev;
	EV_SET(&kev, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	kevent(kq, &kev, 1, NULL, 0, NULL);
	// if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1)
	// 	cerr_and_exit("registerWriteEvent: kevent", NULL, 0); return 0
	return 0;
}

int unregisterWriteEvent(int kq, int fd)
{
    if (kq < 0 || fd < 0) {
        std::cerr << "Invalid file descriptor" << std::endl;
        return -1;
    }

    struct kevent kev;
    EV_SET(&kev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);

    if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1) {
        cerr_and_exit("unregisterWriteEvent: kevent", NULL, 0);
    }

    return 0;
}

int handleClientWrite(Response& dataToSend, int fd, int kq)
{
    if (fd < 0 || kq < 0) {
        std::cerr << "Invalid file descriptor" << std::endl;
        return -1;
    }

    std::string response = dataToSend.getResponse();
    if (response.empty()) {
        std::cerr << "No data to send" << std::endl;
        return -1;
    }

    ssize_t ret = send(fd, response.c_str(), response.size(), 0);
    if (ret == -1) {
        std::cerr << "Error sending data" << std::endl;
        return -1;
    }

    if (static_cast<size_t>(ret) < response.size()) {
        dataToSend.setResponse(response.substr(ret));
    } else {
        dataToSend.setResponse("");
        if (unregisterWriteEvent(kq, fd) == -1) {
            std::cerr << "Error unregistering write event" << std::endl;
            return -1;
        }
    }

    return 0;
}
