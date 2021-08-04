/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/02 16:27:33 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/04 19:30:10 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <sstream>
#include <streambuf>
#include "HttpResponse.hpp"

HttpResponse::HttpResponse(void): HttpMessage()
{
	//TODO
	_contentLength = 0;
}

HttpResponse::HttpResponse(HttpResponse const& to_copy) : HttpMessage(to_copy)
{
	//TODO
}

HttpResponse::~HttpResponse(void)
{
	//TODO
}

HttpResponse::HttpResponse(unsigned short status_code, std::string body_path)
{
	if(!setBody(body_path))
		setHeader(500);
	else
		setHeader(status_code);
}

HttpResponse&	HttpResponse::operator=(HttpResponse const& to_copy)
{
	HttpMessage::operator=(to_copy)	;
	//TODO
	return (*this);
}

std::string		HttpResponse::toString(void) const
{
	std::string res = _header;
	res.append(_body);
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
	_status_code = code;
	setHeader();
}

void	HttpResponse::setHeader(void)
{
	setDateTime();
	setLength();
	setConnectionStatus();//TODO
	_header = "HTTP/1.1 ";
	_header.append(_status_code);
	_header.append(" ");
	_header.append(_status_message);
	_header.append("\r\n");
	_header.append("Date: ");
	_header.append(_date);
	_header.append("\r\n");
	_header.append("Server: ");
	_header.append(_server_name);//TODO get server_name from server
	_header.append("\r\n");
	_header.append("Content-Length: ");
	_header.append(_content_length);
	_header.append("\r\n");
	_header.append("Content-Type: ");
	_header.append("text/html; charset=iso-8859-1");
	_header.append("\r\n");
	_header.append("Connection: ");
	_header.append(_connection);
	_header.append("\r\n");
}

bool	HttpResponse::setBody(std::string const& body_path)
{
	std::ifstream	file;

	file.open(body_path.c_str());
	if (!file.is_open())
	{
		_body = DEFAULT_ERROR_BODY;
		return false; //TODO error handling
	}
	_body = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	return true;
}

void	HttpResponse::setBody(std::string::iterator start, std::string::iterator end)
{
	_body = std::string(start, end);
	_content_length = _body.size();
}

void	HttpResponse::setDateTime(void)
{
	time_t t;
	struct tm* tt;
	time (&t);
	tt = localtime(&t);
	_date = asctime(tt);
}

void	HttpResponse::setLength(void)
{
	_content_length = _body.size();
}

void	HttpResponse::setLength(unsigned long length)
{
	_content_length = length;
}

void	HttpResponse::setConnectionStatus()
{
	_connection = "CLOSED";//TODO
}
