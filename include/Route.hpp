#pragma once

#include "publiclib.hpp"

class Route
{
	public:
		Route(std::string id);
		~Route();

		Route						&operator=(const Route &other);
		friend std::ostream			&operator<<(std::ostream &out, const Route &route);

		void						setRoute(const std::string &route);
		void						setRoot(const std::string &root);
		void						setPage(const std::string &page);
		void						setMethods(const std::string &methods);
		void						setListing(const std::string listing);
		void						setDownload(const std::string download);
		void						setDownloadDir(const std::string downloadDir);
		void						setRedir(const std::string redir);
		void						setRedirDir(const std::string redirDir);

		const std::string			&getRoute() const;
		const std::string			&getRoot() const;
		const std::string			&getPage() const;
		bool						&isMethodAllowed(std::string requestMethod) const;
		bool						&isListing() const;
		std::string					&listRoute() const;
		bool						&isDownload() const;
		const std::string			&getDownloadDir() const;
		bool						&isRedir() const;
		const std::string			&getRedirDir() const;


	private:
		Route();
		Route(const Route &other);

		const std::string			_id;
		std::string					_route;
		std::string					_root;
		std::string					_page;
		std::vector<std::string>	_methods;
		bool						_listing;
		bool						_isDownload;
		std::string					_downloadDir;
		bool						_isRedir;
		std::string					_redirDir;
};