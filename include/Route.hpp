#pragma once

#include "publiclib.hpp"

class Route
{
	public:
		Route(const std::string &id);
		Route();
		Route(const Route &other);
		~Route();

		Route						&operator=(const Route &other);
		friend std::ostream			&operator<<(std::ostream &out, const Route &route);

		void						setId(const std::string &id);
		void						setRoute(const std::string &route);
		void						setRoot(const std::string &root);
		void						setPage(const std::string &page);
		void						setMethods(const std::string &methods);
		void						setListing(const std::string listing);
		void						setDownload(const std::string download);
		void						setDownloadDir(const std::string downloadDir);
		void						setUpload(const std::string upload);
		void						setForceUpload(const std::string forceUpload);
		void						setRedir(const std::string redir);
		void						setRedirDir(const std::string redirDir);
		void						setCgi(const std::string cgi);


		const std::string			&getRoute() const;
		const std::string			&getRoot() const;
		const std::string			&getPage() const;
		bool						isMethodAllowed(std::string requestMethod) const;
		bool						isListing() const;
		std::string					listRoute() const;
		bool						isUpload() const;
		bool						isForceUpload() const;
		bool						isDownload() const;
		const std::string			&getDownloadDir() const;
		bool						isRedir() const;
		const std::string			&getRedirDir() const;
		bool						isCgi(std::string &extension) const;
		void						verif() const;


	private:
		std::string					_id;
		std::string					_route;
		std::string					_root;
		std::string					_page;
		std::vector<std::string>	_methods;
		bool						_listing;
		bool						_upload;
		bool						_forceUpload;
		bool						_isDownload;
		std::string					_downloadDir;
		bool						_isRedir;
		std::string					_redirDir;
		std::vector<std::string>	_cgi;
};