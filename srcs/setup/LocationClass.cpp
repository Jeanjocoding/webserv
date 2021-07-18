/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationClass.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/15 13:31:12 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/18 13:41:25 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include "LocationClass.hpp"

LocationClass::LocationClass()
{
	_uri = "/";
	_param = "";
	_methods[GET_RANK] = true;
	_methods[POST_RANK] = true;
	_methods[DELETE_RANK] = true;
}

LocationClass::LocationClass(std::string const& params, std::string const& buff)
{
	std::istringstream iss(params);
	std::string tmp;
	iss >> _uri;
	if (iss >> tmp)
	{
		_param = _uri;
		_uri = tmp;
	}
	_methods[GET_RANK] = true;
	_methods[POST_RANK] = true;
	_methods[DELETE_RANK] = true;
	setContext("location", buff);
	setMethods();
}

LocationClass::LocationClass(LocationClass const& copy)
{
	_uri = copy._uri;
	_param = copy._param;
	_methods[GET_RANK] = copy._methods[GET_RANK];
	_methods[POST_RANK] = copy._methods[POST_RANK];
	_methods[DELETE_RANK] = copy._methods[DELETE_RANK];
}

LocationClass::~LocationClass()
{
}


LocationClass& LocationClass::operator = (LocationClass const& copy)
{
	_uri = copy._uri;
	_param = copy._param;
	_methods[GET_RANK] = copy._methods[GET_RANK];
	_methods[POST_RANK] = copy._methods[POST_RANK];
	_methods[DELETE_RANK] = copy._methods[DELETE_RANK];
	return *this;
}

void	LocationClass::setMethods(void)
{
	std::map<std::string, std::string>::iterator it = _directives.find("limit_except");
	if (it == _directives.end())
		return ;
	std::istringstream iss(it->second);
	std::vector<std::string> vect;
	for (std::string tmp; iss >> tmp;)
		vect.push_back(tmp);
	_methods[GET_RANK] = false;
	_methods[POST_RANK] = false;
	_methods[DELETE_RANK] = false;
	for (std::vector<std::string>::iterator i = vect.begin(); i != vect.end(); i++)
	{
		if (*i == "GET")
				_methods[GET_RANK] = true;
		else if (*i == "POST")
				_methods[POST_RANK] = true;
		else if (*i == "DELETE")
				_methods[DELETE_RANK] = true;
		else
			continue;
	}	
}

unsigned int LocationClass::matchUri(std::string const& s) const
{
	int res = 0;
	for (std::string::const_iterator it = _uri.begin(), i = s.begin(); i != s.end() && it != _uri.end(); i++, it++, res++)
		if (*i != *it)
			return res;
	return res;
}
