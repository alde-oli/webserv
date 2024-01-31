#pragma once

#include "publiclib.hpp"

class CGI
{
	public:
		CGI();
		CGI(const CGI &other);
		~CGI();

		CGI								&operator=(const CGI &other);

		void							addExtension(const std::string &extension);
		void							addPath(const std::string &path);

		const std::vector<std::string>	&getExtensions() const;
		const std::vector<std::string>	&getPaths() const;

	private:
		std::vector<std::string>		_extensions;
		std::vector<std::string>		_paths;
};