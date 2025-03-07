#ifndef ERROR_HPP_
# define ERROR_HPP_

# include "Utils.hpp"
# include <exception>
// Class declaration
class Error : public exception
{
  public:
	virtual ~Error() throw()
	{
	}
	explicit Error(const string &message) : _message(message)
	{
	}

	const char *what() const throw()
	{
		return (_message.c_str());
	}

  private:
	string _message;
};
;

class ConfigException : public Error
{
  public:
	explicit ConfigException(const string &msg) : Error(msg)
	{
	}
};

class ConfigParserException : public std::exception
{
  public:
	ConfigParserException(const char * msg) : _msg(msg)
	{
	}
	virtual const char *what() const throw()
	{
		return (_msg);
	}
	~ConfigParserException() throw()
	{
	}

  private:
	const char * _msg;
};

#endif
