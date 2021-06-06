/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minimal_server.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 15:27:02 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/06 20:01:12 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define PORT "8001"
#define SELECT_TIMEOUT 10

int main(void)//void for now
{
	struct addrinfo	hint;
	struct addrinfo	*result;
	struct timeval	tv, tv_copy;
	int 			server_socket, retval;
	fd_set			rfds, rfds_copy;
//	fd_set			wfds, wfds_copy;

	//set addrinfo hint for getaddrinfo
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;
	hint.ai_protocol = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;

	//set timeval for select
	tv.tv_sec = SELECT_TIMEOUT;
	tv.tv_usec = 0;

	result = 0;

	retval = getaddrinfo(NULL, PORT, &hint, &result);
	if (retval)
	{
		std::cerr << "getaddrinfo: " << retval << std::endl;
		exit(EXIT_FAILURE);
	}
	server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (server_socket == -1)
	{
		std::cerr << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	if (bind(server_socket, result->ai_addr, result->ai_addrlen) < 0)
	{
		std::cerr << "Failed to bind to port " << PORT << ". errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(server_socket, 10) < 0)
	{
		std::cerr << "Failed to grab connection. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	FD_ZERO(&rfds);//memset fd_set
	FD_SET(server_socket, &rfds);//add server socket to fd_set

	while (true)
	{
		rfds_copy = rfds;
		tv_copy = tv;
		if (select(FD_SETSIZE, &rfds_copy, NULL, NULL, &tv_copy) < 0)//timeout should be NULL
		{
			perror("select eror");
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (!FD_ISSET(i, &rfds_copy))
			{
				if (i == server_socket)//new connection
				{
					int client_socket = accept(i, result->ai_addr, &result->ai_addrlen);
					FD_SET(client_socket, &rfds);
				}
			}
			else
			{
				std::cout << "handling connection " << i << std::endl;
//				handle_connection(i);
				FD_CLR(i, &rfds);
			}
		}
	}
}
