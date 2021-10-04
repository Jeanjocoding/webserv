/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 15:27:02 by asablayr          #+#    #+#             */
/*   Updated: 2021/10/04 17:48:09 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "webserv.hpp"
#include "serverClass.hpp"
#include "ConnectionClass.hpp"

static void	start_servers(std::vector<serverClass*> server_map, fd_set& rfds)
{
	std::vector<serverClass*> 			serv_up;

	for (std::vector<serverClass*>::iterator it = server_map.begin(); it != server_map.end(); it++)
	{
		try 
		{
			(*it)->startServer();//binds set to listen etc...
			std::cout << "server " << (*it)->_server_name << " started on : " << (*it)->_listen << std::endl;
			serv_up.push_back(*it);
			FD_SET((*it)->_server_socket, &rfds);//add server socket to fd_set
		}
		catch (char const*)
		{
			for (std::vector<serverClass*>::iterator i = serv_up.begin(); i != serv_up.end(); i++)
			{
				if ((*it)->_port == (*i)->_port)
				{
					(*it)->_server_socket = (*i)->_server_socket;
					break ;
				}
			}
		}
	}
	if (serv_up.empty())
	{
		for (std::vector<serverClass*>::iterator i = server_map.begin(); i != server_map.end(); i++)
			delete *i;
		exit(EXIT_FAILURE);
	}
}

static ConnectionClass* find_connection(int fd, std::map<int, ConnectionClass>& map)
{
	for (std::map<int, ConnectionClass>::iterator it = map.begin(); it != map.end(); it++)
		if (fd == it->second._socketNbr || fd == it->second.getOutputFd() || fd == it->second.getInputFd())
			return &(it->second);
	return 0;
}

static void checkConnectionTimeouts(fd_set& rfds, fd_set& wfds, std::map<int, ConnectionClass>& connection_map)
{
	for (std::map<int, ConnectionClass>::iterator i = connection_map.begin(); i != connection_map.end(); i ++)// TODO unit test
	{
		if (time(0) - i->second.getTimer() > i->second._servers[0]->getKeepAliveTimeout())// if connection is timing out
		{
			if (FD_ISSET(i->first, &rfds))
			{
				if (i->second.getStatus() == CO_IS_CLOSING && ((time(0) - i->second.getTimer()) > (i->second._servers[0]->getKeepAliveTimeout() * 2))) // si j'essaye de fermer depuis timeout * 2
				{
					std::cout << "clean closing was taking too much time on fd " << i->first <<", forcing closure" << std::endl;
					i->second.simpleCloseConnection(); //je force fermeture
					FD_CLR(i->first, &rfds);
					connection_map.erase(i->first);
					break;
				}
				else if (i->second.getStatus() != CO_IS_CLOSING)
				{
					std::cout << "close cuz timeout in rfds" << std::endl;
					if (connection_map[i->first].closeWriteConnection() == -1)
					{
						FD_CLR(i->first, &rfds);
						connection_map.erase(i->first);
						continue;
					}
				}
			}
			else if (FD_ISSET(i->first, &wfds))
			{
				std::cout << "close cuz timeout in wfds" << std::endl;
				if (connection_map[i->first].closeWriteConnection() == -1)
				{
					FD_CLR(i->first, &wfds);
					connection_map.erase(i->first);
					continue;
				}
				else
				{
					FD_CLR(i->first, &wfds);
					FD_SET(i->first, &rfds);
				}
			}
		}
	}
}

int main(int ac, char** av)
{
	std::vector<serverClass*>			server_map;
	std::map<int, ConnectionClass>		connection_map;
	fd_set								rfds, rfds_copy;
	fd_set								wfds, wfds_copy;
	struct timeval						st_timeout;

	if (ac == 2)//&& av[1] != *.conf
		server_map = setup_server(av[1]);
	else
		server_map = setup_server(DEFAULT_CONF_FILE);

	FD_ZERO(&rfds);//memset fd_set
	FD_ZERO(&wfds);//memset fd_set
	start_servers(server_map, rfds);
	st_timeout.tv_sec = std::atoi(server_map[0]->_keepalive_timeout.c_str());//set keepalive_timeout
	std::cout << "timeout: " << st_timeout.tv_sec;
	st_timeout.tv_usec = 0;
	while (true)
	{
		rfds_copy = rfds;
		wfds_copy = wfds;
		if (select(FD_SETSIZE, &rfds_copy, &wfds_copy, NULL, &st_timeout) < 0)
		{
			std::perror("select error");
			for (std::vector<serverClass*>::iterator i = server_map.begin(); i != server_map.end(); i++)
				delete *i;
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &rfds_copy))
			{
				bool check = false;
				for (std::vector<serverClass*>::iterator it = server_map.begin(); it != server_map.end(); it++)
				{
					if (i == (*it)->_server_socket)//new connection on server n°j
					{
						int client_socket = accept(i, (*it)->_addr->ai_addr, &((*it)->_addr->ai_addrlen));
						if (client_socket < 0)
							std::perror("accept");
						else
						{
							connection_map[client_socket] = ConnectionClass(client_socket, *it);
							connection_map[client_socket].setServers(server_map, i);
							FD_SET(client_socket, &rfds);
							check = true;
						}
						break;
					}
				}
				if (check)
					continue;
				ConnectionClass* connection = find_connection(i, connection_map);
				if (!connection)
					continue;
				if (connection->getStatus() == CO_HAS_TO_READ_CGI)// if connection is awaiting cgi
				{
					cgiReadOnPipe(*connection);
					if (connection->getStatus() == CO_HAS_TO_ANSWER || connection->getStatus() == CO_HAS_TO_SEND) // if read cgi is done or has failed
					{
						FD_CLR(i, &rfds);
						FD_SET(connection->_socketNbr, &wfds);
					}
					continue;
				}
				else if (connection->receiveRequest() <= 0) // close connection if error while receiving paquets
				{
					std::cout << "close cuz recv request" << std::endl;
					connection->simpleCloseConnection();
				}
				if (connection->getStatus() == CO_IS_CLOSING) // if connection is trying to close
					connection->closeReadConnection(); // try to close the read part of the connection
				if (connection->getStatus() == CO_ISCLOSED) // erase if connection has been closed and clear fd_set
				{
					FD_CLR(i, &rfds);
					connection_map.erase(i);
				}
				else if (connection->getStatus() == CO_ISREADY) // at least one request ready to be answered
				{
					FD_CLR(i, &rfds);
					if (connection->_request_pipeline[0].isCGI())// if cgi is requested 
					{
						FD_CLR(i, &rfds);
						connection->setStatus(CO_HAS_TO_SETUP_CGI);
						if (setup_CGI(*connection))
						{
							std::cout << "cgi has been setup\n";
							FD_SET(connection->getInputFd(), &wfds);
						}
						else
							FD_SET(connection->_socketNbr, &wfds);
					}
					else // if cgi is not needed
						FD_SET(i, &wfds);
				}
			}
			else if (FD_ISSET(i, &wfds_copy)) // fd is ready for write
			{
				ConnectionClass* connection = find_connection(i, connection_map);
				if (!connection)
					continue;
				switch (connection->getStatus())
				{
					case CO_ISREADY :  // if request doesn't need cgi
						std::cout << "case co is ready\n";
						answer_connection(*connection); // set and send response for one request
						break;
					case CO_HAS_TO_WRITE_CGI : //if cgi is requested
						std::cout << "case co has to write CGI\n";
						if (cgiWriteOnPipe(*connection) == -1) // if write on cgi fd fails
						{
							FD_CLR(i, &wfds);
							FD_SET(connection->_socketNbr, &wfds);
							connection->setStatus(CO_HAS_TO_ANSWER);
						}
						else // if write cgi is complete
						{
							FD_CLR(i, &wfds);
							FD_SET(connection->getOutputFd(), &rfds);
							connection->setStatus(CO_HAS_TO_READ_CGI);
						}
						break;
					case CO_HAS_TO_ANSWER : // connection is ready to answer cgi request
						std::cout << "case co has to answer\n";
						answer_CGI(*connection); // put cgi buffer in response and send response
						break;
					case CO_HAS_TO_SEND : // response ready and has to be sent
						std::cout << "case co has to send\n";
						connection->sendResponse(); // send _currentResponse
						break;
				}
				if (connection->getStatus() == CO_ISDONE && !connection->isPersistent()) // all request answered but connection not persistent
				{
					if (connection->closeWriteConnection() == -1) // try to close connection
					{
						FD_CLR(i, &wfds);
						connection_map.erase(i);
						continue;
					}
					else
					{
						FD_CLR(i, &wfds);
						FD_SET(i, &rfds);
					}
				}
				else if (connection->getStatus() == CO_ISDONE) // all request have been answered
				{
					FD_CLR(i, &wfds);
					FD_SET(i, &rfds);
				}
				else if (connection->getStatus() == CO_ISREADY || connection->getStatus() == CO_HAS_TO_ANSWER || connection->getStatus() == CO_HAS_TO_SEND)
				{
					if (connection->_request_pipeline[0].isCGI())// if cgi is requested 
					{
						if (setup_CGI(*connection))
						{
							std::cout << "cgi has been setup\n";
							FD_SET(connection->getInputFd(), &wfds);
						}
						else
							FD_SET(connection->_socketNbr, &rfds);
					}
				}
			}
		}
		checkConnectionTimeouts(rfds, wfds, connection_map); // check all connections timeout and remove or set connection accordingly
	}
}
