#include "Utils.hpp"




int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << GREEN << "Usage: " << BLUE << argv[0] << CYAN << " config file" << RESET << std::endl;
		return 1;
	}

	std::cout << "Hello, World!" << std::endl;
	return 0;
}
