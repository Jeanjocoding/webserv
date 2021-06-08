/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClass.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 18:49:16 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/08 17:02:13 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "serverClass.hpp"

serverClass::serverClass()
{
}

serverClass::serverClass(std::string port, std::string host, std::string server_name, std::map<unsigned short, std::string> error_pages, unsigned int client_body_max) : _port(port), _host(host), _server_name(server_name), _default_error_pages(error_pages), _client_body_size_max(client_body_max)//might add setup_routes
{

	int				retval;
	int				optval;
	struct addrinfo	hint;

	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;
	hint.ai_protocol = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;

	_addr = 0;

	retval = getaddrinfo(NULL, _port.c_str(), &hint, &_addr);
	if (retval)
	{
		std::cerr << "getaddrinfo: " << retval << std::endl;
		exit(EXIT_FAILURE);
	}
	_server_socket = socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);
	if (_server_socket == -1)
	{
		std::cerr << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	optval = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		perror("setsockopt");
	if (bind(_server_socket, _addr->ai_addr, _addr->ai_addrlen) < 0)
	{
		std::cerr << "Failed to bind to port " << _port << ". errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(_server_socket, 10) < 0)
	{
		std::cerr << "Failed to grab connection. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

serverClass::serverClass(serverClass const& to_copy) : _port(to_copy._port), _host(to_copy._host), _server_name(to_copy._server_name), _default_error_pages(to_copy._default_error_pages), _client_body_size_max(to_copy._client_body_size_max), _server_socket(to_copy._server_socket), _addr(to_copy._addr)
{
}

serverClass& serverClass::operator = (serverClass const& to_copy)
{
	_port = to_copy._port;
	_host = to_copy._host;
	_server_name = to_copy._server_name;
	_default_error_pages = to_copy._default_error_pages;
	_client_body_size_max = to_copy._client_body_size_max;
	_server_socket = to_copy._server_socket;
	return (*this);
}

serverClass::~serverClass()
{
}
