/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 15:27:02 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/22 21:38:12 by asablayr         ###   ########.fr       */
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

int main(int ac, char** av)
{
	std::vector<serverClass*>			server_map;
	std::map<int, ConnectionClass>		connection_map;
	fd_set								rfds, rfds_copy;
	fd_set								wfds, wfds_copy;

	if (ac == 2)//&& av[1] == *.conf
		server_map = setup_server(av[1]);
	else
		server_map = setup_server(DEFAULT_CONF_FILE);

	FD_ZERO(&rfds);//memset fd_set
	FD_ZERO(&wfds);//memset fd_set
	for (std::vector<serverClass*>::iterator it = server_map.begin(); it != server_map.end(); it++)
	{
		try 
		{
			(*it)->startServer();
			std::cout << "server started on : " << (*it)->_listen << std::endl;
			FD_SET((*it)->_server_socket, &rfds);//add server socket to fd_set
		}
		catch (char const*)
		{
			for (std::vector<serverClass*>::iterator i = server_map.begin(); i != server_map.end(); i++)
				delete *i;
			exit(EXIT_FAILURE);
		}
	}
	while (true)
	{
		rfds_copy = rfds;
		wfds_copy = wfds;
		if (select(FD_SETSIZE, &rfds_copy, &wfds_copy, NULL, NULL) < 0)
		{
			std::perror("select eror");
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
							FD_SET(client_socket, &rfds);
							check = true;
						}
						break;
					}
				}
				if (check)
					continue;
//				handle_connection(connection_map[i]);
				if (connection_map[i].receiveRequest() <= 0) // close connection if error while receiving paquets
					connection_map[i].closeConnection();
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
				answer_connection(connection_map[i]);
				if (connection_map[i].getStatus() == CO_ISCLOSED) // erase if connection is not persistent or respond has encountered an error
				{
					connection_map[i].closeConnection();
					FD_CLR(i, &wfds);
					connection_map.erase(i);
				}
				else if (connection_map[i].getStatus() == CO_ISDONE) // all requests have been answered
				{
					FD_CLR(i, &wfds);
					FD_SET(i, &rfds);
				}
			}
		}
	}
}
