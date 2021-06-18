/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   contextClass.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/12 15:24:19 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/18 13:31:49 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "contextClass.hpp"

contextClass::contextClass()
{
}

contextClass::contextClass(std::string name, std::string buff): _name(name)
{
	setDirectives();
	setBlocks();
	
	_block_content = getBlock(_name, buff).second;
	std::cout << "context " << _name << " : " << _block_content << std::endl;

	getBlocksInContext(_block_set);
	getDirectivesInContext(_directive_set);
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

static bool check_before(std::string buff, std::size_t i)
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

static bool check_after(std::string buff, std::size_t& i)
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
		it += i;
		i += block_name.size();
		if (check_before(buff, i - block_name.size()) && check_after(buff, i))//if word is block
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

void	contextClass::getBlocksInContext(std::vector<std::string> const& set)
{
	for (auto it = set.begin(); it != set.end(); it++)
	{
		for (auto check = getBlock(*it, _block_content); check.first; check = getBlock(*it, _block_content))
		{
			_blocks.push_back(new contextClass(*it, check.second));
			_block_content.erase(_block_content.find(check.second), check.second.size());
		}
	}
}

void	contextClass::getDirectivesInContext(std::vector<std::string> const& directive_set)
{
	for (auto it = _directive_set.begin(); it != directive_set.end(); it++)
	{
		std::pair<bool, std::string>check = getSingleDirective(*it, _block_content);//get directive and erase it from buffer
		if (check.first)
		{
			_directives[*it] = check.second;
			std::cout << "directive found : " << _directives[*it] << "in context " << _name << std::endl;
			_block_content.erase(_block_content.find(_directives[*it]), _directives[*it].size());
		}
		if (getSingleDirective(*it, _block_content).first)//check for same directive
		{
			std::cerr << "error configuration file : twice the same directive in same context" << std::endl;//switch to define
			exit(EXIT_FAILURE);
		}
	}
}
