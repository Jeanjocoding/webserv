/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationClass.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/15 13:31:12 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/04 21:31:59 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include "LocationClass.hpp"

LocationClass::LocationClass(): _uri("/"), _param(""), _root("."), _index("index.html"), _auto_index(false)
{
	_methods[GET_METHOD] = true;
	_methods[POST_METHOD] = true;
	_methods[DELETE_METHOD] = true;
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
	_methods[GET_METHOD] = true;
	_methods[POST_METHOD] = true;
	_methods[DELETE_METHOD] = true;
	setContext("location", buff);
	setMethods();
}

LocationClass::LocationClass(LocationClass const& copy): _uri(copy._uri), _param(copy._param), _root(copy._root), _index(copy._index), _auto_index(copy._auto_index)
{
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
	_root = copy._root;
	_index = copy._index;
	_methods[GET_METHOD] = copy._methods[GET_METHOD];
	_methods[POST_METHOD] = copy._methods[POST_METHOD];
	_methods[DELETE_METHOD] = copy._methods[DELETE_METHOD];
	_auto_index = copy._auto_index;
	return *this;
}

std::string	LocationClass::getUri(void) const
{
	return _uri;
}

std::string	LocationClass::getParam(void) const
{
	return _param;
}

std::string	LocationClass::getRoot(void) const
{
	return _root;
}

std::string	LocationClass::getIndex(void) const
{
	return _index;
}

std::string	LocationClass::getAutoIndex(void) const
{
	//TODO
	return std::string();
}

std::map<unsigned short, std::string>& LocationClass::getErrorMap(void)
{
	return _error_pages;
}

std::string	LocationClass::getErrorPage(unsigned short error_code)
{
	return _error_pages[error_code];
}

std::string	LocationClass::getErrorPage(unsigned short error_code) const
{
	return _error_pages.find(error_code)->second;
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
	_methods[GET_METHOD] = false;
	_methods[POST_METHOD] = false;
	_methods[DELETE_METHOD] = false;
	for (std::vector<std::string>::iterator i = vect.begin(); i != vect.end(); i++)
	{
		if (*i == "GET")
			_methods[GET_METHOD] = true;
		else if (*i == "POST")
			_methods[POST_METHOD] = true;
		else if (*i == "DELETE")
			_methods[DELETE_METHOD] = true;
		else
			continue;
	}	
}

void	LocationClass::setErrorPages(std::map<unsigned short, std::string> const& error_map)
{
	_error_pages = error_map;
}

bool LocationClass::methodIsAllowed(unsigned int method) const
{
	if (method < 0 || method > 2)
		return false;
	else 
		return _methods[method];
}

bool LocationClass::autoIndexIsOn(void) const
{
	return _auto_index;
}

unsigned int LocationClass::matchUri(std::string const& s) const
{
	int res = 0;
	for (std::string::const_iterator it = _uri.begin(), i = s.begin(); i != s.end() && it != _uri.end(); i++, it++, res++)
		if (*i != *it)
			return res;
	return res;
}
