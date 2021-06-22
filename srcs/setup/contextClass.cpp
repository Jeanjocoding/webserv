/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   contextClass.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 15:24:19 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/22 15:47:25 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "contextClass.hpp"

contextClass::contextClass()
{
}

contextClass::contextClass(std::string name, std::string buff): _name(name)
{
	setAcceptedDirectives();
	setDirectives();
	setBlocks();
	
	_block_content = getBlock(_name, buff).second;
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
		std::cerr << "gibberish spotted in context " << _name << std::endl;
		exit(EXIT_FAILURE);
	}
}

contextClass::contextClass(contextClass const& copy)
{
	_directive_set = copy._directive_set;
	_block_set = copy._block_set;
	_block_content = copy._block_content;
	_blocks = copy._blocks;
}

contextClass& contextClass::operator = (contextClass const& copy)
{
	_directive_set = copy._directive_set;
	_block_set = copy._block_set;
	_block_content = copy._block_content;
	_blocks = copy._blocks;
	return *this;
}

contextClass::~contextClass()
{
	for (auto it = _blocks.begin(); it != _blocks.end(); it++)
		delete *it;
}

void contextClass::setBlocks(void)
{
    if (_name == "http")
	{
       _block_set = {
			"server",
		};
	}
	else if (_name == "events")
	{
		_block_set = {};//empty for now
	}
    else if (_name == "server")
	{
       _block_set = {
			"location"
		};
	}
    else if (_name == "location")
	{
       _block_set = {};
	}
    else
	{
		_block_set = {
			"http",
			"events"//might delete
		};
	}
}

void contextClass::setDirectives(void)
{
    if (_name == "http")
	{
       _directive_set = {
			"access_log",
			"error_log",
			"index"
		};
	}
	else if (_name == "events")
	{
		_directive_set = {};
	}
    else if (_name == "server")
	{
       _directive_set = {
			"listen",
			"server_name",
			"access_log",
			"error_log",
			"root"
		};
	}
    else if (_name == "location")
	{
       _directive_set = {
			"fastcgi_pass",
			"access_log",
			"error_log",
			"root"
		};
	}
    else
	{
		_directive_set = {
			"access_log",
			"error_log"
		};
	}
}


static bool check_before(std::string const& buff, std::size_t i)
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

static bool check_after(std::string const& buff, std::size_t& i)
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
	auto							it = buff.begin();
	auto							ite = buff.begin();
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

std::pair<bool, std::string>	contextClass::getSingleDirective(std::string const& directive_name, std::string const& buff) const
{
	std::pair<bool, std::string>	res;
	std::size_t						i;
	auto							it = buff.begin();
	auto							ite = buff.begin();

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
	for (auto it = _block_set.begin(); it != _block_set.end(); it++)
	{
		for (auto check = getBlock(*it, _block_content); check.first; check = getBlock(*it, _block_content))
		{
			_blocks.push_back(new contextClass(*it, check.second));
			_block_content.erase(_block_content.find(check.second), check.second.size());
		}
	}
}

void	contextClass::getDirectivesInContext(void)
{
	for (auto it = _directive_set.begin(); it != _directive_set.end(); it++)
	{
		std::pair<bool, std::string>check = getSingleDirective(*it, _block_content);//get directive and erase it from buffer
		if (check.first)
		{
			_block_content.erase(_block_content.find(check.second), check.second.size() + 1);
			check.second.erase(check.second.find(*it), (*it).size());
			while (check.second[0] == ' ')
				check.second.erase(0, 1);
			while (check.second.size() && check.second[check.second.size() - 1] == ' ')
				check.second.erase(check.second.size() - 1, 1);
			if (check.second.empty())
			{
				std::cerr << "empty directive " << *it << std::endl;
				exit(EXIT_FAILURE);
			}
			_directives[*it] = check.second;
		}
		if (getSingleDirective(*it, _block_content).first)//check for same directive
		{
			std::cerr << "error configuration file : directive " << *it << " found twice in same context" << std::endl;//switch to define
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
		auto it = _block_content.begin();
		while (it != _block_content.end() && *it != ' ' && *it != ';' && *it != '{' && *it != '}')
			it++;
		std::string temp(_block_content.begin(), it);
		if (temp.empty())
			return ;
		auto directive = _accepted_directive_set.find(temp);
		if (directive != _accepted_directive_set.end() && (directive->second.isInContext(_name)))
		{
			std::pair<bool, std::string> check;
			if (directive->second._syntax == BLOCK)
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

void contextClass::setAcceptedDirectives(void)
{
	_accepted_directive_set["absolute_redirect"]._name = "absolute_redirect";
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
	_accepted_directive_set["error_page"]._name = "error_page";
	_accepted_directive_set["etag"]._name = "etag";
	_accepted_directive_set["gzip"]._name = "gzip";
	_accepted_directive_set["http"]._name = "http";
	_accepted_directive_set["if_modified_since"]._name = "if_modified_since";
	_accepted_directive_set["ignore_invalid_headers"]._name = "ignore_invalid_headers";
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
	_accepted_directive_set["variables_hash_bucket_size"]._name = "variables_hash_bucket_size";
	_accepted_directive_set["variables_hash_max_size"]._name = "variables_hash_max_size";
	_accepted_directive_set["worker_connections"]._name = "worker_connections";
	_accepted_directive_set["worker_processes"]._name = "worker_processes";

	_accepted_directive_set["absolute_redirect"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["aio"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["aio_write"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["alias"]._contexts = {"location"};
	_accepted_directive_set["auth_delay"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["chunked_transfer_encoding"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["client_body_buffer_size"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["client_body_in_file_only"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["client_body_in_single_buffer"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["client_body_temp_path"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["client_body_timeout"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["client_header_buffer_size"]._contexts = {"http", "server"};
	_accepted_directive_set["client_header_timeout"]._contexts = {"http", "server"};
	_accepted_directive_set["client_max_body_size"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["connection_pool_size"]._contexts = {"http", "server"};
	_accepted_directive_set["default_type"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["directio"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["directio_alignment"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["disable_symlinks"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["error_page"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["etag"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["gzip"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["http"]._contexts = {"main"};
	_accepted_directive_set["if_modified_since"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["ignore_invalid_headers"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["include"]._contexts = {"main", "http", "server", "location"};
	_accepted_directive_set["internal"]._contexts = {"location"};
	_accepted_directive_set["keepalive_disable"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["keepalive_requests"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["keepalive_time"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["keepalive_timeout"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["large_client_header_buffers"]._contexts = {"http", "server"};
	_accepted_directive_set["limit_except"]._contexts = {"location"};
	_accepted_directive_set["limit_rate"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["limit_rate_after"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["lingering_close"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["lingering_time"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["lingering_timeout"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["listen"]._contexts = {"server"};
	_accepted_directive_set["location"]._contexts = {"server", "location"};
	_accepted_directive_set["log_not_found"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["log_subrequest"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["max_ranges"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["merge_slashes"]._contexts = {"http", "server"};
	_accepted_directive_set["msie_padding"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["msie_refresh"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["open_file_cache"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["open_file_cache_errors"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["open_file_cache_min_uses"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["open_file_cache_valid"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["output_buffers"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["pid"]._contexts = {"main"};
	_accepted_directive_set["port_in_redirect"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["postpone_output"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["read_ahead"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["recursive_error_pages"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["request_pool_size"]._contexts = {"http", "server"};
	_accepted_directive_set["reset_timedout_connection"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["resolver"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["resolver_timeout"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["root"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["satisfy"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["send_lowat"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["send_timeout"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["sendfile"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["sendfile_max_chunk"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["server"]._contexts = {"http"};
	_accepted_directive_set["server_name"]._contexts = {"server"};
	_accepted_directive_set["server_name_in_redirect"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["server_names_hash_bucket_size"]._contexts = {"http"};
	_accepted_directive_set["server_names_hash_max_size"]._contexts = {"http"};
	_accepted_directive_set["server_tokens"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["ssl_prefer_server_ciphers"]._contexts = {"http", "server"};
	_accepted_directive_set["ssl_protocols"]._contexts = {"http", "server"};
	_accepted_directive_set["subrequest_output_buffer_size"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["tcp_nodelay"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["tcp_nopush"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["try_files"]._contexts = {"http", "location"};
	_accepted_directive_set["types"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["types_hash_bucket_size"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["types_hash_max_size"]._contexts = {"http", "server", "location"};
	_accepted_directive_set["underscores_in_headers"]._contexts = {"http", "server"};
	_accepted_directive_set["user"]._contexts = {"main"};
	_accepted_directive_set["variables_hash_bucket_size"]._contexts = {"http"};
	_accepted_directive_set["variables_hash_max_size"]._contexts = {"http"};
	_accepted_directive_set["worker_connections"]._contexts = {"events"};
	_accepted_directive_set["worker_processes"]._contexts = {"main"};

	_accepted_directive_set["absolute_redirect"]._syntax = ON_OFF;
	_accepted_directive_set["aio"]._syntax = ON_OFF;
	_accepted_directive_set["aio_write"]._syntax = ON_OFF;
	_accepted_directive_set["alias"]._syntax = STRING;
	_accepted_directive_set["auth_delay"]._syntax = INT;
	_accepted_directive_set["chunked_transfer_encoding"]._syntax = ON_OFF;
	_accepted_directive_set["client_body_buffer_size"]._syntax = INT;
	_accepted_directive_set["client_body_in_file_only"]._syntax = ON_OFF;
	_accepted_directive_set["client_body_in_single_buffer"]._syntax = ON_OFF;
	_accepted_directive_set["client_body_temp_path"]._syntax = STRING;
	_accepted_directive_set["client_body_timeout"]._syntax = INT;
	_accepted_directive_set["client_header_buffer_size"]._syntax = INT;
	_accepted_directive_set["client_header_timeout"]._syntax = INT;
	_accepted_directive_set["client_max_body_size"]._syntax = INT;
	_accepted_directive_set["connection_pool_size"]._syntax = INT;
	_accepted_directive_set["default_type"]._syntax = STRING;
	_accepted_directive_set["directio"]._syntax = INT;
	_accepted_directive_set["directio_alignment"]._syntax = INT;
	_accepted_directive_set["disable_symlinks"]._syntax = ON_OFF;
	_accepted_directive_set["error_page"]._syntax = STRING;
	_accepted_directive_set["etag"]._syntax = ON_OFF;
	_accepted_directive_set["gzip"]._syntax = ON_OFF;
	_accepted_directive_set["http"]._syntax = BLOCK;
	_accepted_directive_set["if_modified_since"]._syntax = STRING;
	_accepted_directive_set["ignore_invalid_headers"]._syntax = ON_OFF;
	_accepted_directive_set["include"]._syntax = STRING;
	_accepted_directive_set["internal"]._syntax = 0;
	_accepted_directive_set["keepalive_disable"]._syntax = STRING;
	_accepted_directive_set["keepalive_requests"]._syntax = INT;
	_accepted_directive_set["keepalive_time"]._syntax = INT;
	_accepted_directive_set["keepalive_timeout"]._syntax = INT;
	_accepted_directive_set["large_client_header_buffers"]._syntax = INT;
	_accepted_directive_set["limit_except"]._syntax = BLOCK;
	_accepted_directive_set["limit_rate"]._syntax = INT;
	_accepted_directive_set["limit_rate_after"]._syntax = INT;
	_accepted_directive_set["lingering_close"]._syntax = ON_OFF;
	_accepted_directive_set["lingering_time"]._syntax = INT;
	_accepted_directive_set["lingering_timeout"]._syntax = INT;
	_accepted_directive_set["listen"]._syntax = STRING;
	_accepted_directive_set["location"]._syntax = BLOCK;
	_accepted_directive_set["log_not_found"]._syntax = ON_OFF;
	_accepted_directive_set["log_subrequest"]._syntax = ON_OFF;
	_accepted_directive_set["max_ranges"]._syntax = INT;
	_accepted_directive_set["merge_slashes"]._syntax = ON_OFF;
	_accepted_directive_set["msie_padding"]._syntax = ON_OFF;
	_accepted_directive_set["msie_refresh"]._syntax = ON_OFF;
	_accepted_directive_set["open_file_cache"]._syntax = ON_OFF;
	_accepted_directive_set["open_file_cache_errors"]._syntax = ON_OFF;
	_accepted_directive_set["open_file_cache_min_uses"]._syntax = INT;
	_accepted_directive_set["open_file_cache_valid"]._syntax = INT;
	_accepted_directive_set["output_buffers"]._syntax = INT;
	_accepted_directive_set["pid"]._syntax = STRING;
	_accepted_directive_set["port_in_redirect"]._syntax = ON_OFF;
	_accepted_directive_set["postpone_output"]._syntax = INT;
	_accepted_directive_set["read_ahead"]._syntax = INT;
	_accepted_directive_set["recursive_error_pages"]._syntax = ON_OFF;
	_accepted_directive_set["request_pool_size"]._syntax = INT;
	_accepted_directive_set["reset_timedout_connection"]._syntax = ON_OFF;
	_accepted_directive_set["resolver"]._syntax = STRING;
	_accepted_directive_set["resolver_timeout"]._syntax = INT;
	_accepted_directive_set["root"]._syntax = STRING;
	_accepted_directive_set["satisfy"]._syntax = ON_OFF;
	_accepted_directive_set["send_lowat"]._syntax = INT;
	_accepted_directive_set["send_timeout"]._syntax = INT;
	_accepted_directive_set["sendfile"]._syntax = ON_OFF;
	_accepted_directive_set["sendfile_max_chunk"]._syntax = INT;
	_accepted_directive_set["server"]._syntax = BLOCK;
	_accepted_directive_set["server_name"]._syntax = STRING;
	_accepted_directive_set["server_name_in_redirect"]._syntax = ON_OFF;
	_accepted_directive_set["server_names_hash_bucket_size"]._syntax = INT;
	_accepted_directive_set["server_names_hash_max_size"]._syntax = INT;
	_accepted_directive_set["server_tokens"]._syntax = ON_OFF;
	_accepted_directive_set["ssl_prefer_server_ciphers"]._syntax = ON_OFF;
	_accepted_directive_set["ssl_protocols"]._syntax = STRING;
	_accepted_directive_set["subrequest_output_buffer_size"]._syntax = INT;
	_accepted_directive_set["tcp_nodelay"]._syntax = ON_OFF;
	_accepted_directive_set["tcp_nopush"]._syntax = ON_OFF;
	_accepted_directive_set["try_files"]._syntax = STRING;
	_accepted_directive_set["types"]._syntax = BLOCK;
	_accepted_directive_set["types_hash_bucket_size"]._syntax = INT;
	_accepted_directive_set["types_hash_max_size"]._syntax = INT;
	_accepted_directive_set["underscores_in_headers"]._syntax = ON_OFF;
	_accepted_directive_set["user"]._syntax = STRING;
	_accepted_directive_set["variables_hash_bucket_size"]._syntax = INT;
	_accepted_directive_set["variables_hash_max_size"]._syntax = INT;
	_accepted_directive_set["worker_connections"]._syntax = INT;
	_accepted_directive_set["worker_processes"]._syntax = INT;
}
