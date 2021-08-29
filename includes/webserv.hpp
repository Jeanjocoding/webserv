/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 22:00:11 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/28 08:54:34 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <vector>

#include "serverClass.hpp"
#include "ConnectionClass.hpp"
#include "HttpMessage.hpp"
#include "HttpRequest.hpp"
#include "contextClass.hpp"
#include "cgiLauncher.hpp"
#include "PostHandler.hpp"
#include "deleteHandler.hpp"

#define DEFAULT_CONF_FILE "conf.conf"

std::vector<serverClass*>			setup_server(std::string conf_file);
std::string							read_file(std::string filename);
bool								parse_conf_file(std::string& buff);
void								handle_connection(ConnectionClass& connection);
void								answer_connection(ConnectionClass& connection);

void								setCgiParams(t_CgiParams& params, HttpRequest const& request, LocationClass const& location);

#endif
