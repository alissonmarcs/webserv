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
  cout << getIpString(&client_addr) << " connected\n";
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
              cout << getIpString(client->getAdress()) << " close connection\n";
              if (close (events[i].data.fd) == -1)
                FATAL_ERROR ("close()");
              clients.erase (events[i].data.fd);
            }
          else if (events[i].events & EPOLLIN)
            handleClient(clients[events[i].data.fd]);
        }
    }
}

void
ServerManager::mainLoop ()
{
  struct epoll_event events[MAX_EPOLL_EVENTS];
  int ready_fds;

  while (1)
    {
      memset (&events, 0, sizeof (events));
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
  map<int, Client>::iterator start = clients.begin ();
  map<int, Client>::iterator end = clients.end ();

  for (; start != end; start++)
    {
      client = &start->second;
      current_time = time (NULL);
      client_time = client->getLastReadTime ();
      if (client_time == 0)
        continue ;
      else if (current_time - client_time > 60)
        {
          cout << getIpString(client->getAdress()) << " timeout, closing connection\n";
          if (close (client->getClientFd ()) == -1)
            FATAL_ERROR ("close()");
          clients.erase (start);
          start = clients.begin ();
          end = clients.end ();
        }
    }
}

void
ServerManager::handleClient (Client &client)
{
  client.readRequest();
  if (client.getErrorCode() != 0)
    {
      cout << getIpString(client.getAdress()) << " sends a bad request, closing connection. " << "Error code: " << client.getErrorCode() << "\n";
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