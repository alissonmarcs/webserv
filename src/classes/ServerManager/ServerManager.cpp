#include "ServerManager.hpp"

ServerManager::ServerManager ()
{

}

ServerManager::ServerManager (const ServerManager & src)
{
    (void) src;
}

ServerManager::~ServerManager ()
{

}

void ServerManager::initServers ()
{
    struct epoll_event event;    

    epoll_fd = epoll_create(1);
    if (epoll_fd == -1)
        fatalError("Error creating epoll file descriptor");
    for (size_t i = 0; i < servers.size(); i++)
    {
        servers[i].init();
        bzero(&event, sizeof(event));
        event.events = EPOLLIN;
        event.data.fd = servers[i].server_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, servers[i].server_fd, &event) == -1)
            fatalError("Error adding server's file descriptor to epoll");
    }
}

Server * ServerManager::isServer (int fd)
{
    for (size_t serverIndex = 0; serverIndex < servers.size(); serverIndex++)
    {
        if (servers[serverIndex].server_fd == fd)
            return (&servers[serverIndex]);
    }
    return (NULL);
}

void ServerManager::acceptClient(Server * owner)
{
    struct epoll_event ev;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd, server_fd;

    bzero (&client_addr, sizeof(client_addr));
    bzero (&ev, sizeof(ev));
    server_fd = owner->server_fd;
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1)
        fatalError("Error accepting client");
    clients[client_fd] = Client(client_fd, client_addr, owner);
    ev.events = EPOLLRDHUP;
    ev.data.fd = client_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) 
        fatalError("Error adding client's file descriptor to epoll");
    cout << "New connection from " << getClientIp(&client_addr) << endl;
}

void ServerManager::mainLoop ()
{
    struct epoll_event events[MAX_EPOLL_EVENTS];
    int ready_fds;
    Server * server;

    while (1)
    {
        bzero (&events, sizeof(events));
        ready_fds = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, 300);
        if (ready_fds == -1)
            fatalError("Error waiting for events");
        for (int i = 0; i < ready_fds; i++)
        {
            if ((server = isServer(events[i].data.fd)))
                acceptClient(server);
            else
            {
                if (events[i].events & EPOLLRDHUP)
                {
                    cout << "Client closed connection " << endl;
                    if (close (events[i].data.fd) == -1)
                        fatalError("close");
                    clients.erase(events[i].data.fd);
                }
            } 
        }
    }
}