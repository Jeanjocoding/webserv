/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 22:00:11 by asablayr          #+#    #+#             */
/*   Updated: 2021/10/03 11:19:05 by asablayr         ###   ########.fr       */
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
#include "deleteHandler.hpp"

#define DEFAULT_CONF_FILE "conf.conf"
#define FILE_NOT_FOUND -1
#define EXTENSION_NOT_VALID -2

std::vector<serverClass*>			setup_server(std::string conf_file);
std::string							read_file(std::string filename);
bool								parse_conf_file(std::string& buff);
void								handle_connection(ConnectionClass& connection);
void								answer_connection(ConnectionClass& connection);
void								print_request(HttpRequest& request);
int 								setCgiParams(t_CgiParams& params, HttpRequest const& request, LocationClass const& location);
bool								setup_CGI(ConnectionClass& connection);
void								answer_CGI(ConnectionClass& connection);

void								add_header_part(HttpResponse& response, char* str, size_t buffer_size, size_t& body_beginning);

#endif
