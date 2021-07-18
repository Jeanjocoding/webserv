/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directiveClass.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/21 17:42:15 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/18 12:16:41 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>
#include "directiveClass.hpp"

directiveClass::directiveClass()
{
}

directiveClass::directiveClass(directiveClass const& copy) : _name(copy._name), _contexts(copy._contexts), _syntax(copy._syntax)
{
}

directiveClass& directiveClass::operator = (directiveClass const& copy)
{
	_name = copy._name;
	_contexts = copy._contexts;
	_syntax = copy._syntax;
	return *this;
}

directiveClass::~directiveClass()
{
}

bool	directiveClass::isInContext(std::string const& context_name) const
{
	for (std::vector<std::string>::const_iterator i = _contexts.begin(); i != _contexts.end(); i++)
	{
		if (context_name == *i)
			return true;
	}
	return false;
}

static bool	onOffParse(std::string const& arg)
{
	for (std::size_t i = 0; arg[i]; i++)
	{
		if (arg[i] == ' ')
			continue;
		else if (arg[i] == 'o')
		{
			if (i + 1 < arg.size() && arg[i + 1] == 'n')
			{
				i += 2;
				while (arg[i] && arg[i] == ' ')
					i++;
				if (arg[i] != ';')
					return false;
				else
					return true;
			}
			else if (i + 2 < arg.size() && arg[i + 1] == 'f' && arg[i + 2] == 'f')
			{
				i += 3;
				while (arg[i] && arg[i] == ' ')
					i++;
				if (arg[i] != ';')
					return false;
				else
					return true;
			}
		}
		else
			return false;
	}
	return false;
}

static bool	stringParse(void)
{
	return true;
}

static bool	fileParse(std::string const& arg)
{
	std::string::const_iterator	it = arg.begin();
	std::string::const_iterator	ite = arg.begin();
	int							i = 0;
	std::string					tmp;
	std::ofstream				file;

	while (it != arg.end() && *it == ' ')
	{
		it++;
		i++;
	}
	if (it == arg.end())
		return false;
	if (arg.find(' ', it - arg.begin()) < arg.find(';', it - arg.begin()))
		ite += arg.find(' ', it - arg.begin());
	else if (arg.find(' ', it - arg.begin()) == std::string::npos)
		ite = arg.end();
	else
		ite = arg.begin() + arg.find(';', it - arg.begin());
	tmp = std::string(it ,ite);
	file.open(tmp.c_str());
	if (!file.is_open())
		return false;
	file.close();
	return true;
}

static bool	sizeParse(std::string& arg)
{
	std::size_t	i = 0;

	while (arg[i] && arg[i] == ' ')
		i++;
	while (arg[i] && arg[i] >= '0' && arg[i] <= '9')
		i++;
	if (arg[i])
	{
		if (arg[i] != 'K' && arg[i] != 'M' && arg[i] != 'k' && arg[i] != 'm')
			return false;
		else
			i++;
	}
	while (arg[i] && arg[i] == ' ')
		i++;
	if (arg[i] && arg[i] != ';')
		return false;	
	return true;
}

static bool	numberParse(std::string& arg)
{
	std::size_t	i = 0;

	while (arg[i] && arg[i] == ' ')
		i++;
	while (arg[i] && arg[i] >= '0' && arg[i] <= '9')
		i++;
	while (arg[i] && arg[i] == ' ')
		i++;
	if (arg[i] && arg[i] != ';')
		return false;	
	return true;
}

static bool	numberSizeParse(std::string& arg)
{
	std::size_t	i = 0;

	while (arg[i] && arg[i] == ' ')
		i++;
	while (arg[i] && arg[i] >= '0' && arg[i] <= '9')
		i++;
	while (arg[i] && arg[i] == ' ')
		i++;
	while (arg[i] && arg[i] >= '0' && arg[i] <= '9')
		i++;
	if (arg[i])
	{
		if (arg[i] != 'K' && arg[i] != 'M' && arg[i] != 'k' && arg[i] != 'm')
			return false;
		else
			i++;
	}
	while (arg[i] && arg[i] == ' ')
		i++;
	if (arg[i] && arg[i] != ';')
		return false;	
	return true;
}

static bool	timeParse(std::string& arg)
{
	std::size_t	i = 0;

	while (arg[i] && arg[i] == ' ')
		i++;
	while (arg[i] && arg[i] >= '0' && arg[i] <= '9')
		i++;
	if (arg[i])
	{
		if (arg[i] == 's' || arg[i] == 'y' || arg[i] == 'M' || arg[i] == 'w'
			|| arg[i] == 'd' || arg[i] == 'h' || arg[i] == 'm'
			|| (arg[i] == 'm' && arg[i + 1] && arg[i + 1] == 's') || arg[i] == 's')
			i++;
		else
			return false;
	}
	while (arg[i] && arg[i] == ' ')
		i++;
	if (!arg[i])
		return true;	
	return false;
}

static bool	pathParse(std::string& arg)
{
	std::size_t	i = 0;

	while (arg[i] && arg[i] == ' ')
		i++;
	if (!arg[i] || (arg[i] != '/' && arg[i] != '.'))
		return false;
	while (arg[i] && arg[i] != ' ')
		i++;
	while (arg[i] && arg[i] == ' ')
		i++;
	if (arg[i])
		return false;
	return true;
}

static bool	allAnyParse(std::string& arg)
{
	for (std::size_t i = 0; arg[i]; i++)
	{
		if (arg[i] == ' ')
			continue;
		else if (arg[i] == 'a')
		{
			if (i + 2 < arg.size() && arg[i + 1] == 'n' && arg[i + 2] == 'y')
			{
				i += 2;
				while (arg[i] && arg[i] == ' ')
					i++;
				if (arg[i] && arg[i] != ';')
					return false;
				else
					return true;
			}
			else if (i + 2 < arg.size() && arg[i + 1] == 'l' && arg[i + 2] == 'l')
			{
				i += 3;
				while (arg[i] && arg[i] == ' ')
					i++;
				if (arg[i] && arg[i] != ';')
					return false;
				else
					return true;
			}
		}
		else
			return false;
	}
	return false;
}

static bool	nameParse(std::string& arg)
{
	std::size_t	i = 0;

	while (arg[i] && arg[i] == ' ')
		i++;
	while (arg[i] && ((arg[i] >= '0' && arg[i] <= '9')
		|| (arg[i] >= 'a' && arg[i] <= 'z') || (arg[i] >= 'A' && arg[i] <= 'Z')))
		i++;
	while (arg[i] && arg[i] == ' ')
		i++;
	if (arg[i] && arg[i] != ';')
		return false;	
	return true;
}

static bool parseHost(std::string const& arg, std::size_t& i)
{
	while (arg[i] && arg[i] == ' ')
		i++;
	if (arg[i] && arg[i] == '*')
	{
		i++;
		if (arg[i] && arg[i] == ':')
		{
			i++;
			return true;
		}
		else
			return false;
	}
	for (short j = 0; j < 3; j++)
	{
		for (std::size_t it = 0; i < arg.size() && it < 3; it++)
		{
			if (arg[i])
			{
				if (arg[i] >= '0' && arg[i] <= '9')
				{
					i++;
					continue;
				}
				else if (arg[i] == '.')
					break;
				else
					return false;
			}
			else
				return false;
		}
		if (!arg[i] || arg[i] != '.')
			return false;
		i++;
	}
	for (std::size_t it = 0; i < arg.size() && it < 3; it++)
	{
		if (arg[i])
		{
			if (arg[i] >= '0' && arg[i] <= '9')
			{
				i++;
				continue;
			}
			else if (arg[i] == ':')
			{
				i++;
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	if (arg[i] || arg[i] != ':')
		return false;
	i++;
	return true;
}

static bool parsePort(std::string const& arg, std::size_t i)
{
	for (std::size_t j = 0; arg[i] && arg[i] >= '0' && arg[i] <= '9' && j < 4; j++)
		i++;
	while (arg[i] && arg[i] == ' ')
		i++;
	if (!arg[i])
		return true;
	else
		return false;
}

static bool	addressParse(std::string& arg)
{
	std::size_t	i = 0;

	return true;
	if (parseHost(arg, i) && arg[i])
		return parsePort(arg, i);
	return false;
}

static bool	rateParse(std::string& arg)
{
	std::size_t	i = 0;

	while (arg[i] && arg[i] == ' ')
		i++;
	while (arg[i] && arg[i] >= '0' && arg[i] <= '9')
		i++;
	while (arg[i] && arg[i] == ' ')
		i++;
	if (arg[i] && arg[i] != ';')
		return false;	
	return true;
}

static bool	codeUriParse(std::string arg)
{
	std::size_t	i = 0;

	while (arg[i] && arg[i] == ' ')
		i++;
	for (short it = 0; arg[i] && arg[i] >= '0' && arg[i] <= '9' && it < 3; it++)
		i++;
	while (arg[i] && arg[i] == ' ')
		i++;
	arg.erase(0, i);
	if (arg.empty())
		return true;
	return (pathParse(arg));
}

static bool methodParse(std::string arg)
{
	std::istringstream iss(arg);
	std::vector<std::string> vect;
	std::string set[] = {"GET", "POST", "DELETE", "HEAD", "PUT"};
	std::string tmp;
	for (unsigned int i = 0; iss >> tmp; i++)
		vect.push_back(tmp);
	if (vect.empty())
		return false;
	for (std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); it++)
	{
		bool check = false;
		for (int i = 0; i < 5 ; i++)
		{
			if (set[i] == *it)
			{
				check = true;
				break;
			}
		}
		if (check == false)
			return false;
	}
	for (std::vector<std::string>::iterator it = vect.begin(); it != --vect.end(); it++)
	{
		for (std::vector<std::string>::iterator i = it + 1; i != vect.end(); i++)
		{
			if (*i == *it)
				return false;
		}
	}
	return true;
}

bool	directiveClass::parse(std::string arg)
{
	if (arg.empty())
		return false;
	switch (_syntax)
	{
		case SYNTAX_ON_OFF:
			return (onOffParse(arg));
		case SYNTAX_STRING:
			return (stringParse());
		case SYNTAX_FILE:
			return (fileParse(arg));
		case SYNTAX_PATH:
			return (pathParse(arg));
		case SYNTAX_SIZE:
			return (sizeParse(arg));
		case SYNTAX_NUMBER:
			return (numberParse(arg));
		case SYNTAX_NUMBER_SIZE:
			return (numberSizeParse(arg));
		case SYNTAX_TIME:
			return (timeParse(arg));
		case SYNTAX_ALL_ANY:
			return (allAnyParse(arg));
		case SYNTAX_NAME:
			return (nameParse(arg));
		case SYNTAX_ADDRESS:
			return (addressParse(arg));
		case SYNTAX_RATE:
			return (rateParse(arg));
		case SYNTAX_CODE_URI:
			return (codeUriParse(arg));
		case SYNTAX_METHOD:
			return (methodParse(arg));
		default:
			return false;
	}
}
