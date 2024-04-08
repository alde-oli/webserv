#pragma once

#include "libs.hpp"

template <typename T>
//class to autodelete ptr when out of scope
class AutoPtr
{
	public:
		AutoPtr() {};
		explicit AutoPtr(T* p = NULL);
		~AutoPtr(); 

		T&			operator*() const;
		T*			operator->() const;

	private:
		T*			_ptr;
};

template <typename T>
//class to autodelete ptr[] when out of scope
class AutoAPtr
{
	public:
		AutoAPtr() {};
		explicit AutoAPtr(T* p = NULL);
		//AutoAPtr(const AutoAPtr& a) {};
		~AutoAPtr();
		
		T*			get() const; 

	private:
		T*			_ptr;
};

//class to auto close file descriptor when out of scope
class AutoFD
{
	public:
		AutoFD();
		explicit AutoFD(int fd);
		~AutoFD();

		void	set(int fd);
		
		int				get() const;
		void			closeFD();

	private:
		int				_fd;
};
