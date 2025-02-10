#include "Client.hpp"

bool
Client::isCGI()
{
    if (this->route->getCgiExt().empty())
        return (false);
  return (true);
}

void
Client::handleCGI()
{

}