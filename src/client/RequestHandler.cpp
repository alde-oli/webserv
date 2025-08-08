#include "../../include/libs.hpp"
#include "../../include/client/RequestHandler.hpp"
#include "../../include/client/Request.hpp"
#include "../../include/logs.hpp"

std::string getInterpreterPathForExtension(const std::string& extension)
{
	if (extension == "py") return "/usr/bin/python";
	else if (extension == "php") return "/usr/bin/php";
	else if (extension == "rb") return "/usr/bin/ruby";
	else if (extension == "pl") return "/usr/bin/perl";
	else if (extension == "sh") return "/bin/sh";
	else if (extension == "cgi") return "/usr/bin/perl";
	else if (extension == "rbw") return "/usr/bin/ruby";
	else if (extension == "tcl") return "/usr/bin/tclsh";
	return "";
}

bool isDirectory(const std::string& path)
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) 
        return false;
    return S_ISDIR(statbuf.st_mode);
}

static bool rCgi(Request &request, Response &response, ServConfig &server)
{
    std::map<std::string, std::string> envMap = request.getBody().getCgiArgs();
    std::string path = request["uri"].substr(0, request["uri"].find_last_of('/') + 1);
    Route route = server.getRoute(path);
    std::string cgiName = request["uri"].substr(request["uri"].find_last_of('/') + 1, request["uri"].find_last_of('?') - request["uri"].find_last_of('/') - 1);
    std::string cgiPath = route.getRoot() + cgiName;

    if (access(cgiPath.c_str(), X_OK) == -1)
	{
        response.setCode(404);
		std::cout << "access fail" << std::endl;
        return true;
    }

    std::string extension = cgiName.substr(cgiName.find_last_of('.') + 1);
    std::string interpreterPath = getInterpreterPathForExtension(extension);

    if (interpreterPath.empty()) {
        response.setCode(500);
		std::cout << "interpreterPath empty" << std::endl;
        return true;
    }

    std::vector<char*> env;
    for (std::map<std::string, std::string>::iterator it = envMap.begin(); it != envMap.end(); ++it) {
        std::string envVar = it->first + "=" + it->second;
        char* envCStr = new char[envVar.size() + 1];
        strcpy(envCStr, envVar.c_str());
        env.push_back(envCStr);
    }
    env.push_back(NULL);

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        response.setCode(500);
		std::cout << "pipe fail" << std::endl;
        return true;
    }

    const char* argv[] = { interpreterPath.c_str(), cgiPath.c_str(), NULL };
    pid_t pid = fork();
    if (pid == -1) {
        response.setCode(500);
        return true;
    } else if (pid == 0) { // Child process
        close(pipefd[0]); // Close read end, child will write
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
        execve(argv[0], const_cast<char* const*>(argv), const_cast<char* const*>(&env[0]));
        exit(EXIT_FAILURE); // Execve failed
    } else { // Parent process
        close(pipefd[1]); // Close write end, parent will read
        char buffer[4096]; // Adjust size as necessary
        std::string output;
        ssize_t count;
        while ((count = read(pipefd[0], buffer, sizeof(buffer)-1)) > 0) {
            buffer[count] = '\0';
            output += buffer;
        }
        close(pipefd[0]); // Close read end after done reading

        int status;
        waitpid(pid, &status, 0);
        if (status != 0) {
            response.setCode(500);
        } else {
            response.setCode(200);
            // Here you can set the CGI output to the response body
            response.setContent(output);
			response.setContentLength(output.size());
			response.setContentType("text/html; charset=UTF-8");
			response.setKeepAlive(false);
        }
    }

    for (size_t i = 0; i < env.size(); ++i) {
        delete[] env[i];
    }
    return true;
}



static std::string	extensionType(Request &request)
{
	std::string contentType;

	contentType = request["uri"].find(".html") != std::string::npos ? "text/html" :
				request["uri"].find(".css") != std::string::npos ? "text/css" :
				request["uri"].find(".js") != std::string::npos ? "application/javascript" :
				request["uri"].find(".png") != std::string::npos ? "image/png" :
				request["uri"].find(".jpg") != std::string::npos ? "image/jpeg" :
				request["uri"].find(".jpeg") != std::string::npos ? "image/jpeg" :
				request["uri"].find(".gif") != std::string::npos ? "image/gif" :
				request["uri"].find(".svg") != std::string::npos ? "image/svg+xml" :
				request["uri"].find(".ico") != std::string::npos ? "image/x-icon" :
				request["uri"].find(".mp3") != std::string::npos ? "audio/mpeg" :
				request["uri"].find(".mp4") != std::string::npos ? "video/mp4" :
				request["uri"].find(".avi") != std::string::npos ? "video/x-msvideo" :
				request["uri"].find(".pdf") != std::string::npos ? "application/pdf" :
				request["uri"].find(".zip") != std::string::npos ? "application/zip" :
				request["uri"].find(".tar") != std::string::npos ? "application/x-tar" :
				request["uri"].find(".gz") != std::string::npos ? "application/gzip" :
				request["uri"].find(".dmg") != std::string::npos ? "application/x-apple-diskimage" :
				request["uri"].find(".xml") != std::string::npos ? "application/xml" :
				request["uri"].find(".json") != std::string::npos ? "application/json" :
				request["uri"].find(".csv") != std::string::npos ? "text/csv" :
				request["uri"].find(".txt") != std::string::npos ? "text/plain" :
				"application/octet-stream";
	return (contentType);
}

bool	RequestHandler::rGet(Request &request, Response &response, ServConfig &server)
{
	std::string path = request["uri"].substr(0, request["uri"].find_last_of('/') + 1); //need to check it is correctly truncated
	if (!server.isRoute(path))
		{response.setCode(404); return true;}
	
	Route route = server.getRoute(path);

	if (route.isRedir())
		{response.setCode(302);
		response.setLocation(route.getRedirDir());
		response.setKeepAlive(false);
		return true;}

	std::vector<std::string> methods = route.getMethods();
	if (methods.size() == 0 || std::find(methods.begin(), methods.end(), "GET") == methods.end())
		{response.setCode(405); return true;}
	
	std::string toGet = request["uri"].substr(request["uri"].find_last_of('/') + 1); // need to check it is correctly truncated
	if (toGet.empty())
		{if (route.isListing())
			{response.setCode(200);
			response.setContentType("text/html; charset=UTF-8");
			response.setContentLength(route.list().size());
			response.setContent(route.list());
			response.setKeepAlive(false);
			return true;}
		else if (!route.getDefaultPage().empty())
			{response.setCode(200);
			response.setContentType("text/html; charset=UTF-8");
			std::string pagePath = route.getDefaultPage();

			std::ifstream file(pagePath.c_str());
			if (!file.is_open())
				{response.setCode(500); return true;}

			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			response.setContent(content);
			response.setContentLength(content.size());
			response.setKeepAlive(false);
			return true;}
		else
			{response.setCode(404); return true;}}

	std::string args;
	std::string extension;
	if (toGet.find_last_of('?') != std::string::npos)
	{
		args = toGet.substr(toGet.find_last_of('?') + 1);
		request.setCgiArgs(args);
		extension = toGet.substr(toGet.find_last_of('.'), toGet.find_last_of('?') - toGet.find_last_of('.'));
	}
	if (toGet.find_last_of('?') == std::string::npos && toGet.find_last_of('.') != std::string::npos)
		extension = toGet.substr(toGet.find_last_of('.'));
	else if (toGet.find_last_of('?') != std::string::npos && toGet.find_last_of('.') != std::string::npos)
		extension = toGet.substr(toGet.find_last_of('.'), toGet.find_last_of('?') - toGet.find_last_of('.'));
	if (route.isCgi(extension))
		{return rCgi(request, response, server);}
	toGet = route.getRoot() + toGet;

	if (isDirectory(toGet))
		{response.setCode(404); return true;}

	std::ifstream file(toGet.c_str(), std::ios::binary | std::ios::in);
	if (!file.is_open())
		{response.setCode(404); return true;}
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	if (static_cast<long long>(content.size()) > static_cast<long long>(server.getMaxBodySize()))
		{response.setCode(413); return true;}
	response.setCode(200);
	response.setContent(content);
	response.setContentLength(content.size());
	response.setKeepAlive(false);
	if (route.isUpload())
	{
		response.setContentDisposition("attachment");
		response.setContentType(extensionType(request));
	}
	return true;
}


bool	RequestHandler::rPost(Request &request, Response &response, ServConfig &server)
{
	std::string path = request["uri"].substr(0, request["uri"].find_last_of('/') + 1); //need to check it is correctly truncated

	if (!server.isRoute(path))
		{response.setCode(404); return true;}
	
	Route route = server.getRoute(path);

	if (route.isRedir())
		{response.setCode(302);
		response.setLocation(route.getRedirDir());
		response.setKeepAlive(false);
		return true;}

	std::vector<std::string> methods = route.getMethods();
	if (methods.size() == 0 || std::find(methods.begin(), methods.end(), "POST") == methods.end())
		{response.setCode(405); return true;}
	
	std::string cgi = request["uri"].substr(request["uri"].find_last_of('/') + 1);
	if (!cgi.empty())
		return rCgi(request, response, server);
	
	if (!route.isDownload())
		{response.setCode(405); return true;}
	
	if (request["Content-Type"].find("multipart/form-data") == std::string::npos)
		{response.setCode(400); return true;}
	
	std::vector<contentData> files = request.getBody().getFiles();
	for (size_t i = 0; i < files.size(); i++)
	{
		std::string path = route.getDownloadDir() + files[i]._fileName;
		std::ofstream file(path.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
		if (!file.is_open())
			{response.setCode(500); return true;}
		file.write(files[i]._data.c_str(), files[i]._data.size());
		file.close();

		response.setCode(201);
		response.setKeepAlive(false);
		response.setContentLength(0);
	}
	return true;
}


bool	RequestHandler::rDel(Request &request, Response &response, ServConfig &server)
{
	std::string path = request["uri"].substr(0, request["uri"].find_last_of('/') + 1); //need to check it is correctly truncated

	if (!server.isRoute(path))
		{response.setCode(404); return true;}
	
	Route route = server.getRoute(path);

	if (route.isRedir())
		{response.setCode(302);
		response.setLocation(route.getRedirDir());
		return true;}

	std::vector<std::string> methods = route.getMethods();
	if (methods.size() == 0 || std::find(methods.begin(), methods.end(), "DELETE") == methods.end())
		{response.setCode(405); return true;}

	std::string toDel = request["uri"].substr(request["uri"].find_last_of('/') + 1); // need to check it is correctly truncated
	if (toDel == "")
		{response.setCode(400); return true;}

	toDel = route.getRoot() + toDel;

	if (std::remove(toDel.c_str()) != 0)
		{response.setCode(500); return true;}
	response.setCode(204);
	response.setKeepAlive(false);
	return true;
}
