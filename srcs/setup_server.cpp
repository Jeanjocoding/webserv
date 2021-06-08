/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 19:42:54 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/08 15:30:43 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <map>
#include <iostream>
#include <fstream>
#include "serverClass.hpp"

static int	get_server_block(std::ifstream* file, std::string* buff)
{
	file = nullptr;
	buff = nullptr;
	return (0);
}

static int	parse_conf(std::string* buff)
{
	buff = nullptr;
	return (0);
}

static std::string set_single_param(std::string name, std::string buff)
{
	std::string cur;

	name = "";
	buff = "";
	return cur;
}

static std::map<unsigned short, std::string> set_error_pages(std::string name, std::string buff)
{
	std::map<unsigned short, std::string> res;

	name = "";
	buff = "";
	return res;
}

static std::map<unsigned short, std::string> base_error_pages(void)
{
	std::map<unsigned short, std::string> res;

	res[400] = ERR_400_PATH;
	res[404] = ERR_404_PATH;
	return res;
}

static int set_body_max(std::string name, std::string buff)
{
	int	size;

	name = "";
	buff = "";
	size = 0;
	return size;
}
/*
static std::string* set_routes(std::string name, std::string buff)
{
	std::string* cur;

	name = "";
	buff = "";
	cur = 0;
	return cur;
}
*/
std::map<std::string, serverClass*>	setup_server(std::string conf_file)
{
	std::map<std::string, serverClass*> server_map;

	std::ifstream 						file(conf_file.c_str());
	std::string*						buff;

	std::string							port;
	std::string							host;
	std::string     					server_name;
	std::map<unsigned short, std::string>    default_error_pages;
	int									client_body_max;
//	std::string*						setup_routes;

	buff = nullptr;

	if (!file.is_open())
	{
		std::cerr << "failed to open " << conf_file << " errno : " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	while (get_server_block(&file, buff))
	{
		if (parse_conf(buff) < 0)
			std::cout << "error in conf file\n";//to change
		else
		{
			port = set_single_param("listen", *buff);
			if (port.empty())
				port = DEFAULT_PORT;
			host = set_single_param("host", *buff);//check param name in nginx
			if (host.empty())
				host = DEFAULT_HOST;
			server_name = set_single_param("server_name", *buff);
			if (server_name.empty())
				server_name = DEFAULT_SERVER_NAME;
			default_error_pages = set_error_pages("error_page", *buff);
			if (default_error_pages.empty())
				default_error_pages = base_error_pages();
			client_body_max = set_body_max("client_body_max", *buff);//check param name
			if (!client_body_max)
				client_body_max = DEFAULT_BODY_MAX;
/*			setup_routes = set_routes("setup_routes", buff);//check param name
			if (!setup_routes)
				setup_routes = DEFAULT_SETUP_ROUTES;
*/			server_map[port] = new serverClass(port, host, server_name, default_error_pages, client_body_max);//might add setup_routes
		}
	}
	if (server_map.empty())
	{
		port = DEFAULT_PORT;
		host = DEFAULT_HOST;
		server_name = DEFAULT_SERVER_NAME;
		default_error_pages = base_error_pages();
		client_body_max = DEFAULT_BODY_MAX;
		//setup_routes = DEFAULT_SETUP_ROUTES;
		server_map[port] = new serverClass(port, host, server_name, default_error_pages, client_body_max);//might add setup_routes
	}
	return (server_map);
}
