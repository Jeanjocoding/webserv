/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClass.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 18:49:16 by asablayr          #+#    #+#             */
/*   Updated: 2021/09/08 20:57:01 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
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
	_default_error_pages = baseErrorPages();
	_client_body_size_max = DEFAULT_BODY_MAX;
	_keepalive_timeout = DEFAULT_KEEPALIVE_TIMEOUT;
	_sendfile = DEFAULT_SENDFILE;
	_upload_store = DEFAULT_UPLOAD_STORE;
	_addr = 0;
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
	_sendfile = to_copy._sendfile;
	_upload_store = to_copy._upload_store;

	_server_socket = to_copy._server_socket;
	_addr = 0;
}

serverClass::~serverClass()
{
	close(_server_socket);
	for (std::vector<LocationClass*>::iterator it = _location.begin(); it != _location.end(); it++)
		delete *it;
	if (_addr)
		freeaddrinfo(_addr);
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
	_sendfile = to_copy._sendfile;
	_upload_store = to_copy._upload_store;

	_server_socket = to_copy._server_socket;
	_addr = 0;
	return (*this);
}

std::string*	serverClass::operator [] (std::string setting_name)
{
	if (setting_name == "listen")//change for switch
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
	else if (setting_name == "client_body_size_max")
		return &_client_body_size_max;
	else if (setting_name == "keepalive_timeout")
		return &_keepalive_timeout;
	else if (setting_name == "upload_store")
		return &_upload_store;
	else if (setting_name == "sendfile")
		return &_sendfile;
	else if (atoi(setting_name.c_str()))
		return &_default_error_pages[atoi(setting_name.c_str())];
	else
		return NULL;
}

void			serverClass::setListen(std::string listen)
{
	_listen = listen;
}

void			serverClass::setPort(std::string port)
{
	_port = port;
}

void			serverClass::setHost(std::string host)
{
	_host = host;
}

void			serverClass::setServerName(std::string server_name)
{
	_server_name = server_name;
}

void			serverClass::setRoot(std::string root)
{
	_root = root;
}

void			serverClass::setIndex(std::string index)
{
	_index = index;
}

LocationClass&	serverClass::getLocation(std::string const& uri) const
{
	unsigned long	max_match = 0;
	unsigned long	tmp = 0;
	LocationClass* ret;
	std::cout << "uri : " << uri << std::endl;
	for (std::vector<LocationClass*>::const_iterator it = _location.begin(); it != _location.end(); it++)
	{
		if ((*it)->getParam() == "=")
		{
			if ((*it)->getUri() == uri)
				return **it;
			else
				continue ;
		}
		else if ((*it)->getParam() == "~")
		{
			tmp = caseSensitiveReMatch((*it)->getUri(), uri);
			if (tmp > max_match)
			{
				max_match = tmp;
				ret = *it;
			}
		}
		else if ((*it)->getParam() == "~*")
		{
			if (uri.find((*it)->getUri()) != std::string::npos)
				return **it;
		}
		else if ((*it)->getParam() == "^~" || (*it)->getParam() == "")
		{
			tmp = caseSensitiveMatch((*it)->getUri(), uri);
			if (tmp > max_match)
			{
				max_match = tmp;
				ret = *it;
			}
		}
	}
	std::cout << "location chosen : " << ret->getUri() << "\n"; // testing
	return *ret;
}

long	serverClass::getKeepAliveTimeout(void) const
{
	long			res = 0;
	unsigned int	i = 0;

	res = atoi(_keepalive_timeout.c_str());
	while (_keepalive_timeout[i] >= '0' && _keepalive_timeout[i] <= '9')
		i++;
	if (_keepalive_timeout[i] == 0 || _keepalive_timeout[i] == 's')
		return res;
	else if (_keepalive_timeout[i] == 'm' && _keepalive_timeout[i + 1] == 's')
		res /= 1000;
	else if (_keepalive_timeout[i] == 'm')
		res *= 60;
	else if (_keepalive_timeout[i] == 'h')
		res *= 360;
	else if (_keepalive_timeout[i] == 'd')
		res *= 360 * 24;
	else if (_keepalive_timeout[i] == 'w')
		res *= 360 * 24 * 7;
	else if (_keepalive_timeout[i] == 'M')
		res *= 360 * 24 * 30;
	else if (_keepalive_timeout[i] == 'y')
		res *= 360 * 24 * 365;
	return res;
}

void			serverClass::setLocation(void)
{
	bool	check = false;

	for (std::vector<LocationClass*>::iterator it = _location.begin(); it != _location.end(); it++)
		setLocation(**it);
	for (std::vector<LocationClass*>::iterator it = _location.begin(); it != _location.end(); it++)
		if ((*it)->getUri() == "/" && ((*it)->getParam() == "" || (*it)->getParam() == "^~"))
			check = true;
	if (!check)
	{
		_location.insert(_location.begin(), new LocationClass());
		setLocation(**_location.begin());
	}
}

void			serverClass::setLocation(LocationClass& location) const
{
	location.setServerName(_server_name);
	if (location._directives.find("root") == location._directives.end())
		location.setRoot(_root);
	if (location._directives.find("index") == location._directives.end())
		location.setIndex(_index);
	if (location._directives.find("keepalive_timeout") == location._directives.end())
		location.setKeepaliveTimeout(_keepalive_timeout);
	if (location._directives.find("client_body_size_max") == location._directives.end())
		location.setClientBodySizeMax(_client_body_size_max);
	if (location._directives.find("upload_store") == location._directives.end())
		location.setUploadStore(_upload_store);
	if (location._directives.find("sendfile") == location._directives.end())
		location.setSendfile(_sendfile);
	location.setErrorPages(_default_error_pages);
/*	if (location._directives.find("error_log") == location._directives.end())
		location.setErrorLog(_root);
	if (location._directives.find("access_log") == location._directives.end())
		location.setAccessLog(_root);
*/}

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
		throw;
	}
	_server_socket = socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);
	if (_server_socket == -1)
	{
		std::cerr << "Failed to create socket. errno: " << errno << std::endl;
		throw;
	}
	optval = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		perror("setsockopt");
		throw;
	}
	if (bind(_server_socket, _addr->ai_addr, _addr->ai_addrlen) < 0)
	{
		std::cerr << "Failed to bind to port " << _port << ". errno: " << errno << std::endl;
		throw "bind error";
	}
	if (listen(_server_socket, 10) < 0)
	{
		std::cerr << "Failed to grab connection. errno: " << errno << std::endl;
		throw;
	}
}

std::map<unsigned short, std::string>	serverClass::baseErrorPages(void)
{
	std::map<unsigned short, std::string>	res;

	res[400] = ERR_400_PATH;
	res[404] = ERR_404_PATH;
	res[405] = ERR_405_PATH;
	res[501] = ERR_501_PATH;//TODO complete error map
	return res;
}

unsigned long	serverClass::caseSensitiveReMatch(std::string const& s1, std::string const& s2) const
{//TODO
	std::string s = s1;
	s = s2;
	return 0;
}

unsigned long	serverClass::caseInsensitiveReMatch(std::string const& s1, std::string const& s2) const
{//TODO
	std::string s = s1;
	s = s2;
	return 0;
}

unsigned long	serverClass::caseSensitiveMatch(std::string const& s1, std::string const& s2) const
{
	unsigned long ret = 0;
	for (std::string::const_iterator i = s1.begin(), it = s2.begin(); i != s1.end() && it != s2.end(); i++, it++)
	{
		if (*i != *it)
			break;
		ret++;
	}
	if (ret < s1.length())
		return 0;
	else
		return ret;
}
