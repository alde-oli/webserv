#pragma once

#include "../libs.hpp"

#include "Codes.hpp"
#include "Route.hpp"
#include "ServConfig.hpp"
#include "ParsingExec.hpp"
#include "ParsingUtils.hpp"

int							countDeuxPoints(std::string &line);
in_addr_t					setIPv4(const std::string &addr);
std::string					setFileStr(const std::string &file);
long long int				setBodySize(std::string size);
bool						setBool(std::string boolean);
bool						isRouteValid(const std::string& chemin);
std::string					setDirStr(const std::string &dir);
std::vector<std::string>	setMethod(const std::string& method);
std::vector<std::string> 	setCgiExtension(const std::string &cgiExtension);