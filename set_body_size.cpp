#include "include/publiclib.hpp"
#include <iostream>
#include <cctype>
#include <cstdlib>

// M - 3x000
// G - 6x000

static long long int valid_size(std::string size)
{
	int i = 0;
	long long nbr;

	if (size.find('M') != std::string::npos)
	{
		size = size.substr(0, size.find('M'));
		nbr = atol(size.c_str());
		nbr *= 1000;
		return (nbr);
	}
	else if (size.find('G') != std::string::npos)
	{
		size = size.substr(0, size.find('G'));
		nbr = atol(size.c_str());
		nbr *= 1000000;
		return (nbr);
	}
	else
	{
		return (nbr = atol(size.c_str()));
	}
}

long long int set_body_size(std::string size)
{
	int		i = 0;
	if (!size.empty()) 
	{
		int i = 0;
		while (i < size.length() && isdigit(size[i]))
		{
			i++;
		}
		if (i < size.length() && !isdigit(size[i]) && (size[i] == 'M' || size[i] == 'G'))
		{
			if (i + 1 == size.length())
			{
				return (valid_size(size));
			}
			else
			{
				std::cout << "Error: bad body size input: " << size << std::endl;
				exit (1);
			}
		}
		else if (i == size.length())
		{
			return (valid_size(size));
		}
		else
		{
			std::cout << "Error: bad body size input: " << size << std::endl;
			exit (1);
		}
	}
	std::cout << "Error: bad body size input: " << size << std::endl;
	exit (1);
}

// int main()
// {
// 	long long zbeb = 0;
// 	std::string test = "123b";

// 	zbeb = set_body_size(test);
// 	std::cout << "result: " << zbeb << std::endl;
// 	return (0);
// }