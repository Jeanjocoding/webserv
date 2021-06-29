/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClass.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 18:49:16 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/22 16:25:43 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "serverClass.hpp"

serverClass::serverClass()
{
	_default_server = false;
	_port = DEFAULT_PORT;
	_host = DEFAULT_HOST;
	_listen = DEFAULT_HOST;
	_listen += ":";
	_listen += DEFAULT_PORT;
	_server_name = DEFAULT_SERVER_NAME;
	_root = DEFAULT_ROOT;
	_index = DEFAULT_INDEX;
	_error_log = DEFAULT_ERROR_LOG;
	_access_log = DEFAULT_ACCESS_LOG;
	_fastcgi_pass = DEFAULT_FASTCGI_PASS;
	_default_error_pages = baseErrorPages();
	_client_body_size_max = DEFAULT_BODY_MAX;
	_keepalive_timeout = DEFAULT_KEEPALIVE_TIMEOUT;
}

serverClass::serverClass(serverClass const& to_copy)
{
	_default_server = to_copy._default_server;
	_listen = to_copy._listen;
	_port = to_copy._port;
	_host = to_copy._host;
	_server_name = to_copy._server_name;
	_root = to_copy._root;
	_index = to_copy._index;
	_error_log = to_copy._error_log;
	_access_log = to_copy._access_log;
	_default_error_pages = to_copy._default_error_pages;
	_client_body_size_max = to_copy._client_body_size_max;
	_keepalive_timeout = to_copy._keepalive_timeout;

	_server_socket = to_copy._server_socket;
	_addr = to_copy._addr;
}

serverClass& serverClass::operator = (serverClass const& to_copy)
{
	_default_server = false;
	_listen = to_copy._listen;
	_port = to_copy._port;
	_host = to_copy._host;
	_server_name = to_copy._server_name;
	_root = to_copy._root;
	_index = to_copy._index;
	_error_log = to_copy._error_log;
	_access_log = to_copy._access_log;
	_default_error_pages = to_copy._default_error_pages;
	_client_body_size_max = to_copy._client_body_size_max;
	_keepalive_timeout = to_copy._keepalive_timeout;

	_server_socket = to_copy._server_socket;
	_addr = to_copy._addr;
	return (*this);
}

std::string*	serverClass::operator [] (std::string setting_name)
{
	if (setting_name == "listen")
		return &_listen;
	else if (setting_name == "port")
		return &_port;
	else if (setting_name == "host")
		return &_host;
	else if (setting_name == "server_name")
		return &_server_name;
	else if (setting_name == "root")
		return &_root;
	else if (setting_name == "index")
		return &_index;
	else if (setting_name == "error_log")
		return &_error_log;
	else if (setting_name == "access_log")
		return &_access_log;
	else if (setting_name == "fastcgi_pass")
		return &_fastcgi_pass;
	else if (setting_name == "404")
		return &_default_error_pages[404];
	else if (setting_name == "400")
		return &_default_error_pages[400];
	else if (setting_name == "client_body_size_max")
		return &_client_body_size_max;
	else if (setting_name == "keepalive_timeout")
		return &_keepalive_timeout;
	else
		return NULL;
}

void serverClass::startServer()
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
	if (!_listen.empty())
	{
		_port = _host = _listen;
		_port.erase(0, _port.find(":") + 1);
		_host.erase(_host.find(_port) - 1, _port.size() + 1);
	}
	retval = getaddrinfo(_host.c_str(), _port.c_str(), &hint, &_addr);
	if (retval)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(retval) << std::endl;
		std::cout << "port : " << _port << std::endl;
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

std::map<unsigned short, std::string>	serverClass::baseErrorPages(void)
{
	std::map<unsigned short, std::string>	res;

	res[400] = ERR_400_PATH;
	res[404] = ERR_404_PATH;
	return res;
}

serverClass::~serverClass()
{
	close(_server_socket);
}
