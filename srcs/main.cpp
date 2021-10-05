/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 15:27:02 by asablayr          #+#    #+#             */
/*   Updated: 2021/09/22 15:40:41 by asablayr         ###   ########.fr       */
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

int main(int ac, char** av)
{
	std::vector<serverClass*>			server_map;
	std::map<int, ConnectionClass>		connection_map;
	std::map<int, ConnectionClass&>		input_pipe_map;
	std::map<int, ConnectionClass&>		output_pipe_map;
	fd_set								rfds, rfds_copy;
	fd_set								wfds, wfds_copy;
	struct timeval				st_timeout;
	int					close_return_value;
	int					receive_return_value;
//	int								receive_return;

	if (ac == 2)//&& av[1] == *.conf
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
				if (output_pipe_map.count(i))
				{
					// GERER ERREUR ICI, si cgiReadOnPipe retourne -1, il faut renvoyer une erreur type 500
					std::cout << "in input pipe count" << std::endl;
					if (cgiReadOnPipe((*(output_pipe_map.find(i))).second) == -1)
					{
						FD_CLR((*(output_pipe_map.find(i))).second.getOutputFd(), &rfds);
						FD_SET((*(output_pipe_map.find(i))).second._socketNbr, &wfds);
						output_pipe_map.erase(i);

					}
					else if (!((*(output_pipe_map.find(i))).second.HasToReadOnPipe())) //si j'ai fini de lire sur le pipe
					{
						FD_CLR((*(output_pipe_map.find(i))).second.getOutputFd(), &rfds);
						(*(output_pipe_map.find(i))).second.setHasDoneCgi(1);
						FD_SET((*(output_pipe_map.find(i))).second._socketNbr, &wfds);
						output_pipe_map.erase(i);
					}
					continue;
				}
				if (connection_map[i].isClosing())
				{
					close_return_value = connection_map[i].closeReadConnection();
					if (close_return_value == 1)
					{
						FD_CLR(i, &rfds);
						connection_map.erase(i);
					}
					continue;

				}
				if ((receive_return_value = connection_map[i].receiveRequest()) <= 0) // close connection if error while receiving paquets
				{
					std::cout << "close cuz recv request" << std::endl;
					if (receive_return_value == 0 || receive_return_value == TCP_ERROR)
						connection_map[i].simpleCloseConnection();
				}
				if (connection_map[i].getStatus() == CO_ISCLOSED) // erases if connection has encoutered an error
				{
					FD_CLR(i, &rfds);
					connection_map.erase(i);
				}
				else if (connection_map[i].getStatus() == CO_ISREADY) // at least one request ready to be answered
				{
					FD_CLR(i, &rfds);
					FD_SET(i, &wfds);
				}
			}
			else if (FD_ISSET(i, &wfds_copy))
			{
				if (input_pipe_map.count(i))
				{
					// GERER ERREUR ICI, si cgiWriteOnPipe retourne -1, il faut renvoyer une erreur type 500
					if (cgiWriteOnPipe((*(input_pipe_map.find(i))).second) == -1)
					{
						FD_SET((*(input_pipe_map.find(i))).second._socketNbr, &wfds);
					}
					else
					{
						FD_SET((*(input_pipe_map.find(i))).second.getOutputFd(), &rfds);
						output_pipe_map.insert(std::pair<int, ConnectionClass&>((*(input_pipe_map.find(i))).second.getOutputFd(), (*(input_pipe_map.find(i))).second));
					}
					FD_CLR((*(input_pipe_map.find(i))).second.getInputFd(), &wfds);
					input_pipe_map.erase(i);
					continue;
				}
				answer_connection(connection_map[i]);
				if (connection_map[i].HasToWriteOnPipe() && !input_pipe_map.count(i))
				{
					input_pipe_map.insert(std::pair<int, ConnectionClass&>(connection_map[i].getInputFd(), connection_map[i]));
					FD_SET(connection_map[i].getInputFd(), &wfds);
					FD_CLR(i, &wfds);  //A TESTER AVEC ATTENTION
					continue;
				}
				connection_map[i].setStatus(CO_ISDONE);
				if (connection_map[i].getStatus() != CO_ISCLOSED && !connection_map[i].isPersistent())
				{
					close_return_value = connection_map[i].closeWriteConnection();
					if (close_return_value == -1)
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
				else if (connection_map[i].getStatus() == CO_ISDONE) // all requests have been answered
				{
					FD_CLR(i, &wfds);
					FD_SET(i, &rfds);
				}
			}
		}
		for (std::map<int, ConnectionClass>::iterator i = connection_map.begin(); i != connection_map.end(); i ++)// TODO unit test
		{
//			std::cout << "i in timeout check: " << i->first << ", map size: " << connection_map.size() << ", iterations: " << iterations << std::endl;
			if (!i->second.isPersistent() && !i->second.isClosing())
				continue;
			if (time(0) - i->second.getTimer() > i->second._servers[0]->getKeepAliveTimeout())// TODO switch server selection and unit from sec to ms
			{
				if (FD_ISSET(i->first, &rfds))
				{
					if (i->second.isClosing() && ((time(0) - i->second.getTimer()) > (i->second._servers[0]->getKeepAliveTimeout() * 2))) // si j'essaye de fermer depuis timeout * 2
					{
						std::cout << "clean closing was taking too much time on fd " << i->first <<", forcing closure" << std::endl;
						i->second.simpleCloseConnection(); //je force fermeture
						FD_CLR(i->first, &rfds);
						connection_map.erase(i->first);
						break;
					}
					else if (!i->second.isClosing())
					{
						std::cout << "close cuz timeout in rfds" << std::endl;
						close_return_value = connection_map[i->first].closeWriteConnection();
						if (close_return_value == -1)
						{
							FD_CLR(i->first, &rfds);
							connection_map.erase(i->first);
							continue;
						}
					}
//					FD_CLR(i->first, &rfds);
				}
				else if (FD_ISSET(i->first, &wfds))
				{
					std::cout << "close cuz timeout in wfds" << std::endl;
					close_return_value = connection_map[i->first].closeWriteConnection();
					if (close_return_value == -1)
					{
						FD_CLR(i->first, &wfds);
//						FD_CLR(i->first, &rfds);
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
}
