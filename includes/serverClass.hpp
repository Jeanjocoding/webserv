/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClass.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 10:16:05 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/04 19:48:38 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

#include <map>
#include <vector>
#include "error_page_path.hpp"
#include "default_server_settings.hpp"
#include "contextClass.hpp"

class serverClass
{
	public:

			serverClass();
			serverClass(serverClass const& to_copy);

			~serverClass(void);

			serverClass& operator = (serverClass const& to_copy);
			std::string* operator [] (std::string setting_name);

			void	startServer(void);

			bool									_default_server;
			std::string								_listen;
			std::string								_port;
			std::string								_host;
			std::string								_server_name;
			std::string								_root;
			std::string								_index;
			std::string								_error_log;
			std::string								_access_log;
			std::string								_fastcgi_pass;
			std::map<unsigned short, std::string>	_default_error_pages;
			std::string								_client_body_size_max;
			std::string								_keepalive_timeout;
			std::map<std::string, contextClass*>	_location;

			int										_server_socket;
			struct addrinfo							*_addr;

	private:
			std::map<unsigned short, std::string>	baseErrorPages(void);

};

#endif
