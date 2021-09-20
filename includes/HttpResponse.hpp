/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/02 15:07:21 by asablayr          #+#    #+#             */
/*   Updated: 2021/09/20 15:31:01 by asablayr         ###   ########.fr       */
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
#  define OS "Ubuntu"
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
	void						setBody(std::string::const_iterator start, std::string::const_iterator end);
	void						setBody(char *str, int len);
	void						setLength(void);
	void						setLength(unsigned long length);
	void						setDateTime(void);
	void						setServerName(void);
	void						setServerName(std::string const& name);
	void						setConnectionStatus(std::string const& connection_status = "keep-alive");
	void						setConnectionStatus(bool connection_status);
	bool						isError(void) const;
	void						setError(bool val);

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

	bool				_isError;

};

#endif
