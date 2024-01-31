#include "Cgi.hpp"

//////////////////////////////
//contructors and destructor//
//////////////////////////////

CGI::CGI() : _extensions(), _paths()
{
}

CGI::CGI(const CGI &other)
{
    *this = other;
}

CGI::~CGI()
{
}
