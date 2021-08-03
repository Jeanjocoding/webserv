/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/02 16:27:33 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/02 18:38:50 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <sstream>
#include <streambuf>
#include "HttpResponse.hpp"

HttpResponse::HttpResponse(void): HttpMessage()
{
	//TODO
}

HttpResponse::HttpResponse(HttpResponse const& to_copy) : HttpMessage(to_copy)
{
	//TODO
}

HttpResponse::~HttpResponse(void)
{
	//TODO
}

HttpResponse&	HttpResponse::operator=(HttpResponse const& to_copy)
{
	HttpMessage::operator=(to_copy)	;
	//TODO
	return (*this);
}

std::string		HttpResponse::toString(void) const
{
	std::string res = headerToString();
	res += bodyToString();
	return res;
}

std::string		HttpResponse::headerToString(void) const
{
	std::string res;
	//TODO
	return res;
}

std::string		HttpResponse::bodyToString(void) const
{
	std::string res;
	//TODO
	return res;
}

void	HttpResponse::setStatusCode(std::string const& status_str)
{
	std::stringstream ss(status_str);
	ss << status_str;
	ss >> _status_code;
}

void	HttpResponse::setStatusCode(unsigned short status_nbr)
{
	_status_code = status_nbr;
}

void	HttpResponse::setHeader(unsigned short code)
{
	if (code != 0)
		_status_code = code;
	//TODO
}

void	HttpResponse::setBody(std::string const& body_path)
{
	std::ifstream	file;

	file.open(body_path.c_str());
	if (!file.is_open())
		return ; //TODO error handling
	_body = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());//TODO input file in string
}
