/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 19:42:54 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/18 13:35:24 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <iostream>
#include <fstream>
#include "serverClass.hpp"
#include "webserv.hpp"

static void	input_context(contextClass const& context, serverClass& base_server, std::vector<serverClass*>& vector_server)
{
	for (auto it = context._directives.begin(); it != context._directives.end(); it++)
	{	
		if (base_server[it->first])
		{
			*(base_server[it->first]) = it->second;
			std::cout << "server's " << it->first << " = " << it->second << " from context " << context._name << std::endl;
		}
	}
	for (auto it = context._blocks.begin(); it != context._blocks.end(); it++)
	{
		std::cout << "iterator in context " << context._name << " : " << (*it)->_name << std::endl;
		if ((*it)->_name == "server")
		{
			serverClass new_server = base_server;
			input_context(**it, new_server, vector_server);
			vector_server.push_back(&new_server);
		}
		else
			input_context(**it, base_server, vector_server);
	}
}

std::vector<serverClass*>	setup_server(std::string conf_file)
{
	serverClass					base_serv;
	std::vector<serverClass*>	server_map;

	std::string								buff;

	buff = read_file(conf_file);
	if (parse_conf_file(buff))
	{
		std::cout << buff << std::endl;
		contextClass main_context("main", buff);
		for (auto it = main_context._blocks.begin(); it != main_context._blocks.end(); it++)
			std::cout << "cooontexteeeeuuuh : " << (*it)->_name << "\n";
		input_context(main_context, base_serv, server_map);
		if (server_map.empty())
		{
			std::cout << "no server in conf file\n";
			server_map.push_back(&base_serv);
		}
	}
	else
	{
		std::cerr << "error in " << conf_file << " configuration file\n";
		exit(EXIT_FAILURE);
	}
	return (server_map);
}
