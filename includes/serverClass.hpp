/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClass.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 10:16:05 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/04 18:31:18 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

#include <map>
#include <vector>
#include "error_page_path.hpp"
#include "contextClass.hpp"

#define	DEFAULT_PORT "8001"
#define	DEFAULT_HOST "0.0.0.0"
#define	DEFAULT_LISTEN "0.0.0.0:8001"
#define	DEFAULT_SERVER_NAME "webserver"
#define DEFAULT_ROOT "/"
#define DEFAULT_INDEX "/"
#define DEFAULT_ERROR_LOG "error.log"
#define DEFAULT_ACCESS_LOG "access.log"
#define DEFAULT_FASTCGI_PASS "false"
#define	DEFAULT_BODY_MAX "1000000"
#define DEFAULT_KEEPALIVE_TIMEOUT "45"

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
