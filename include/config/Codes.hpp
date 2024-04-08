#pragma once

#include "../libs.hpp"

#define DEFAULT_ERRORS "error_pages/"

// class to store custom error pages
// is used to get custom and default error pages from a given code
// is also used to get the right message for codes from 1XX to 5XX
class Codes
{
	public:
		/////////////////////////////////
		// Constructors and destructor //
		/////////////////////////////////

		Codes() {};
		~Codes() {};

		////////////////////////
		// operators overload //
		////////////////////////

		Codes						&operator=(Codes const &src);
		friend std::ostream			&operator<<(std::ostream &out, Codes const &src);

		/////////////
		// setters //
		/////////////

		void						addErrPage(int code, std::string page);

		/////////////
		// getters //
		/////////////

		const std::string			getErrPage(int code) const;

		//////////////////////
		// member functions //
		//////////////////////

		bool						checkValidity(std::string dir);

		std::string					getMsgCode(int code);
		std::string					Msg500(int code_erreur);
		std::string					Msg400(int code_erreur);
		std::string					Msg300(int code_erreur);
		std::string					Msg200(int code_erreur);

	private:
		std::map<int, std::string>	_errPages;	
};
