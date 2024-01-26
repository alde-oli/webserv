#include "MultipartFormData.hpp"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

// data example:
//
// ------WebKitFormBoundary3dmDfetTEeYcc8m1
// Content-Disposition: form-data; name="fileUpload1"; filename="test.txt"
// Content-Type: text/plain
//
// test
//
// ------WebKitFormBoundary3dmDfetTEeYcc8m1
// Content-Disposition: form-data; name="fileUpload2"; filename="test2.txt"
// Content-Type: text/plain
//
// test2
//
// ------WebKitFormBoundary3dmDfetTEeYcc8m1
// Content-Disposition: form-data; name="fileUpload3"; filename="test3.txt"
// Content-Type: text/plain
//
// test3
//
// ------WebKitFormBoundary3dmDfetTEeYcc8m1--


MultipartFormData::MultipartFormData(std::string contentType, std::string rawContent)
{
	boundary = contentType.substr(contentType.find("boundary=") + 9);
	std::string delimiter = "--" + boundary;
	std::string delimiterEnd = "--" + boundary + "--";

	std::istringstream	rawStream(rawContent);
	std::string			line;
	contentData 		data;
	while(getline(rawStream, line, '\n'))
	{
		if (line == delimiter)
		{
			if (data.name != "")
				content.push_back(data);
			continue ;
		}
		else if (line == delimiterEnd)
		{
			if (data.name != "")
				content.push_back(data);
			break;
		}
		else if (line.find("Content-Disposition: form-data; name=") != std::string::npos)
		{
			data.name = line.substr(line.find("name=") + 6);
			data.name = data.name.substr(0, data.name.find("\""));
			data.filename = line.substr(line.find("filename=") + 10);
			data.filename = data.filename.substr(0, data.filename.find("\""));
			getline(rawStream, line, '\n');
		}
		else
		{
			data.content += line + "\n";
		}

	}
}