/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationClass.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/15 13:31:12 by asablayr          #+#    #+#             */
/*   Updated: 2021/09/29 14:35:00 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <dirent.h>
#include "LocationClass.hpp"

LocationClass::LocationClass(): _uri("/"), _param(""), _root("."), _index(""), _autoindex_bool(false)
{
	_methods[GET_METHOD] = true;
	_methods[POST_METHOD] = true;
	_methods[DELETE_METHOD] = true;
}

LocationClass::LocationClass(std::string const& params, std::string const& buff) : contextClass("location", buff), _uri("/"), _param(""), _server_name("webserv"), _root("."), _index("index.html"), _autoindex_bool(false)
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
	setCGI();
	setKeepaliveTimeout();
	setClientBodySizeMax();
	setUploadStore();
	setSendfile();
}

LocationClass::LocationClass(LocationClass const& copy): contextClass(copy), _uri(copy._uri), _param(copy._param), _server_name(copy._server_name), _root(copy._root), _index(copy._index), _autoindex_bool(copy._autoindex_bool), _cgi_bool(copy._cgi_bool), _cgi_path(copy._cgi_path), _redirect_bool(copy._redirect_bool), _redirect_code(copy._redirect_code), _redirect_uri(copy._redirect_uri), _error_pages(copy._error_pages), _keepalive_timeout(copy._keepalive_timeout), _client_body_size_max(copy._client_body_size_max), _upload_store(copy._upload_store), _sendfile(copy._sendfile)
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
	_server_name = copy._server_name;
	_root = copy._root;
	_index = copy._index;
	_autoindex_bool = copy._autoindex_bool;
	_cgi_bool = copy._cgi_bool;
	_cgi_path = copy._cgi_path;
	_redirect_bool = copy._redirect_bool;
	_redirect_code = copy._redirect_code;
	_redirect_uri = copy._redirect_uri;
	_error_pages = copy._error_pages;
	_keepalive_timeout = copy._keepalive_timeout;
	_client_body_size_max = copy._client_body_size_max;
	_methods[GET_METHOD] = copy._methods[GET_METHOD];
	_methods[POST_METHOD] = copy._methods[POST_METHOD];
	_methods[DELETE_METHOD] = copy._methods[DELETE_METHOD];
	_upload_store = copy._upload_store;
	_sendfile = copy._sendfile;
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

long		LocationClass::getKeepaliveTimeout(void) const
{
	return _keepalive_timeout;
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
	for (std::map<unsigned short, std::string>::const_iterator it = error_map.begin(); it != error_map.end(); it++)
	{
		if (_error_pages.find(it->first) == _error_pages.end())
		{
			_error_pages.insert(*it);
		}
	}
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

bool LocationClass::isCGI(void) const
{
	return _cgi_bool;
}

std::string LocationClass::getCGI(void) const
{
	return _cgi_path;
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
	return _autoindex_bool;
}

std::string LocationClass::getAutoindex(void) const
{
	return generateAutoindex(_uri);
}

std::string LocationClass::getAutoindex(std::string const& request_uri) const
{
	return generateAutoindex(request_uri);
}

std::string const& LocationClass::getUploadStore(void) const
{
	return _upload_store;
}

std::string LocationClass::getUploadStore(void)
{
	return _upload_store;
}

bool LocationClass::getSendfile(void) const
{
	return _sendfile;
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

void	LocationClass::setRoot(std::string const& root)
{
	_root = root;
}

void	LocationClass::setServerName(std::string const& server_name)
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

void	LocationClass::setIndex(std::string const& index)
{
	_index = index;
}

void	LocationClass::setKeepaliveTimeout(void)
{
	std::map<std::string, std::string>::const_iterator it = _directives.find("keepalive_timeout");
	if (it == _directives.end())
	{
		_keepalive_timeout = 0;
		return ;
	}
	_keepalive_timeout = atoi(it->second.c_str());
	unsigned int i = 0;
	while (it->second[i] >= '0' && it->second[i] <= '9')
		i++;
	if (it->second[i] == 0 || it->second[i] == 's')
		return ;
	else if (it->second[i] == 'm' && it->second[i + 1] == 's')
		_keepalive_timeout /= 1000;
	else if (it->second[i] == 'm')
		_keepalive_timeout *= 60;
	else if (it->second[i] == 'h')
		_keepalive_timeout *= 360;
	else if (it->second[i] == 'd')
		_keepalive_timeout *= 360 * 24;
	else if (it->second[i] == 'w')
		_keepalive_timeout *= 360 * 24 * 7;
	else if (it->second[i] == 'M')
		_keepalive_timeout *= 360 * 24 * 30;
	else if (it->second[i] == 'y')
		_keepalive_timeout *= 360 * 24 * 365;
}

void	LocationClass::setKeepaliveTimeout(std::string const& val)
{
	_keepalive_timeout = atoi(val.c_str());
	unsigned int i = 0;
	while (val[i] >= '0' && val[i] <= '9')
		i++;
	if (val[i] == 0 || val[i] == 's')
		return ;
	else if (val[i] == 'm' && val[i + 1] == 's')
		_keepalive_timeout /= 1000;
	else if (val[i] == 'm')
		_keepalive_timeout *= 60;
	else if (val[i] == 'h')
		_keepalive_timeout *= 360;
	else if (val[i] == 'd')
		_keepalive_timeout *= 360 * 24;
	else if (val[i] == 'w')
		_keepalive_timeout *= 360 * 24 * 7;
	else if (val[i] == 'M')
		_keepalive_timeout *= 360 * 24 * 30;
	else if (val[i] == 'y')
		_keepalive_timeout *= 360 * 24 * 365;
}

void	LocationClass::setKeepaliveTimeout(long val)
{
	_keepalive_timeout = val;
}

void	LocationClass::setClientBodySizeMax(void)
{
	unsigned int i = 0;
	std::map<std::string, std::string>::const_iterator it = _directives.find("client_max_body_size");
	if (it == _directives.end())
		return ;
	_client_body_size_max = atoi(it->second.c_str());
	while (it->second[i] >= '0' && it->second[i] <= '9')
		i++;
	if (it->second[i] == 0)
		return ;
	else if (it->second[i] == 'K' || it->second[i] == 'k')
		_client_body_size_max *= 1000;
	else if (it->second[i] == 'M' || it->second[i] == 'm')
		_client_body_size_max *= 1000000;
}

void	LocationClass::setClientBodySizeMax(std::string const& val)
{
	unsigned int i = 0;
	_client_body_size_max = atoi(val.c_str());
	while (val[i] >= '0' && val[i] <= '9')
		i++;
	if (val[i] == 0)
		return ;
	else if (val[i] == 'K' || val[i] == 'k')
		_client_body_size_max *= 1000;
	else if (val[i] == 'M' || val[i] == 'm')
		_client_body_size_max *= 1000000;
}

void	LocationClass::setClientBodySizeMax(long val)
{
	_client_body_size_max = val;
}

void	LocationClass::setAutoindex(std::string const& autoindex_str)
{
	if (autoindex_str == "on")
		_autoindex_bool = true;
	else
		_autoindex_bool = false;
}

void	LocationClass::setAutoindex(bool autoindex_bool)
{
		_autoindex_bool = autoindex_bool;
	if (autoindex_bool == true)
		_autoindex_bool = true;
	else
		_autoindex_bool = false;
}

void	LocationClass::setAutoindex(void)
{
	std::map<std::string, std::string>::const_iterator it = _directives.find("autoindex");
	if (it == _directives.end())
		return ;
	if (it->second == "on")
		_autoindex_bool = true;
	else
		_autoindex_bool = false;
}

std::string	LocationClass::generateAutoindex(std::string const& request_uri) const
{
	DIR					*dir;
	struct dirent		*ent;
	std::string			tmp;
	std::string			autoindex;

//	if (tmp[tmp.size() - 1] != '/' && _uri[0] != '/')
//		tmp.append("/");
	tmp.append(request_uri);
//	if (tmp[tmp.size() - 1] != '/')
//		tmp.append("/");
//	std::cout << "root : " << _root << std::endl;
	std::cout << "tmp : " << tmp << std::endl;
	if ((dir = opendir(tmp.c_str())) != NULL)
	{
		autoindex.append("<html>\n<head><title>Index of ");
		autoindex.append(_uri);
		autoindex.append("</title></head>\n<body>\n<h1>Index of ");
		autoindex.append(_uri);
		autoindex.append("</h1>\n<hr><pre>");
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] != '.')
			{
				autoindex.append("<a href=");
				autoindex.append(ent->d_name);
				autoindex.append(">");
				autoindex.append(ent->d_name);
				autoindex.append("</a>\n");
			}
		}
		autoindex.append("</pre><hr></body>\n</html>");
		closedir(dir);
	}
	return autoindex;
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

void	LocationClass::setCGI(void)
{
	if (!(_param == "~*"))
		_cgi_bool = false;
	if (_uri.find(".bla") != std::string::npos || _uri.find(".php") != std::string::npos
		|| _uri.find(".py") != std::string::npos || _uri.find(".pl") != std::string::npos)
		_cgi_bool = true;
	if (_directives.find("cgi_path") != _directives.end())
		_cgi_path = _directives.find("cgi_path")->second;
	else
		_cgi_path = DEFAULT_CGI_BIN_PATH;
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

void	LocationClass::setUploadStore(void)
{
	std::map<std::string, std::string>::const_iterator it = _directives.find("upload_store");
	if (it == _directives.end())
		return ;
	_upload_store = it->second;
}

void	LocationClass::setUploadStore(std::string const& path)
{
	_upload_store = path;
}

void	LocationClass::setSendfile(void)
{
	std::map<std::string, std::string>::const_iterator it = _directives.find("sendfile");
	if (it != _directives.end() && it->second == "off")
	{
		_sendfile = false;
		_methods[POST_METHOD] = false;
	}
	else
		_sendfile = true;
}

void	LocationClass::setSendfile(std::string const& str)
{
	if (str == "off")
	{
		_sendfile = false;
		_methods[POST_METHOD] = false;
	}
	else
		_sendfile = true;
}

void	LocationClass::setSendfile(bool sendfile)
{
	_sendfile = sendfile;
}

/***********************************************************************************/
/*							TESTING												   */
/***********************************************************************************/

void	LocationClass::printLocation(void) const
{
	std::cout << "_uri : " << _uri << std::endl;
	std::cout << "_param : " << _param << std::endl;
	std::cout << "_server_name : " << _server_name << std::endl;
	std::cout << "_root : " << _root << std::endl;
	std::cout << "_index : " << _index << std::endl;
	std::cout << "GET : " << _methods[GET_METHOD] << std::endl;
	std::cout << "DELETE : " << _methods[DELETE_METHOD] << std::endl;
	std::cout << "POST : " << _methods[POST_METHOD] << std::endl;
	std::cout << "autoindex : " << _autoindex_bool << std::endl;
	std::cout << "isCgi : " << _cgi_bool << "\t cgi_path : " << _cgi_path << std::endl;
	std::cout << "isRedirect : " << _redirect_bool << "\tredirect uri : " << _redirect_uri << std::endl;
	std::cout << "keepalive_timeout : " << _keepalive_timeout << std::endl;
	std::cout << "client_body_size_max : " << _client_body_size_max << std::endl;
	std::cout << "upload_store : " << _upload_store << std::endl;
	std::cout << "sendfile : " << _sendfile << std::endl;
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

long LocationClass::getClientBodySizeMax(void) const
{
	return (_client_body_size_max);
}
