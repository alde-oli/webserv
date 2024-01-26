
#include <iostream>
#include <cstdlib>
#include <stdexcept>
//#include <typeinfo>
#include <ctime>
#include "A.hpp"
#include "B.hpp"
#include "C.hpp"

Base	*generate(void)
{
	int	i = rand() % 3;
	if (i == 0)
		return new A();
	else if (i == 1)
		return new B();
	else
		return new C();
}

void	identify(Base *p)
{
	if (dynamic_cast<A *>(p))
		std::cout << "A" << std::endl;
	else if (dynamic_cast<B *>(p))
		std::cout << "B" << std::endl;
	else if (dynamic_cast<C *>(p))
		std::cout << "C" << std::endl;
}

void	identify(Base &p)
{
	A	*a = NULL;
	B	*b = NULL;
	C	*c = NULL;

	if (a && b && c)
		return ;
	try
	{
		a = &dynamic_cast<A &>(p);
		std::cout << "A" << std::endl;
	}
	catch (std::exception &e)
	{
		try
		{
			b = &dynamic_cast<B &>(p);
			std::cout << "B" << std::endl;
		}
		catch (std::exception &e)
		{
			try
			{
				c = &dynamic_cast<C &>(p);
				std::cout << "C" << std::endl;
			}
			catch (std::exception &e)
			{
				std::cout << "Unknown" << std::endl;
			}
		}
	}
}

int	main(void)
{
	srand(time(NULL));
	for (int i = 0; i < 10; i++)
	{
		std::cout << std::endl << "Test " << i << std::endl;
		Base	*base = generate();
		std::cout << "  Pointer: ";
		identify(base);
		std::cout << "Reference: ";
		identify(*base);
		//std::cout << "real type is : " << typeid(*base).name() << std::endl;
		delete base;
	}
	return 0;
}

