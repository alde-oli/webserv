#pragma once

#include "publiclib.hpp"

class Route
{
	public:
		Route(std::string id);
		~Route();

		Route							&operator=(const Route &other);

		void							setRoute(const std::string &route);
		void							setRoot(const std::string &root);
		void							setPage(const std::string &page);
		void							setMethods(const std::string &methods);
		void							setListing(bool listing);

		const std::string				&getRoute() const;
		const std::string				&getRoot() const;
		const std::string				&getPage() const;
		bool							&isMethod(std::string requestMethod) const;

	private:
		Route();
		Route(const Route &other);

		const std::string				_id;
		std::string						_route;
		std::string						_root;
		std::string						_page;
		std::vector<std::string>		_methods;
		bool							_listing;
		bool							_isDownload;
		std::string						_downloadDir;
		bool							_isRedir;
		std::string						_redirDir;
};