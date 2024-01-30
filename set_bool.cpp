#include "include/publiclib.hpp"

#include <cstring>

bool set_bool(std::string boolean)
{
	bool value;

	if (!strcmp(boolean.c_str(), "FALSE") || !strcmp(boolean.c_str(), "false"))
	{
		return (value = false);
	}
	else if (!strcmp(boolean.c_str(), "TRUE") || !strcmp(boolean.c_str(), "true"))
	{
		return (value = true);
	}
	else
	{
		std::cout << "Error: bad boolean type: " << boolean << std::endl;
		exit (1);
	}
}

// int main()
// {
// 	bool zbeb = 0;
// 	std::string test = "TRUE";

// 	zbeb = set_bool(test);
// 	std::cout << "result: " << zbeb << std::endl;
// 	return (0);
// }