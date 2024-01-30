#pragma once

#include "publiclib.hpp"

class Route
{
	public:
		Route();
		Route(const Route &other);
		~Route();

		Route							&operator=(const Route &other);

		void							setRoute(const std::string &route);
		void							setRoot(const std::string &root);
		void							setPage(const std::string &page);
		void							setMethods(const std::vector<std::string> &methods);

		const std::string				&getRoute() const;
		const std::string				&getRoot() const;
		const std::string				&getPage() const;
		const std::vector<std::string>	&getMethods() const;

	private:
		std::string						_route;
		std::string						_root;
		std::string						_page;
		std::vector<std::string>		_methods;
		bool							_listing;
};