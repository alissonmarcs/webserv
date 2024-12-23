#include "ServerManager.hpp"

ServerManager::ServerManager () {}

ServerManager::ServerManager (const ServerManager &src) { (void)src; }

ServerManager::~ServerManager () {}

void
ServerManager::initServers ()
{
  struct epoll_event event;

  epoll_fd = epoll_create (1);
  if (epoll_fd == -1)
    FATAL_ERROR ("Error creating epoll file descriptor");
  for (size_t i = 0; i < servers.size (); i++)
    {
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
  struct epoll_event ev;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof (client_addr);
  int client_fd, server_fd;

  memset (&client_addr, 0, sizeof (client_addr));
  memset (&ev, 0, sizeof (ev));
  server_fd = owner->getServerFd ();
  client_fd
      = accept (server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd == -1)
    FATAL_ERROR ("accept");
  clients[client_fd] = Client (client_fd, client_addr, owner);
  ev.events = EPOLLRDHUP | EPOLLIN;
  ev.data.fd = client_fd;
  if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
    FATAL_ERROR ("epoll_ctl");
  LOGGER (getClientIp(&client_addr).c_str(), "connected");
}

void
ServerManager::mainLoop ()
{
  struct epoll_event events[MAX_EPOLL_EVENTS];
  int ready_fds;
  Server *server;

  while (1)
    {
      memset (&events, 0, sizeof (events));
      ready_fds = epoll_wait (epoll_fd, events, MAX_EPOLL_EVENTS, 300);
      if (ready_fds == -1)
        FATAL_ERROR ("epoll_wait");
      for (int i = 0; i < ready_fds; i++)
        {
          if ((server = isServer (events[i].data.fd)))
            acceptClient (server);
          else
            {
              if (events[i].events & EPOLLRDHUP)
                {
                  LOGGER (getClientIp(clients[events[i].data.fd].getAdress()).c_str(), "closed connection");
                  if (close (events[i].data.fd) == -1)
                    FATAL_ERROR ("close()");
                  clients.erase (events[i].data.fd);
                }
            }
        }
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