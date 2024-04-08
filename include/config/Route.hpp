#pragma once

#include "../libs.hpp"

//class to store every informations about a route
class Route
{
	public:
		///////////////////////////////
		//Constructors and destructor//
		///////////////////////////////

		Route();
		~Route(){};

		//////////////////////
		//operators overload//
		//////////////////////

		Route						&operator=(Route const &src);
		friend std::ostream		&operator<<(std::ostream &out, Route const &src);

		///////////
		//setters//
		///////////

		void						setId(std::string id);
		void						setPath(std::string path);
		void						setRoot(std::string root);

		void						setMethods(std::string methods);

		void						setIsRedir(std::string isRedir);
		void						setRedirDir(std::string redirDir);

		void						setListing(std::string listing);

		void						setDefaultPage(std::string defaultPage);

		void						setIsDownload(std::string isDownload);
		void						setDownloadDir(std::string downloadDir);

		void						setIsUpload(std::string isUpload);
		void						setForceUpload(std::string forceUpload);

		void						setCgiExt(std::string cgiExt);

		///////////
		//getters//
		///////////

		std::string					getId();
		std::string					getPath();
		std::string					getRoot();

		std::vector<std::string>	getMethods();

		bool						isRedir();
		std::string					getRedirDir();

		bool						isListing();
		std::string					list();

		std::string					getDefaultPage();

		bool						isDownload();
		std::string					getDownloadDir();

		bool						isUpload();

		bool						isCgi(std::string path);

		////////////////////
		//member functions//
		////////////////////

		void						verif() const;


	private:
		std::string					_id;
		std::string					_path;
		std::string					_root;

		std::vector<std::string>	_methods;

		bool						_isRedir;
		std::string					_redirDir;

		bool						_listing;

		std::string					_defaultPage;

		bool						_isDownload;
		std::string					_downloadDir;

		bool						_isUpload;
		bool						_forceUpload;

		std::vector<std::string>	_cgiExt;
};
