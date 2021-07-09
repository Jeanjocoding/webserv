/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   contextClass.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 15:24:19 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/09 22:15:30 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include <sstream>
#include "contextClass.hpp"

contextClass::contextClass()
{
}

contextClass::contextClass(std::string name, std::string buff): _name(name)
{
	setAcceptedDirectives();
	setDirectives();
	setBlocks();
	
	if (_name != "location")
		_block_content = getBlock(_name, buff).second;
	else
		_block_content = getParamedBlock(_name, buff).second;
	if (_name != "main")
	{
		_block_content.erase(0, _block_content.find('{') + 1);
		_block_content.erase(_block_content.rfind('}'), 1);
	}
	getBlocksInContext();
	getDirectivesInContext();
	getAcceptedDirectivesInContext();
	while (!_block_content.empty() && _block_content[0] == ' ')
		_block_content.erase(0, 1);
	if (!_block_content.empty())
	{
		std::cerr << "gibberish spotted in context " << _name << " : " << _block_content << std::endl;
		exit(EXIT_FAILURE);
	}
}

contextClass::contextClass(contextClass const& copy)
{
	_name = copy._name;
	_param = copy._param;
	_block_content = copy._block_content;
	_directive_set = copy._directive_set;
	_block_set = copy._block_set;
	_directives = copy._directives;
	_blocks = copy._blocks;
	_accepted_directive_set = copy._accepted_directive_set;
}

contextClass& contextClass::operator = (contextClass const& copy)
{
	_name = copy._name;
	_param = copy._param;
	_block_content = copy._block_content;
	_directive_set = copy._directive_set;
	_block_set = copy._block_set;
	_directives = copy._directives;
	_blocks = copy._blocks;
	_accepted_directive_set = copy._accepted_directive_set;
	return *this;
}

contextClass::~contextClass()
{
	for (std::vector<contextClass*>::iterator it = _blocks.begin(); it != _blocks.end(); it++)
		delete *it;
}

void contextClass::setBlocks(void)
{
    if (_name == "http")
	{
       _block_set.push_back("server");
	}
	else if (_name == "events")
	{
//		_block_set = {};//empty for now
		return;
	}
	else if (_name == "server")
	{
		_block_set.push_back("location");
	}
	else if (_name == "location")
	{
//		_block_set = {};
		return;
	}
	else
	{
		_block_set.push_back("http");
		_block_set.push_back("events");//might delete
	}
}

void contextClass::setDirectives(void)
{
    if (_name == "http")
	{
       _directive_set.push_back(_accepted_directive_set["access_log"]);
       _directive_set.push_back(_accepted_directive_set["error_log"]);
       _directive_set.push_back(_accepted_directive_set["index"]);
	}
	else if (_name == "events")
	{
//		_directive_set = {};
		return;
	}
    else if (_name == "server")
	{
       _directive_set.push_back(_accepted_directive_set["listen"]);
       _directive_set.push_back(_accepted_directive_set["server_name"]);
       _directive_set.push_back(_accepted_directive_set["access_log"]);
       _directive_set.push_back(_accepted_directive_set["error_log"]);
       _directive_set.push_back(_accepted_directive_set["root"]);
       _directive_set.push_back(_accepted_directive_set["return"]);
	}
    else if (_name == "location")
	{
       _directive_set.push_back(_accepted_directive_set["fastcgi_pass"]);
       _directive_set.push_back(_accepted_directive_set["access_log"]);
       _directive_set.push_back(_accepted_directive_set["error_log"]);
       _directive_set.push_back(_accepted_directive_set["root"]);
       _directive_set.push_back(_accepted_directive_set["return"]);
       _directive_set.push_back(_accepted_directive_set["limit_accept"]);
	}
    else
	{
       _directive_set.push_back(_accepted_directive_set["access_log"]);
       _directive_set.push_back(_accepted_directive_set["error_log"]);
	}
}


bool contextClass::check_before(std::string const& buff, std::size_t i) const
{
	if (i == 0)
		return true;
	i--;
	while (i > 0 && buff[i] == ' ')
		i--;
    if (i > 0 && (buff[i] != '{' && buff[i] != '}' && buff[i] != ';'))
        return false;
    return true;
}

bool contextClass::check_after(std::string const& buff, std::size_t& i) const
{
	size_t	tmp = i;
	while (buff[tmp] && buff[tmp] == ' ')
    	tmp++;
	if (!buff[tmp] || buff[tmp] != '{')
		return false;
	i = tmp;
	return true;
}

std::pair<bool, std::string>	contextClass::getBlock(std::string const& block_name, std::string const& buff) const
{
	std::string						block;
	std::string::const_iterator		it = buff.begin();
	std::string::const_iterator		ite = buff.begin();
	std::size_t						i;
	int								brackets;
	std::pair<bool, std::string>	res;

	res.first = false;
	res.second = buff;
	i = buff.find(block_name);
	while (i != std::string::npos)
	{
		it = buff.begin() + i;
		i += block_name.size();
		if (check_before(buff, i - block_name.size()) && check_after(buff, i))
			break;
		i = buff.find(block_name, i);
		continue;
	}
	if (i == std::string::npos)//no block found
		return res;
	brackets = 1;
	while (buff[++i] && brackets)
	{
		if (buff[i] == '{')
			brackets++;
		else if (buff[i] == '}')
			brackets--;
	}
	if (brackets)
		return res;
	ite += i;
	res.first = true;
	res.second = std::string(it, ite);
	return res;
}

std::pair<std::string, std::string>	contextClass::getParamedBlock(std::string const& block_name, std::string const& buff) const
{
	std::string							block;
	std::string							tmp;
	std::string::const_iterator			it = buff.begin();
	std::string::const_iterator			param_it = buff.begin();
	std::string::const_iterator			ite = buff.begin();
	std::string::const_iterator			param_ite = buff.begin();
	std::size_t							i;
	int									brackets;
	std::pair<std::string, std::string>	res;

	res.first = std::string();
	res.second = buff;
	i = buff.find(block_name);
	while (i != std::string::npos)
	{
		it = buff.begin() + i;
		i += block_name.size();
		if (check_before(buff, i - block_name.size()))
		{
			while (i < buff.size() && buff[i] == ' ')
				i++;
			param_it += i;
			i = buff.find(' ', i);
			if (check_after(buff, i))
			{
				param_ite += i;
				tmp = std::string(param_it, param_ite);
				break;
			}
			else
			{
				param_ite += i;
				tmp = std::string(param_it, param_ite);
				if (tmp == "~*" || tmp == "~" || tmp == "^~" || tmp == "=" || tmp == "~")
				{
					while (buff[i] && buff[i] == ' ')
						i++;
					i = buff.find(' ', i);
					if (check_after(buff, i))
					{
						param_ite = buff.begin() + i;
						tmp = std::string(param_it, param_ite);
						break;
					}
				}
			}
				
		}
		i = buff.find(block_name, i);
		continue;
	}
	if (i == std::string::npos)//no block found
		return res;
	brackets = 1;
	while (buff[++i] && brackets)
	{
		if (buff[i] == '{')
			brackets++;
		else if (buff[i] == '}')
			brackets--;
	}
	if (brackets)
		return res;
	ite += i;
	res.first = tmp;
	res.second = std::string(it, ite);
	return res;
}

std::pair<bool, std::string>	contextClass::getSingleDirective(std::string const& directive_name, std::string const& buff) const
{
	std::pair<bool, std::string>	res;
	std::size_t						i;
	std::string::const_iterator		it = buff.begin();
	std::string::const_iterator		ite = buff.begin();

	res.first = false;
	res.second = buff;
	i = buff.find(directive_name);
	while (i != std::string::npos)
	{
		it += i;
		i += directive_name.size();
		if (check_before(buff, i - directive_name.size()))
			break;
		i = buff.find(directive_name, i);
	}
	if (i == std::string::npos)//no block found
		return res;
	while (*it == ' ')
		it++;
	i = buff.find(";", i);
	if (i == std::string::npos)
		return res;
	ite += i;
	res.first = true;
	res.second = std::string(it, ite);
	return res;
}

void	contextClass::getBlocksInContext(void)
{
	for (std::vector<std::string>::iterator it = _block_set.begin(); it != _block_set.end(); it++)
	{
		if (*it == "location")
		{
			for (std::pair<std::string, std::string> check = getParamedBlock(*it, _block_content); !check.first.empty(); check = getParamedBlock(*it, _block_content))
			{
				_blocks.push_back(new contextClass(*it, check.second));
				_blocks[_blocks.size() - 1]->_param = check.first;
				_block_content.erase(_block_content.find(check.second), check.second.size());
			}
		}
		else
		{
			for (std::pair<bool, std::string> check = getBlock(*it, _block_content); check.first; check = getBlock(*it, _block_content))
			{
				_blocks.push_back(new contextClass(*it, check.second));
				_block_content.erase(_block_content.find(check.second), check.second.size());
			}
		}
	}
}

void	contextClass::getDirectivesInContext(void)
{
	for (std::vector<directiveClass>::iterator it = _directive_set.begin(); it != _directive_set.end(); it++)
	{
		std::pair<bool, std::string>check = getSingleDirective(it->_name, _block_content);//get directive and erase it from buffer
		if (check.first)
		{
			_block_content.erase(_block_content.find(check.second), check.second.size() + 1);
			check.second.erase(check.second.find(it->_name), (it->_name).size());
			while (check.second[0] == ' ')
				check.second.erase(0, 1);
			while (check.second.size() && check.second[check.second.size() - 1] == ' ')
				check.second.erase(check.second.size() - 1, 1);
			if (check.second.empty())
			{
				std::cerr << "empty directive " << it->_name << std::endl;
				exit(EXIT_FAILURE);
			}
			if (it->parse(check.second))
				_directives[it->_name] = check.second;
			else
			{
				std::cerr << "wrong " << it->_name << " directive argument : " << check.second << " in context " << _name << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (getSingleDirective(it->_name, _block_content).first)//check for same directive
		{
			std::cerr << "error configuration file : directive " << it->_name << " found twice in same context" << std::endl;//switch to define
			exit(EXIT_FAILURE);
		}
	}
}

void	contextClass::getAcceptedDirectivesInContext(void)
{
	while (!_block_content.empty())
	{
		while (_block_content[0] == ' ')
			_block_content.erase(0, 1);
		std::string::iterator it = _block_content.begin();
		while (it != _block_content.end() && *it != ' ' && *it != ';' && *it != '{' && *it != '}')
			it++;
		std::string temp(_block_content.begin(), it);
		if (temp.empty())
			return ;
		std::map<std::string, directiveClass>::iterator directive = _accepted_directive_set.find(temp);
		if (directive != _accepted_directive_set.end() && (directive->second.isInContext(_name)))
		{
			std::pair<bool, std::string> check;
			if (directive->second._syntax == SYNTAX_BLOCK)
				check = getBlock(temp, _block_content);
			else
				check = getSingleDirective(temp, _block_content);
			if (check.first)
				_block_content.erase(_block_content.find(check.second), check.second.size() + 1);
			else
			{
				std::cerr << "invalid directive " << directive->second._name << " spotted in context " << _name << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			std::cerr << "gibberish spotted in Cocontext " << _name << " : " << _block_content << "\n";
			exit(EXIT_FAILURE);
		}
		while (_block_content[0] == ' ')
			_block_content.erase(0, 1);
	}
}

std::vector<std::string> contextClass::setAcceptedDirectiveContext(std::string const& buff)
{
	std::vector<std::string> ret;
	std::istringstream iss(buff);
	for (std::string s; iss >> s;)
		ret.push_back(s);
	return ret;
}

void contextClass::setAcceptedDirectives(void)
{
	_accepted_directive_set["absolute_redirect"]._name = "absolute_redirect";
	_accepted_directive_set["access_log"]._name = "access_log";
	_accepted_directive_set["aio"]._name = "aio";
	_accepted_directive_set["aio_write"]._name = "aio_write";
	_accepted_directive_set["alias"]._name = "alias";
	_accepted_directive_set["auth_delay"]._name = "auth_delay";
	_accepted_directive_set["chunked_transfer_encoding"]._name = "chunked_transfer_encoding";
	_accepted_directive_set["client_body_buffer_size"]._name = "client_body_buffer_size";
	_accepted_directive_set["client_body_in_file_only"]._name = "client_body_in_file_only";
	_accepted_directive_set["client_body_in_single_buffer"]._name = "client_body_in_single_buffer";
	_accepted_directive_set["client_body_temp_path"]._name = "client_body_temp_path";
	_accepted_directive_set["client_body_timeout"]._name = "client_body_timeout";
	_accepted_directive_set["client_header_buffer_size"]._name = "client_header_buffer_size";
	_accepted_directive_set["client_header_timeout"]._name = "client_header_timeout";
	_accepted_directive_set["client_max_body_size"]._name = "client_max_body_size";
	_accepted_directive_set["connection_pool_size"]._name = "connection_pool_size";
	_accepted_directive_set["default_type"]._name = "default_type";
	_accepted_directive_set["directio"]._name = "directio";
	_accepted_directive_set["directio_alignment"]._name = "directio_alignment";
	_accepted_directive_set["disable_symlinks"]._name = "disable_symlinks";
	_accepted_directive_set["error_log"]._name = "error_log";
	_accepted_directive_set["error_page"]._name = "error_page";
	_accepted_directive_set["etag"]._name = "etag";
	_accepted_directive_set["fastcgi_pass"]._name = "fastcgi_pass";
	_accepted_directive_set["gzip"]._name = "gzip";
	_accepted_directive_set["http"]._name = "http";
	_accepted_directive_set["if_modified_since"]._name = "if_modified_since";
	_accepted_directive_set["ignore_invalid_headers"]._name = "ignore_invalid_headers";
	_accepted_directive_set["index"]._name = "index";
	_accepted_directive_set["include"]._name = "include";
	_accepted_directive_set["internal"]._name = "internal";
	_accepted_directive_set["keepalive_disable"]._name = "keepalive_disable";
	_accepted_directive_set["keepalive_requests"]._name = "keepalive_requests";
	_accepted_directive_set["keepalive_time"]._name = "keepalive_time";
	_accepted_directive_set["keepalive_timeout"]._name = "keepalive_timeout";
	_accepted_directive_set["large_client_header_buffers"]._name = "large_client_header_buffers";
	_accepted_directive_set["limit_except"]._name = "limit_except";
	_accepted_directive_set["limit_rate"]._name = "limit_rate";
	_accepted_directive_set["limit_rate_after"]._name = "limit_rate_after";
	_accepted_directive_set["lingering_close"]._name = "lingering_close";
	_accepted_directive_set["lingering_time"]._name = "lingering_time";
	_accepted_directive_set["lingering_timeout"]._name = "lingering_timeout";
	_accepted_directive_set["listen"]._name = "listen";
	_accepted_directive_set["location"]._name = "location";
	_accepted_directive_set["log_not_found"]._name = "log_not_found";
	_accepted_directive_set["log_subrequest"]._name = "log_subrequest";
	_accepted_directive_set["max_ranges"]._name = "max_ranges";
	_accepted_directive_set["merge_slashes"]._name = "merge_slashes";
	_accepted_directive_set["msie_padding"]._name = "msie_padding";
	_accepted_directive_set["msie_refresh"]._name = "msie_refresh";
	_accepted_directive_set["open_file_cache"]._name = "open_file_cache";
	_accepted_directive_set["open_file_cache_errors"]._name = "open_file_cache_errors";
	_accepted_directive_set["open_file_cache_min_uses"]._name = "open_file_cache_min_uses";
	_accepted_directive_set["open_file_cache_valid"]._name = "open_file_cache_valid";
	_accepted_directive_set["output_buffers"]._name = "output_buffers";
	_accepted_directive_set["pid"]._name = "pid";
	_accepted_directive_set["port_in_redirect"]._name = "port_in_redirect";
	_accepted_directive_set["postpone_output"]._name = "postpone_output";
	_accepted_directive_set["read_ahead"]._name = "read_ahead";
	_accepted_directive_set["recursive_error_pages"]._name = "recursive_error_pages";
	_accepted_directive_set["request_pool_size"]._name = "request_pool_size";
	_accepted_directive_set["reset_timedout_connection"]._name = "reset_timedout_connection";
	_accepted_directive_set["resolver"]._name = "resolver";
	_accepted_directive_set["resolver_timeout"]._name = "resolver_timeout";
	_accepted_directive_set["return"]._name = "return";
	_accepted_directive_set["root"]._name = "root";
	_accepted_directive_set["satisfy"]._name = "satisfy";
	_accepted_directive_set["send_lowat"]._name = "send_lowat";
	_accepted_directive_set["send_timeout"]._name = "send_timeout";
	_accepted_directive_set["sendfile"]._name = "sendfile";
	_accepted_directive_set["sendfile_max_chunk"]._name = "sendfile_max_chunk";
	_accepted_directive_set["server"]._name = "server";
	_accepted_directive_set["server_name"]._name = "server_name";
	_accepted_directive_set["server_name_in_redirect"]._name = "server_name_in_redirect";
	_accepted_directive_set["server_names_hash_bucket_size"]._name = "server_names_hash_bucket_size";
	_accepted_directive_set["server_names_hash_max_size"]._name = "server_names_hash_max_size";
	_accepted_directive_set["server_tokens"]._name = "server_tokens";
	_accepted_directive_set["ssl_prefer_server_ciphers"]._name = "ssl_prefer_server_ciphers";
	_accepted_directive_set["ssl_protocols"]._name = "ssl_protocols";
	_accepted_directive_set["subrequest_output_buffer_size"]._name = "subrequest_output_buffer_size";
	_accepted_directive_set["tcp_nodelay"]._name = "tcp_nodelay";
	_accepted_directive_set["tcp_nopush"]._name = "tcp_nopush";
	_accepted_directive_set["try_files"]._name = "try_files";
	_accepted_directive_set["types"]._name = "types";
	_accepted_directive_set["types_hash_bucket_size"]._name = "types_hash_bucket_size";
	_accepted_directive_set["types_hash_max_size"]._name = "types_hash_max_size";
	_accepted_directive_set["underscores_in_headers"]._name = "underscores_in_headers";
	_accepted_directive_set["user"]._name = "user";
	_accepted_directive_set["upload_store"]._name = "upload_store";
	_accepted_directive_set["variables_hash_bucket_size"]._name = "variables_hash_bucket_size";
	_accepted_directive_set["variables_hash_max_size"]._name = "variables_hash_max_size";
	_accepted_directive_set["worker_connections"]._name = "worker_connections";
	_accepted_directive_set["worker_processes"]._name = "worker_processes";

	_accepted_directive_set["absolute_redirect"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["access_log"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["aio"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["aio_write"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["alias"]._contexts = setAcceptedDirectiveContext("location");
	_accepted_directive_set["auth_delay"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["chunked_transfer_encoding"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["client_body_buffer_size"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["client_body_in_file_only"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["client_body_in_single_buffer"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["client_body_temp_path"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["client_body_timeout"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["client_header_buffer_size"]._contexts = setAcceptedDirectiveContext("http server");
	_accepted_directive_set["client_header_timeout"]._contexts = setAcceptedDirectiveContext("http server");
	_accepted_directive_set["client_max_body_size"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["connection_pool_size"]._contexts = setAcceptedDirectiveContext("http server");
	_accepted_directive_set["default_type"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["directio"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["directio_alignment"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["disable_symlinks"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["error_log"]._contexts = setAcceptedDirectiveContext("main http mail stream server location");
	_accepted_directive_set["error_page"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["etag"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["fastcgi_pass"]._contexts = setAcceptedDirectiveContext("location");
	_accepted_directive_set["gzip"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["http"]._contexts = setAcceptedDirectiveContext("main");
	_accepted_directive_set["if_modified_since"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["ignore_invalid_headers"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["index"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["include"]._contexts = setAcceptedDirectiveContext("main http server location");
	_accepted_directive_set["internal"]._contexts = setAcceptedDirectiveContext("location");
	_accepted_directive_set["keepalive_disable"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["keepalive_requests"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["keepalive_time"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["keepalive_timeout"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["large_client_header_buffers"]._contexts = setAcceptedDirectiveContext("http server");
	_accepted_directive_set["limit_except"]._contexts = setAcceptedDirectiveContext("location");
	_accepted_directive_set["limit_rate"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["limit_rate_after"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["lingering_close"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["lingering_time"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["lingering_timeout"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["listen"]._contexts = setAcceptedDirectiveContext("server");
	_accepted_directive_set["location"]._contexts = setAcceptedDirectiveContext("server location");
	_accepted_directive_set["log_not_found"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["log_subrequest"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["max_ranges"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["merge_slashes"]._contexts = setAcceptedDirectiveContext("http server");
	_accepted_directive_set["msie_padding"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["msie_refresh"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["open_file_cache"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["open_file_cache_errors"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["open_file_cache_min_uses"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["open_file_cache_valid"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["output_buffers"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["pid"]._contexts = setAcceptedDirectiveContext("main");
	_accepted_directive_set["port_in_redirect"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["postpone_output"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["read_ahead"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["recursive_error_pages"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["request_pool_size"]._contexts = setAcceptedDirectiveContext("http server");
	_accepted_directive_set["reset_timedout_connection"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["resolver"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["resolver_timeout"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["return"]._contexts = setAcceptedDirectiveContext("server location");
	_accepted_directive_set["root"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["satisfy"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["send_lowat"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["send_timeout"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["sendfile"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["sendfile_max_chunk"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["server"]._contexts = setAcceptedDirectiveContext("http");
	_accepted_directive_set["server_name"]._contexts = setAcceptedDirectiveContext("server");
	_accepted_directive_set["server_name_in_redirect"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["server_names_hash_bucket_size"]._contexts = setAcceptedDirectiveContext("http");
	_accepted_directive_set["server_names_hash_max_size"]._contexts = setAcceptedDirectiveContext("http");
	_accepted_directive_set["server_tokens"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["ssl_prefer_server_ciphers"]._contexts = setAcceptedDirectiveContext("http server");
	_accepted_directive_set["ssl_protocols"]._contexts = setAcceptedDirectiveContext("http server");
	_accepted_directive_set["subrequest_output_buffer_size"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["tcp_nodelay"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["tcp_nopush"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["try_files"]._contexts = setAcceptedDirectiveContext("http location");
	_accepted_directive_set["types"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["types_hash_bucket_size"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["types_hash_max_size"]._contexts = setAcceptedDirectiveContext("http server location");
	_accepted_directive_set["underscores_in_headers"]._contexts = setAcceptedDirectiveContext("http server");
	_accepted_directive_set["user"]._contexts = setAcceptedDirectiveContext("main");
	_accepted_directive_set["upload_store"]._contexts = setAcceptedDirectiveContext("server location");
	_accepted_directive_set["variables_hash_bucket_size"]._contexts = setAcceptedDirectiveContext("http");
	_accepted_directive_set["variables_hash_max_size"]._contexts = setAcceptedDirectiveContext("http");
	_accepted_directive_set["worker_connections"]._contexts = setAcceptedDirectiveContext("events");
	_accepted_directive_set["worker_processes"]._contexts = setAcceptedDirectiveContext("main");

	_accepted_directive_set["absolute_redirect"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["access_log"]._syntax = SYNTAX_FILE;
	_accepted_directive_set["aio"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["aio_write"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["alias"]._syntax = SYNTAX_PATH;
	_accepted_directive_set["auth_delay"]._syntax = SYNTAX_TIME;
	_accepted_directive_set["chunked_transfer_encoding"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["client_body_buffer_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["client_body_in_file_only"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["client_body_in_single_buffer"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["client_body_temp_path"]._syntax = SYNTAX_PATH;
	_accepted_directive_set["client_body_timeout"]._syntax = SYNTAX_TIME;
	_accepted_directive_set["client_header_buffer_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["client_header_timeout"]._syntax = SYNTAX_TIME;
	_accepted_directive_set["client_max_body_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["connection_pool_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["default_type"]._syntax = SYNTAX_STRING;
	_accepted_directive_set["directio"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["directio_alignment"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["disable_symlinks"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["error_log"]._syntax = SYNTAX_FILE;
	_accepted_directive_set["error_page"]._syntax = SYNTAX_CODE_URI;
	_accepted_directive_set["etag"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["fastcgi_pass"]._syntax = SYNTAX_ADDRESS;
	_accepted_directive_set["gzip"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["http"]._syntax = SYNTAX_BLOCK;
	_accepted_directive_set["if_modified_since"]._syntax = SYNTAX_STRING;//to define
	_accepted_directive_set["ignore_invalid_headers"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["index"]._syntax = SYNTAX_FILE;
	_accepted_directive_set["include"]._syntax = SYNTAX_STRING;
	_accepted_directive_set["internal"]._syntax = 0;
	_accepted_directive_set["keepalive_disable"]._syntax = SYNTAX_STRING;//to define
	_accepted_directive_set["keepalive_requests"]._syntax = SYNTAX_NUMBER;
	_accepted_directive_set["keepalive_time"]._syntax = SYNTAX_TIME;
	_accepted_directive_set["keepalive_timeout"]._syntax = SYNTAX_TIME;
	_accepted_directive_set["large_client_header_buffers"]._syntax = SYNTAX_NUMBER_SIZE;
	_accepted_directive_set["limit_except"]._syntax = SYNTAX_BLOCK;
	_accepted_directive_set["limit_rate"]._syntax = SYNTAX_RATE;
	_accepted_directive_set["limit_rate_after"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["lingering_close"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["lingering_time"]._syntax = SYNTAX_TIME;
	_accepted_directive_set["lingering_timeout"]._syntax = SYNTAX_TIME;
	_accepted_directive_set["listen"]._syntax = SYNTAX_ADDRESS;
	_accepted_directive_set["location"]._syntax = SYNTAX_BLOCK;
	_accepted_directive_set["log_not_found"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["log_subrequest"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["max_ranges"]._syntax = SYNTAX_NUMBER;
	_accepted_directive_set["merge_slashes"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["msie_padding"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["msie_refresh"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["open_file_cache"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["open_file_cache_errors"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["open_file_cache_min_uses"]._syntax = SYNTAX_NUMBER;
	_accepted_directive_set["open_file_cache_valid"]._syntax = SYNTAX_TIME;
	_accepted_directive_set["output_buffers"]._syntax = SYNTAX_NUMBER_SIZE;
	_accepted_directive_set["pid"]._syntax = SYNTAX_STRING;
	_accepted_directive_set["port_in_redirect"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["postpone_output"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["read_ahead"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["recursive_error_pages"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["request_pool_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["reset_timedout_connection"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["resolver"]._syntax = SYNTAX_ADDRESS;
	_accepted_directive_set["resolver_timeout"]._syntax = SYNTAX_TIME;
	_accepted_directive_set["return"]._syntax = SYNTAX_CODE_URI;
	_accepted_directive_set["root"]._syntax = SYNTAX_PATH;
	_accepted_directive_set["satisfy"]._syntax = SYNTAX_ALL_ANY;
	_accepted_directive_set["send_lowat"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["send_timeout"]._syntax = SYNTAX_TIME;
	_accepted_directive_set["sendfile"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["sendfile_max_chunk"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["server"]._syntax = SYNTAX_BLOCK;
	_accepted_directive_set["server_name"]._syntax = SYNTAX_NAME;
	_accepted_directive_set["server_name_in_redirect"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["server_names_hash_bucket_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["server_names_hash_max_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["server_tokens"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["ssl_prefer_server_ciphers"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["ssl_protocols"]._syntax = SYNTAX_STRING;
	_accepted_directive_set["subrequest_output_buffer_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["tcp_nodelay"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["tcp_nopush"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["try_files"]._syntax = SYNTAX_STRING;//to define
	_accepted_directive_set["types"]._syntax = SYNTAX_BLOCK;
	_accepted_directive_set["types_hash_bucket_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["types_hash_max_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["underscores_in_headers"]._syntax = SYNTAX_ON_OFF;
	_accepted_directive_set["user"]._syntax = SYNTAX_STRING;//to define
	_accepted_directive_set["upload_store"]._syntax = SYNTAX_PATH;
	_accepted_directive_set["variables_hash_bucket_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["variables_hash_max_size"]._syntax = SYNTAX_SIZE;
	_accepted_directive_set["worker_connections"]._syntax = SYNTAX_NUMBER;
	_accepted_directive_set["worker_processes"]._syntax = SYNTAX_NUMBER;
}
