#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

struct contentData
{
	std::string	name;
	std::string	filename;
	std::string	content;
};

class MultipartFormData
{
	public:
		MultipartFormData(){};
		MultipartFormData(std::string contentType, std::string rawContent);
		~MultipartFormData(){};

		std::string getBoundary();
		std::string getContent();

		void setBoundary(std::string boundary);
		void setContent(std::vector<contentData> content);

	private:
		std::string					boundary;
		std::vector<contentData>	content;
};
