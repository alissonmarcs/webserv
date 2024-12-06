#include "Utils.hpp"

int main(int argc, char **argv)
{
	if (validateConfigFile(argc, argv))
		return (1);

	std::cout << "Hello, World!" << std::endl;
	return 0;
}
