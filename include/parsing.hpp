#pragma once

#include "publiclib.hpp"
#include "ServerConfig.hpp"

//parsing utils
in_addr_t					setIPv4(const std::string &addr);
in_port_t					setPort(const std::string &port);
std::string					setFileStr(const std::string &file);
std::string					setDirStr(const std::string &dir);
std::vector<std::string>	setCgiExtension(const std::string &cgiExtension);
std::vector<std::string>	setMethod(const std::string& method);
bool						setBool(std::string boolean);
long long int				setBodySize(std::string size);
bool                        isRouteValid(const std::string& chemin);
void						cerr_and_exit(std::string msg, std::string support, int code);

bool 						tryPath(const std::string path);

void setServer(std::fstream &file, std::string &line, ServerConfig &server);
std::vector<ServerConfig> getConfig(std::string &configFile);
int	countDeuxPoints(std::string &line);