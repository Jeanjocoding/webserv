/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 19:42:54 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/17 20:08:40 by asablayr         ###   ########.fr       */
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
		if (base_server[it->first])
			*(base_server[it->first]) = it->second;
	for (auto it = context._blocks.begin(); it != context._blocks.end(); it++)
	{
		if (it->_name == "server")
		{
			serverClass new_server = base_server;
			input_context(*it, new_server, vector_server);
			vector_server.push_back(&new_server);
		}
		else
			return (input_context(*it, base_server, vector_server));
	}
}

std::vector<serverClass*>	setup_server(std::string conf_file)
{
	serverClass					base_serv;
	serverClass					new_serv;
	std::vector<serverClass*>	server_map;

	std::string								buff;

	buff = read_file(conf_file);
	if (parse_conf_file(buff))
	{
		contextClass main_context("main", buff);
		input_context(main_context, base_serv, server_map);
		if (server_map.empty())
			server_map.push_back(&base_serv);
	}
	else
	{
		std::cerr << "error in " << conf_file << " configuration file\n";
		exit(EXIT_FAILURE);
	}
	return (server_map);
}
