/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 15:27:02 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/07 23:14:56 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "webserv.hpp"
#include "serverClass.hpp"
#include "ConnectionClass.hpp"

#define PORT "8001"
#define SELECT_TIMEOUT 10

int main(int ac, char** av)
{
	std::map<std::string, serverClass>	server;
	std::map<int, ConnectionClass>		connection_map;
	fd_set								rfds, rfds_copy;

	if (ac == 2)//&& av[1] == *.conf
		server = setup_server(av[1]);
	else
		server = setup_server(DEFAULT_CONF_FILE);

	FD_ZERO(&rfds);//memset fd_set
	for (std::map<std::string, serverClass>::iterator it = server.begin(); it != server.end(); it++)
		FD_SET(it->second._server_socket, &rfds);//add server socket to fd_set

	while (true)
	{
		rfds_copy = rfds;
		if (select(FD_SETSIZE, &rfds_copy, NULL, NULL, NULL) < 0)
		{
			perror("select eror");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (!FD_ISSET(i, &rfds_copy))
			{
				for (std::map<std::string, serverClass>::iterator it = server.begin(); it != server.end(); it++)
				{
					if (i == it->second._server_socket)//new connection on server n°j
					{
						int client_socket = accept(i, it->second._addr->ai_addr, &(it->second._addr->ai_addrlen));
						if (client_socket < 0)
							perror("accpet");
						else
							connection_map[client_socket] = ConnectionClass(client_socket, it->second);
						FD_SET(client_socket, &rfds);
					}
				}
			}
			else
			{
				std::cout << "handling connection on fd " << i << std::endl;
				handle_connection(connection_map[i]);
				connection_map.erase(i);//maybe not for keep alive
				close(i);//maybe not
				FD_CLR(i, &rfds);
			}
		}
	}
}
