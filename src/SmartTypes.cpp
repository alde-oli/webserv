#include "../include/SmartTypes.hpp"
#include <unistd.h>

///////////
//AutoPtr//
///////////

template <typename T>
AutoPtr<T>::AutoPtr(T* p)
	: _ptr(p)
{}

template <typename T>
AutoPtr<T>::~AutoPtr()
	{delete _ptr;}


template <typename T>
T& AutoPtr<T>::operator*() const
	{return *_ptr;}

template <typename T>
T* AutoPtr<T>::operator->() const
	{return _ptr;}

////////////
//AutoAPtr//
////////////

template <typename T>
AutoAPtr<T>::AutoAPtr(T* p)
	: _ptr(p)
{}

template <typename T>
AutoAPtr<T>::~AutoAPtr()
	{delete[] _ptr;}


template <typename T>
T* AutoAPtr<T>::get() const
	{return _ptr;}


//////////
//AutoFD//
//////////

AutoFD::AutoFD()
	: _fd(-1)
{}

AutoFD::AutoFD(int fd)
	: _fd(fd)
{}

AutoFD::~AutoFD()
	{if(_fd > -1) closeFD();}


void AutoFD::set(int fd)
	{if (_fd > -1) close(_fd); _fd = fd;}

int AutoFD::get() const
	{return _fd;}

void AutoFD::closeFD()
	{if(_fd > -1) close(_fd); _fd = -1;}
