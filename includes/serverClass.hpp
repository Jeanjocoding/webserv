/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverClass.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 10:16:05 by asablayr          #+#    #+#             */
/*   Updated: 2021/09/13 20:34:16 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

#include <map>
#include <vector>
#include "error_page_path.hpp"
#include "default_server_settings.hpp"
#include "LocationClass.hpp"

class serverClass
{
	public:

			serverClass();
			serverClass(serverClass const& to_copy);

			~serverClass(void);

			serverClass& operator = (serverClass const& to_copy);
			std::string* operator [] (std::string setting_name);

			void			setListen(std::string);
			void			setPort(std::string);
			void			setHost(std::string);
			void			setServerName(std::string);
			void			setRoot(std::string);
			void			setIndex(std::string);
			void			setErrorLog(std::string);
			void			setAccessLog(std::string);
			void			setLocation(void);
			void			setLocation(LocationClass& location) const;
			void			startServer(void);
			LocationClass&	getLocation(std::string const& uri) const;//TODO cgi
			long	getKeepAliveTimeout(void) const;

			bool									_default_server;
			std::string								_listen;
			std::string								_port;
			std::string								_host;
			std::string								_server_name;
			std::string								_root;
			std::string								_index;
			std::string								_error_log;
			std::string								_access_log;
			std::map<unsigned short, std::string>	_default_error_pages;
			std::string								_client_body_size_max;
			std::string								_keepalive_timeout;
			std::string								_upload_store;
			std::string								_sendfile;
			std::string								_autoindex;
			std::vector<LocationClass*>				_location;

			int										_server_socket;
			struct addrinfo							*_addr;

	private:

			unsigned long	caseSensitiveReMatch(std::string const& s1, std::string const& s2) const;
			unsigned long	caseInsensitiveReMatch(std::string const& s1, std::string const& s2) const;
			unsigned long	caseSensitiveMatch(std::string const& s1, std::string const& s2) const;

			std::map<unsigned short, std::string>	baseErrorPages(void);

};

#endif
