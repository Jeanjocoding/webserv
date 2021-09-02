/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationClass.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 14:11:07 by asablayr          #+#    #+#             */
/*   Updated: 2021/09/01 14:41:48 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCLASS_HPP
# define LOCATIONCLASS_HPP

#include <vector>
#include <map>
#include <string>
#include "contextClass.hpp"
#include "http_method.hpp"

class LocationClass : public contextClass
{
	public:
			LocationClass();
//			LocationClass(serverClass const& server);
			LocationClass(std::string const& context_name, std::string const& buff);
			LocationClass(LocationClass const&);

			LocationClass& operator = (LocationClass const& copy);

			~LocationClass();

			std::string&							getUri(void);// get uri of block
			std::string const&						getUri(void) const;// get uri of block
			std::string								getParam(void) const;// get the uri match method
			std::string								getRoot(void) const;// get _root attribute
			void									setRoot(std::string const& root);
			std::string const&						getServerName(void) const;// get _server_name attribute
			std::string&							getServerName(void);// get _server_name attribute
			void									setServerName(std::string const& server_name);// set _server_name
			std::string								getIndex(void) const;// get _index attribute
			void									setIndex(std::string const& index);
			long									getKeepaliveTimeout(void) const;
			void									setKeepaliveTimeout(void);
			void									setKeepaliveTimeout(std::string const& val);
			void									setKeepaliveTimeout(long val);
			long									getClientBodySizeMax(void) const;
			void									setClientBodySizeMax(void);
			void									setClientBodySizeMax(std::string const& val);
			void									setClientBodySizeMax(long val);
			std::string								getAutoIndex(void) const;// generate and returns the auto index page as string
			std::map<unsigned short, std::string>&	getErrorMap(void);// get the _error_pages attribute
			std::map<unsigned short, std::string>	getErrorMap(void) const;// get the _error_pages attribute
			std::string&							getErrorPage(unsigned short error_code);// get single error_page
			std::string								getErrorPage(unsigned short error_code) const;// get single error_page
			void									setErrorPages(std::map<unsigned short, std::string> const& error_map);// set the _error_pages attribute
			bool									methodIsAllowed(unsigned int method) const;// check if HTTP method is allowed
			bool									isCGI(void) const;
			bool									isRedirect(void) const;
			unsigned short							getRedirectCode(void) const;
			std::string								getRedirectUrl(void) const;
			bool									autoIndexIsOn(void) const;// check if auto index is on
			unsigned int 							matchUri(std::string const& s) const;// returns the number of character match

			/*TESTING*/
			void    								printLocation(void) const;
			void    								printDirectives(void) const;

	private:

			void									setRoot(void);
			void									setIndex(void);
			void									setAutoindex(void);
			void									setRedirect(void);
			void									setCGI(void);
			void									setErrorPages(void);
			void									setMethods(void);

			std::string								_uri;
			std::string								_param;
			std::string								_server_name;
			std::string								_root;
			std::string								_index;
			bool									_methods[3];
			bool									_autoindex;
			bool									_cgi_bool;
			std::string								_cgi_path;
			bool									_redirect_bool;
			int										_redirect_code;
			std::string								_redirect_uri;
			std::map<unsigned short, std::string>	_error_pages;
			long									_keepalive_timeout;
			long									_client_body_size_max;

};

#endif
