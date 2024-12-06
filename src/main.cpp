#include "Utils.hpp"


int processInput(std::string path)
{
	struct stat file_info;

	if (stat(path.c_str(), &file_info) == -1)
	{
		if (errno == EACCES)
			return (FileNoPermission);
		else if (errno == ENOENT)
			return (FileNotFound);
	}
	if (S_ISREG(file_info.st_mode))
	{
		if (file_info.st_mode & S_IRUSR)
			return (FileWithPermissions);
		else
			return (FileNoPermission);
	}
	if (S_ISDIR(file_info.st_mode))
	{
		if (file_info.st_mode & S_IRUSR)
			return (DirectoryWithPermissions);
		else
			return (DirectoryNoPermission);
	}
	return (0);
}


int main(int argc, char **argv)
{
	(void)argc;

	std::string path = argv[1];
	int result = processInput(path);

	if (result == FileNoPermission)
		std::cout << RED << "FileNoPermission" << RESET << std::endl;
	else if (result == DirectoryNoPermission)
		std::cout << RED << "DirectoryNoPermission" << RESET << std::endl;
	else if (result == FileNotFound)
		std::cout << RED << "FileNotFound" << RESET << std::endl;
	else if (result == DirectoryNotFound)
		std::cout << RED << "DirectoryNotFound" << RESET << std::endl;
	else if (result == FileWithPermissions)
		std::cout << GREEN << "FileWithPermissions" << RESET << std::endl;
	else if (result == DirectoryWithPermissions)
		std::cout << GREEN << "DirectoryWithPermissions" << RESET << std::endl;


	std::cout << "Hello, World!" << std::endl;
	return 0;
}
