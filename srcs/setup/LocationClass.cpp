/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationClass.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/15 13:31:12 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/26 16:04:41 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include "LocationClass.hpp"

LocationClass::LocationClass(): _uri("/"), _param(""), _root("."), _index(""), _autoindex(false)
{
	_methods[GET_METHOD] = true;
	_methods[POST_METHOD] = true;
	_methods[DELETE_METHOD] = true;
}

LocationClass::LocationClass(std::string const& params, std::string const& buff) : contextClass("location", buff), _uri("/"), _param(""), _server_name("webserv"), _root("."), _index("index.html"), _autoindex(false)
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
	setRoot();
	setIndex();
	setAutoindex();
	setErrorPages();
	setRedirect();
	setMethods();
}

LocationClass::LocationClass(LocationClass const& copy): contextClass(copy), _uri(copy._uri), _param(copy._param), _server_name(copy._server_name), _root(copy._root), _index(copy._index), _autoindex(copy._autoindex), _redirect_bool(copy._redirect_bool), _redirect_code(copy._redirect_code), _redirect_uri(copy._redirect_uri), _error_pages(copy._error_pages)
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
	contextClass::operator = (copy);
	_uri = copy._uri;
	_param = copy._param;
	_root = copy._root;
	_index = copy._index;
	_methods[GET_METHOD] = copy._methods[GET_METHOD];
	_methods[POST_METHOD] = copy._methods[POST_METHOD];
	_methods[DELETE_METHOD] = copy._methods[DELETE_METHOD];
	_autoindex = copy._autoindex;
	_error_pages = copy._error_pages;
	return *this;
}

std::string& LocationClass::getUri(void)
{
	return _uri;
}

std::string	const& LocationClass::getUri(void) const
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

std::string const&	LocationClass::getServerName(void) const
{
	return _server_name;
}

std::string&	LocationClass::getServerName(void)
{
	return _server_name;
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

std::map<unsigned short, std::string> LocationClass::getErrorMap(void) const
{
	return _error_pages;
}

std::string&	LocationClass::getErrorPage(unsigned short error_code)
{
	return _error_pages[error_code];
}

std::string		LocationClass::getErrorPage(unsigned short error_code) const
{
	return _error_pages.find(error_code)->second;
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

bool LocationClass::isRedirect(void) const
{
	return _redirect_bool;
}

unsigned short LocationClass::getRedirectCode(void) const
{
	return _redirect_code;
}

std::string LocationClass::getRedirectUrl(void) const
{
	return _redirect_uri;
}

bool LocationClass::autoIndexIsOn(void) const
{
	return _autoindex;
}

unsigned int LocationClass::matchUri(std::string const& s) const
{
	int res = 0;
	for (std::string::const_iterator it = _uri.begin(), i = s.begin(); i != s.end() && it != _uri.end(); i++, it++, res++)
		if (*i != *it)
			return res;
	return res;
}

void	LocationClass::setRoot(void)
{
	std::map<std::string, std::string>::const_iterator it = _directives.find("root");
	if (it == _directives.end())
		return ;
	_root = it->second;
}

void	LocationClass::setRoot(std::string root)
{
	_root = root;
}

void	LocationClass::setServerName(std::string server_name)
{
	_server_name = server_name;
}

void	LocationClass::setIndex(void)
{
	std::map<std::string, std::string>::const_iterator it = _directives.find("index");
	if (it == _directives.end())
		return ;
	_index = it->second;
}

void	LocationClass::setIndex(std::string index)
{
	_index = index;
}

void	LocationClass::setAutoindex(void)
{
	std::map<std::string, std::string>::const_iterator it = _directives.find("autoindex");
	if (it == _directives.end())
		return ;
	if (it->second == "on")
		_autoindex = true;
	else
		_autoindex = false;
}

void	LocationClass::setRedirect(void)
{
	std::map<std::string, std::string>::const_iterator it = _directives.find("return");
	if (it == _directives.end())
		return ;
	std::istringstream ss(it->second);
	std::string tmp;
	_redirect_bool = true;
	ss >> tmp;
	_redirect_code = std::atoi(tmp.c_str());
	ss >> tmp;
	_redirect_uri = tmp;
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

void	LocationClass::setErrorPages(void)
{
	std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> p = _directives.equal_range("error_page");
	if (p.first == _directives.end())
		return ;
	while (p.first != p.second)
	{
		std::istringstream iss(p.first->second);
		std::vector<std::string> vect;
		for (std::string tmp; iss >> tmp;)
			vect.push_back(tmp);
		_error_pages[std::atoi(vect[0].c_str())] = vect[1];
		p.first++;
	}
}

/***********************************************************************************/
/*							TESTING												   */
/***********************************************************************************/

void	LocationClass::printLocation(void) const
{
	std::cout << "_uri : " << _uri << std::endl;
	std::cout << "_param : " << _param << std::endl;
	std::cout << "_root : " << _root << std::endl;
	std::cout << "_index : " << _index << std::endl;
	std::cout << "GET : " << _methods[GET_METHOD] << std::endl;
	std::cout << "DELETE : " << _methods[DELETE_METHOD] << std::endl;
	std::cout << "POST : " << _methods[POST_METHOD] << std::endl;
	std::cout << "autoindex : " << _autoindex << std::endl;
	for (std::map<unsigned short, std::string>::const_iterator it = _error_pages.begin(); it != _error_pages.end(); it++)
		std::cout << "_error_pages " << it->first << " : " << it->second << std::endl;
}

void	LocationClass::printDirectives(void) const
{
	if (_directives.empty())
	{
		std::cout << "empty directives\n";
		return;
	}
	std::cout << "directives : " << _directives.size() << "\n";
	for (std::multimap<std::string, std::string>::const_iterator it = _directives.begin(); it != _directives.end(); it++)
		std::cout << "_directive " << it->first << " : " << it->second << std::endl;
}
