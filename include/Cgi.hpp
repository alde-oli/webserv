#pragma once

#include "publiclib.hpp"

class CGI
{
	public:
		CGI();
		CGI(const CGI &other);
		~CGI();

		CGI								&operator=(const CGI &other);
		friend std::ostream				&operator<<(std::ostream &out, const CGI &cgi);

		void							addExtension(const std::string &extension);
		void							addPath(const std::string &path);

		const std::string				&getPath() const;
		bool							isValidExt(const std::string &extension) const;

	private:
		std::vector<std::string>		_extensions;
		std::string						_path;
};