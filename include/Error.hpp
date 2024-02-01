#pragma once

#include "publiclib.hpp"

class Error
{
	public:
		Error();
		Error(const Error &other);
		~Error();

		Error						&operator=(const Error &other);
		friend std::ostream			&operator<<(std::ostream &out, const Error &error);

		void						addPage(int errorCode, const std::string &errorPage);
		const std::string			&getPage(int errorCode) const;


	private:
		std::map<int, std::string>	_errorPages;
};