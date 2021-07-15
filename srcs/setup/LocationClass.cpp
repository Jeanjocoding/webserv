/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationClass.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/15 13:31:12 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/15 14:56:17 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationClass.hpp"

LocationClass::LocationClass()
{
	_uri = "/";
	_param = "";
	_methods[GET_METHOD] = true;
	_methods[POST_METHOD] = true;
	_methods[DELETE_METHOD] = true;
}

LocationClass::LocationClass(LocationClass const& copy)
{
	_uri = copy._uri;
	_param = copy._param;
	_methods[GET_METHOD] = copy._methods[GET_METHOD];
	_methods[POST_METHOD] = copy._methods[POST_METHOD];
	_methods[DELETE_METHOD] = copy._methods[DELETE_METHOD];
}

LocationClass::~LocationClass()
{
}


LocationClass& LocationClass::operator = (LocationClass const& copy)
{
	_uri = copy._uri;
	_param = copy._param;
	_methods[GET_METHOD] = copy._methods[GET_METHOD];
	_methods[POST_METHOD] = copy._methods[POST_METHOD];
	_methods[DELETE_METHOD] = copy._methods[DELETE_METHOD];
	return *this;
}

unsigned int LocationClass::matchUri(std::string const& s)
{
	int res = 0;
	for (std::string::const_iterator it = _uri.begin(), i = s.begin(); i != s.end() && it != _uri.end(); i++, it++, res++)
		if (*i != *it)
			return res;
	return res;
}
