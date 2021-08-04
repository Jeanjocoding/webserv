/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationClass.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 14:11:07 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/04 21:29:12 by asablayr         ###   ########.fr       */
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

			std::string								getUri(void) const;// get uri of block
			std::string								getParam(void) const;// get the uri match method
			std::string								getRoot(void) const;// get _root attribute
			std::string								getIndex(void) const;// get _root attribute
			std::string								getAutoIndex(void) const;// generate and returns the auto index page
			std::map<unsigned short, std::string>&	getErrorMap(void);// get the _error_pages attribute
			std::string								getErrorPage(unsigned short error_code);// get single error_page
			std::string								getErrorPage(unsigned short error_code) const;// get single error_page
			void									setMethods(void);// set allowed HTTP methods
			void									setErrorPages(std::map<unsigned short, std::string> const& error_map);// set the _error_pages attribute
			bool									methodIsAllowed(unsigned int method) const;// check if HTTP method is allowed
			bool									autoIndexIsOn(void) const;// check if auto index is on
			unsigned int 							matchUri(std::string const& s) const;// returns the number of character match

	private:

			std::string								_uri;
			std::string								_param;
			std::string								_root;
			std::string								_index;
			bool									_methods[3];
			bool									_auto_index;
			std::map<unsigned short, std::string>	_error_pages;

};

#endif
