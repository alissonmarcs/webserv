#include "ServerManager.hpp"

ServerManager::ServerManager () {}

ServerManager::ServerManager (const ServerManager &src) { (void)src; }

ServerManager::~ServerManager () {}

bool mustBeSkiped (vector<Server> & servers, size_t index)
{
  if (index == 0)
    return (false);

  for (size_t i = 0; i < index; i++)
    {
      if (servers[i].getPort() == servers[index].getPort())
        return (true);
    }
  return (false);
}

void
ServerManager::initServers ()
{
  struct epoll_event event;

  epoll_fd = epoll_create (1);
  if (epoll_fd == -1)
    FATAL_ERROR ("Error creating epoll file descriptor");
  for (size_t i = 0; i < servers.size (); i++)
    {
      if (mustBeSkiped (servers, i))
        continue;
      servers[i].init ();
      std::memset (&event, 0, sizeof (event));
      event.events = EPOLLIN;
      event.data.fd = servers[i].getServerFd ();
      if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, servers[i].getServerFd (),
                     &event)
          == -1)
        FATAL_ERROR ("Error adding server's file descriptor to epoll");
    }
}

Server *
ServerManager::isServer (int fd)
{
  for (size_t serverIndex = 0; serverIndex < servers.size (); serverIndex++)
    {
      if (servers[serverIndex].getServerFd () == fd)
        return (&servers[serverIndex]);
    }
  return (NULL);
}

void
ServerManager::acceptClient (Server *owner)
{
  struct epoll_event ev = {};
  struct sockaddr_in client_addr = {};
  socklen_t client_addr_len = sizeof (client_addr);
  int client_fd, server_fd;

  server_fd = owner->getServerFd ();
  client_fd = accept (server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd == -1)
    FATAL_ERROR ("accept");
  clients[client_fd] = Client (client_fd, client_addr, owner);
  ev.events = EPOLLRDHUP | EPOLLIN | EPOLLOUT;
  ev.data.fd = client_fd;
  if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
    FATAL_ERROR ("epoll_ctl");
  cout << clients[client_fd].getIpString() << " connected\n";
}

void
ServerManager::checkIOEvents (int ready_fds, struct epoll_event *events)
{
  Server * server;
  Client * client;

  for (int i = 0; i < ready_fds; i++)
    {
      if ((server = isServer (events[i].data.fd)))
        acceptClient (server);
      else
        {
          client = &clients[events[i].data.fd];
          if (events[i].events & EPOLLRDHUP)
            {
              cout << client->getIpString() << " close connection\n";
              if (close (events[i].data.fd) == -1)
                cout << "Error when closing client's socket: " << strerror(errno) << '\n';
              clients.erase (events[i].data.fd);
            }
          else if (events[i].events & EPOLLIN && client->isParsingDone() == false)
            client->readRequest();
          else if (events[i].events & EPOLLOUT && client->isParsingDone() == true)
          {
            client->findRoute();
            client->RouteValidation();
            if (client->isCGI())
              client->handleCGI ();
            else
              client->buildResponse();
            if (client->response_is_done)
              sendResponse(client);
          }
        }
    }
}

void
ServerManager::sendResponse (Client *client)
{
  const int client_fd = client->getClientFd();
  const char * response = client->getResponse().c_str();
  const size_t response_size = client->getResponse().size();
  Server * owner = client->getServerOwner();

  if (send (client_fd, response, response_size, 0) < 0)
  {
    cout << "Error when sending response to client's socket: " << strerror(errno) << '\n';
    close (client_fd);
    clients.erase (client_fd);
  }

  if (client->getHeader("keep-alive") == "keep-alive")
    *client = Client (client_fd, client->getAdress(), owner);
  else
  {
    cout << "closing connection of " << client->getIpString() << '\n';
    close (client_fd);
    clients.erase (client_fd);
  }
}

void
ServerManager::mainLoop ()
{
  struct epoll_event events[MAX_EPOLL_EVENTS] = {};
  int ready_fds;

  while (1)
    {
      ready_fds = epoll_wait (epoll_fd, events, MAX_EPOLL_EVENTS, 300);
      if (ready_fds == -1)
        FATAL_ERROR ("epoll_wait");
      checkIOEvents (ready_fds, events);
      checkTimeout ();
      cout.flush();
    }
}

void
ServerManager::checkTimeout ()
{
  time_t current_time, client_time;
  Client * client;
  map<int, Client>::iterator start;

  for (start = clients.begin() ; start != clients.end(); )
    {
      client = &start->second;
      current_time = time (NULL);
      client_time = client->getLastReadTime ();

      if (current_time - client_time > 10)
        {
          cout << client->getIpString() << " timeout, sending response and closing connection\n";
          client->setError(REQUEST_TIMEOUT);
          client->buildResponse();
          start++;
          sendResponse(client);
        }
      else
          start++;
    }
}

void
ServerManager::readFromClient (Client &client)
{
  client.readRequest();
  if (client.getStatusCode() != 0)
    {
      cout << '\n' << client.getIpString() << " sends a bad request, closing connection. " << "Error code: " << client.getStatusCode() << "\n";
      if (close(client.getClientFd()) == -1)
        FATAL_ERROR ("close()");
      clients.erase(client.getClientFd());
    }
}

Server *
ServerManager::getServer (int index)
{
  return (&servers[index]);
}

int
ServerManager::getServersSize ()
{
  return (servers.size ());
}

Server *
ServerManager::getLastServer (void)
{
  return (&servers[servers.size () - 1]);
}

void
ServerManager::addEmptyServer (void)
{
  servers.push_back (Server ());
}

vector<Server> &
ServerManager::getServersRef ()
{
  return (servers);
}
