/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClass.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 10:16:05 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/07 23:15:27 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

#include <map>
#include "error_page_path.hpp"

#define	DEFAULT_PORT "8001"
#define	DEFAULT_HOST ""
#define	DEFAULT_SERVER_NAME "webserver"
#define	DEFAULT_BODY_MAX 1000000

class serverClass
{
	public:

			serverClass(std::string port, std::string host, std::string server_name, std::map<unsigned short, std::string> error_pages, unsigned int client_body_max);//might add setup_routes

			std::string							_port;
			std::string							_host;
			std::string							_server_name;
			std::map<unsigned short, std::string>	_default_error_pages;
			int									_client_body_size_max;
//			std::string*						_setup_routes;

			int									_server_socket;
			struct addrinfo						*_addr;

	private:

};

#endif
