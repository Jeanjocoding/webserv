/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/02 16:27:33 by asablayr          #+#    #+#             */
/*   Updated: 2021/10/01 11:29:43 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <sstream>
#include <streambuf>
#include "HttpResponse.hpp"

HttpResponse::HttpResponse(void): HttpMessage()
{
	//TODO
	_connection = "keepalive";
	_isError = 0;
}

HttpResponse::HttpResponse(HttpResponse const& copy) : HttpMessage(copy)
{
	_header = copy._header;
	_status_line = copy._status_line;
	_http_version = copy._http_version;
	_status_code = copy._status_code;
	_status_message = copy._status_message;
	_date = copy._date;
	_server_name = copy._server_name;
	_content_length = copy._content_length;
	_content_type = copy._content_type;
	_connection = copy._connection;
	_isError = copy._isError;
}

HttpResponse::~HttpResponse(void)
{
	//TODO
}

HttpResponse::HttpResponse(unsigned short status_code, std::string body_path)
{
	setConnectionStatus();
	if (status_code == 301 || status_code == 302)
	{
		_location = body_path;
		setHeader(status_code);
	}
	else if(!setBody(body_path))
		setHeader(500);
	else
		setHeader(status_code);
}

HttpResponse::HttpResponse(unsigned short status_code, std::string::const_iterator begin, std::string::const_iterator end)
{
	setConnectionStatus();
	if (status_code == 301 || status_code == 302)
	{
		_location = std::string(begin, end);
		setHeader(status_code);
	}
	try// Not secure
	{
		setBody(begin, end);
		setHeader(status_code);
	}
	catch (std::exception const& e)
	{
		setHeader(500);
	}
}

HttpResponse&	HttpResponse::operator = (HttpResponse const& copy)
{
	HttpMessage::operator = (copy);
	_header = copy._header;
	_status_line = copy._status_line;
	_http_version = copy._http_version;
	_status_code = copy._status_code;
	_status_message = copy._status_message;
	_date = copy._date;
	_server_name = copy._server_name;
	_content_length = copy._content_length;
	_content_type = copy._content_type;
	_connection = copy._connection;
	_isError = copy._isError;
	return (*this);
}

std::string		HttpResponse::toString(void) const
{
	std::string res = headerToString();
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
	else if (_status_code == "204")
		_status_message = "No Content";
	else if (_status_code == "400")
	{
		_status_message = "Invalid Request";
		setConnectionStatus(false);
	}
	else if (_status_code == "403")
		_status_message = "Forbidden";
	else if (_status_code == "404")
		_status_message = "Not Found";
	else if (_status_code == "405")
		_status_message = "Method Not Allowed";
	else if (_status_code == "413")
	{
		_status_message = "Request Entity Too Large";
		setConnectionStatus(false);
	}
	else if (_status_code == "408")
		_status_message = "Request Timeout";
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
	setDateTime();
	setLength();
	setServerName();//might put it in calling "parent" function
}

std::string	HttpResponse::headerToString(void) const
{
	std::stringstream ss;
	std::string header;

	header = "HTTP/1.1 ";
	ss << _status_code;
	header.append(ss.str());
	header.append(" ");
	header.append(_status_message);
	header.append("\r\n");
	header.append("Date: ");
	header.append(_date);
	header.append("\r\n");
	header.append("Server: ");
	header.append(_server_name);
	header.append("\r\n");
	header.append("Content-Length: ");
	header.append(_content_length);
	header.append("\r\n");
	header.append("Content-Type: ");
	header.append("text/html; charset=iso-8859-1");//TODO write it dynamically
	header.append("\r\n");
	header.append("Connection: ");
	header.append(_connection);
	header.append("\r\n");
	if (!_location.empty())
	{
		header.append("Location: ");
		header.append(_location);
		header.append("\r\n");
	}
	return header;
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

void	HttpResponse::setBody(std::string::const_iterator start, std::string::const_iterator end)
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

bool	HttpResponse::isError(void) const
{
	return (_isError);
}

void	HttpResponse::setError(bool val)
{
	_isError = val;
}
