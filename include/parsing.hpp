#pragma once

#include "publiclib.hpp"

//parsing utils
in_addr_t					setIPv4(const std::string &addr);
in_port_t					setPort(const std::string &port);
std::string					setFileStr(const std::string &file);
std::string					setDirStr(const std::string &dir);
std::vector<std::string>	setCgiExtension(const std::string &cgiExtension);
std::vector<std::string>	set_method(const std::string& method);
bool						set_bool(std::string boolean);
long long int				set_body_size(std::string size);