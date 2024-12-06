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

bool validateConfigFile(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << RED << "Error: " << RESET << "Invalid number of arguments." << std::endl;
		return (true);
	}
	std::string file = argv[1];
	if (file.size() < 5 || file.substr(file.size() - 5) != ".conf")
	{
    	std::cerr << RED << "Error: " << RESET << "The file must have a .conf extension." << std::endl;
    	return true;
	}
	if (processInput(argv[1]) == FileNotFound)
	{
		std::cerr << RED << "Error: " << RESET << "File not found." << std::endl;
		return (true);
	}
	if (processInput(argv[1]) == FileNoPermission)
	{
		std::cerr << RED << "Error: " << RESET << "No permission to read file." << std::endl;
		return (true);
	}
	if (processInput(argv[1]) != FileWithPermissions)
	{
		std::cerr << RED << "Error: " << RESET << "The file must be a regular file and must have the appropriate permissions." << std::endl;
		return (true);
	}
	return (false);
}