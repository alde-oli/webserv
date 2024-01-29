#pragma once

#include "publiclib.hpp"

class Error
{
	public:
		Error();
		Error(const Error &other);
		~Error();

		Error						&operator=(const Error &other);

		void						addPage(int errorCode, const std::string &errorPage);
		const std::string			&getPage(int errorCode) const;

		class						NoErrorPageException : public std::exception
		{ virtual const char		*what() const throw(); };
		class						pageAlreadyExists : public std::exception
		{ virtual const char		*what() const throw(); };

	private:
		std::map<int, std::string>	_errorPages;
};