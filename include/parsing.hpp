#pragma once

#include "publiclib.hpp"

//parsing utils
in_addr_t					setIPv4(const std::string &addr);
in_port_t					setPort(const std::string &port);
std::string					setFileStr(const std::string &file);
std::string					setDirStr(const std::string &dir);
std::vector<std::string>	setCgiExtension(const std::string &cgiExtension);
std::vector<std::string>	setMethod(const std::string& method);
bool						setBool(std::string boolean);
long long int				setBodySize(std::string size);
bool                        isRouteValid(const std::string& chemin)