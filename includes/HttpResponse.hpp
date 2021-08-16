/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/02 15:07:21 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/12 16:57:25 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# define HTTP_VERSION "HTTP/1.1"
# define DEFAULT_ERROR_BODY "<html><body><h1>Error</h1></body></html>"
# define SERVER_NAME "webserv"
# define VERSION "0.1"
# ifdef __APPLE__
#  define OS "MacOS"
# else
#  define OS "Ubuntu"//TODO check for real nginx value
# endif

#include "HttpMessage.hpp"

class HttpResponse: public HttpMessage {

public:

	HttpResponse(void);
	HttpResponse(HttpResponse const& to_copy);
	HttpResponse(unsigned short code, std::string path_to_body);
	virtual ~HttpResponse(void);

	HttpResponse&				operator = (HttpResponse const& to_copy);

	std::string					toString() const;
	void						setStatusCode(std::string const& status_str);
	void						setStatusCode(unsigned short status_str);
	void						setStatusMessage(void);
	void						setHeader(unsigned short code);
	void						setHeader(void);
	bool						setBody(std::string const& body_path);
	void						setBody(std::string::iterator start, std::string::iterator end);
	void						setLength(void);
	void						setLength(unsigned long length);
	void						setDateTime(void);
	void						setServerName(void);
	void						setServerName(std::string const& name);
	void						setConnectionStatus(void);

private:

	std::string					headerToString() const;

	std::string			_header;
	std::string			_status_line;
	std::string			_http_version; //TODO should be in httpMessage
	std::string			_status_code;
	std::string			_status_message;
	std::string			_date;
	std::string			_server_name;
	std::string			_content_length;
	std::string			_content_type;
	std::string			_connection;

	long				_contentLength;

};

#endif
