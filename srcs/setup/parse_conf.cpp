/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_conf.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 19:42:54 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/04 21:09:06 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <string>

static bool	check_brackets(std::string buff)
{
	unsigned int	open_brackets;
	unsigned int	closing_brackets;

	open_brackets = 0;
	closing_brackets = 0;
	
	for (std::size_t it = buff.find("{"); it != std::string::npos; it = buff.find("{", it + 1))
		open_brackets++;
	for (std::size_t it = buff.find("}"); it != std::string::npos ; it = buff.find("}", it + 1))
		closing_brackets++;
	if (open_brackets == closing_brackets)
		return true;
	std::cerr << "invalid configuration file: brackets mismatch\n";
	return false;
}

static bool	check_quotes(std::string buff)
{
	unsigned int	quote;

	quote = 0;
	
	for (std::size_t it = buff.find("\""); it != std::string::npos; it = buff.find("\"", it + 1))
		quote++;
	if (quote % 2 == 0)
		return true;
	std::cerr << "invalid configuration file: quote mismatch\n";
	return false;
}

std::string	read_file(std::string filename)
{
	std::ifstream	file;
	std::string		buff;
	std::string 	line;
	std::size_t		i;

	file.open(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "failed to open " << filename << std::endl;
		return buff;
	}
	while (getline(file, line))
	{
		i = line.find("#");
		if (i != std::string::npos)//erase commented parts
			line.erase(i, line.size() - i);
		buff += line;
	}
	file.close();
	return buff;
}

bool	parse_conf_file(std::string& buff)
{
	const std::string s = " ";
	const std::string t = "\t";

	if (buff.empty())
		return false;
	std::string::size_type n = 0;
	while ((n = buff.find(t, n)) != std::string::npos)
	{
	    buff.replace(n, t.size(), s);//convert tabs to single space
	    n += s.size();
	}
	if (check_brackets(buff) && check_quotes(buff))// && check_http(buff))
		return true;
	return false;
}
