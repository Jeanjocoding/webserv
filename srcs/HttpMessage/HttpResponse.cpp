/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/02 16:27:33 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/27 11:50:58 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <sstream>
#include <streambuf>
#include "HttpResponse.hpp"

HttpResponse::HttpResponse(void): HttpMessage()
{
	//TODO
//	_contentLength = 0;
}

HttpResponse::HttpResponse(HttpResponse const& copy) : HttpMessage(copy)
{
	_header = copy._header;
	//TODO
}

HttpResponse::~HttpResponse(void)
{
	//TODO
}

HttpResponse::HttpResponse(unsigned short status_code, std::string body_path)
{
	if (status_code == 301 || status_code == 302)
	{
		setHeader(status_code);
		_header.append("Location: ");
		_header.append(body_path);
		_header.append("\r\n");
	}
	else if(!setBody(body_path))
		setHeader(500);
	else
		setHeader(status_code);
}

HttpResponse&	HttpResponse::operator = (HttpResponse const& copy)
{
	HttpMessage::operator = (copy);
	_header = copy._header;
	//TODO
	return (*this);
}

std::string		HttpResponse::toString(void) const
{
	std::string res = _header;
	res.append("\r\n");
	res.append(_content, _currentContentLength);
	return res;
}

void	HttpResponse::setStatusCode(std::string const& status_str)
{
	_status_code = status_str;
	setStatusMessage();
}

void	HttpResponse::setStatusCode(unsigned short status_nbr)
{
	std::stringstream ss;
	ss << status_nbr;
	ss >> _status_code;
	setStatusMessage();
}

void	HttpResponse::setStatusMessage(void)
{
	if (_status_code == "200")
		_status_message = "OK";
	if (_status_code == "204")
		_status_message = "No Content";
	else if (_status_code == "400")
		_status_message = "Invalid Request";
	else if (_status_code == "403")
		_status_message = "Forbidden";
	else if (_status_code == "405")
		_status_message = "Method Not Allowed";
	else if (_status_code == "408")
		_status_message = "Request Timeout";
	else if (_status_code == "404")
		_status_message = "Not Found";
	else if (_status_code == "500")
		_status_message = "Internal Server Error";
	else if (_status_code == "501")
		_status_message = "Not Implemented";
	else if (_status_code == "502")
		_status_message = "Bad Gateway";
	else if (_status_code == "503")
		_status_message = "Service Unavailable";
	else if (_status_code == "504")
		_status_message = "Gateway Timeout";
	else if (_status_code == "505")
		_status_message = "HTTP Version Not Supported";
	else 
		_status_message = "";
}

void	HttpResponse::setHeader(unsigned short code)
{
	setStatusCode(code);
	setHeader();
}

void	HttpResponse::setHeader(void)
{
	std::stringstream ss;

	setDateTime();
	setLength();
	setServerName();//might put it in calling "parent" function
	setConnectionStatus();//TODO
	_header = "HTTP/1.1 ";
	ss << _status_code;
	_header.append(ss.str());
	_header.append(" ");
	_header.append(_status_message);
	_header.append("\r\n");
	_header.append("Date: ");
	_header.append(_date);
	_header.append("\r\n");
	_header.append("Server: ");
	_header.append(_server_name);
	_header.append("\r\n");
	_header.append("Content-Length: ");
	ss.str("");
	ss << _content_length;
	_header.append(ss.str());
	_header.append("\r\n");
	_header.append("Content-Type: ");
	_header.append("text/html; charset=iso-8859-1");//TODO write it dynamically
	_header.append("\r\n");
	_header.append("Connection: ");
	_header.append(_connection);
	_header.append("\r\n");
}

bool	HttpResponse::setBody(std::string const& body_path)
{
	std::ifstream	file;

	std::cout << "trying to respond file : " << body_path << std::endl;
	file.open(body_path.c_str());
	if (!file.is_open())
	{
		setContent(DEFAULT_ERROR_BODY);
		setLength();
		return false;
	}
	setContent(std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()));
	setLength();
	return true;
}

void	HttpResponse::setBody(std::string::iterator start, std::string::iterator end)
{
	setContent(std::string(start, end));
	setLength();
}

void	HttpResponse::setBody(char *str, int len)
{
	setContent(std::string(str, len));
	setLength();
}
void	HttpResponse::setDateTime(void)
{
	time_t t;
	struct tm* tt;
	time (&t);
	tt = localtime(&t);
	_date = asctime(tt);
	if (*(--_date.end()) == '\n')
		_date.erase(--_date.end());
}

void	HttpResponse::setLength(void)
{
	std::stringstream ss;
	ss << _currentContentLength;
	ss >> _content_length;
}

void	HttpResponse::setLength(unsigned long length)
{
	std::stringstream ss;
	ss << length;
	ss >> _content_length;
}

void	HttpResponse::setServerName(std::string const& name)
{
	_server_name = name;
	setServerName();
}

void	HttpResponse::setServerName()
{
	_server_name = SERVER_NAME;
	_server_name.append("/");
	_server_name.append(VERSION);
	_server_name.append(" (");
	_server_name.append(OS);
	_server_name.append(")");
}

void	HttpResponse::setConnectionStatus(std::string const& connection_status)
{
	_connection = connection_status;
}

void	HttpResponse::setConnectionStatus(bool connection_status)
{
	if (connection_status == true)
		_connection = "keep-alive";
	else
		_connection = "close";
}
