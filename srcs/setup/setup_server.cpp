/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 19:42:54 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/18 13:41:45 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "serverClass.hpp"
#include "webserv.hpp"

static void	input_context(contextClass const& context, serverClass& base_server, std::vector<serverClass*>& vector_server)
{
	for (std::map<std::string, std::string>::const_iterator it = context._directives.begin(); it != context._directives.end(); it++)
		if (base_server[it->first])
			*(base_server[it->first]) = it->second;
	for (std::vector<contextClass*>::const_iterator it = context._blocks.begin(); it != context._blocks.end(); it++)
	{
		if ((*it)->_name == "server")
		{
			serverClass* new_server = new serverClass(base_server);
			input_context(**it, *new_server, vector_server);
			vector_server.push_back(new_server);
		}
		else if ((*it)->_name == "location")
		{
			LocationClass *i = dynamic_cast<LocationClass*>(*it);
			base_server._location[i->_uri] = new LocationClass(*i);
		}
		else
			input_context(**it, base_server, vector_server);
	}
}

std::vector<serverClass*>	setup_server(std::string conf_file)
{
	serverClass*				base_serv;
	std::vector<serverClass*>	server_map;

	std::string								buff;

	buff = read_file(conf_file);
	base_serv = new serverClass();
	if (parse_conf_file(buff))
	{
		contextClass main_context("main", buff);
		input_context(main_context, *base_serv, server_map);
		if (server_map.empty())
			server_map.push_back(base_serv);
	}
	else
	{
		std::cerr << "error in " << conf_file << " configuration file\n";
		exit(EXIT_FAILURE);
	}
	return (server_map);
}
