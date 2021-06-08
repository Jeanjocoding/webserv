/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 22:00:11 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/08 15:19:11 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "serverClass.hpp"
#include "ConnectionClass.hpp"
#include "HttpMessage.hpp"
#include "HttpRequest.hpp"

#define DEFAULT_CONF_FILE "conf.conf"

std::map<std::string, serverClass*>	setup_server(std::string conf_file);
void								handle_connection(ConnectionClass& connection);

#endif
