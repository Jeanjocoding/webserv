/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/02 15:07:21 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/04 18:15:51 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#define HTTP_VERSION "HTTP/1.1"

#include "HttpMessage.hpp"

class HttpResponse: public HttpMessage {

public:

	HttpResponse(void);
	HttpResponse(HttpResponse const& to_copy);
//	HttpResponse(std::string str_message);
	virtual ~HttpResponse(void);

	HttpResponse&				operator=(HttpResponse const& to_copy);
//	virtual int					parseFromString(std::string str_request);
//	virtual std::string const&	startLinetoString(void) const;
	void						clear();
	void						addResponseLine(std::string& method, std::string& target);
	void						setValidity(bool validity);
	void						setErrorCode(int responseCode);
	int const&					getErrorCode() const;
	bool						isValid() const;
	void						setProtocolVersion(int bigVersion, int smallVersion);
	int							getLineCount() const;
	void						incrementLineCount();
	void						setStartLine(std::string const& line);
	std::string const&			getStartLine(void) const;
	long						getContentLength(void) const;
	void						setContentLength(long length);
	void						setContent(std::string const& req_content);
	std::string const&			getContent() const;

private:

	std::string			_status_line;
	std::string const	_http_version = HTTP_VERSION;
	std::string			_status_code;
	std::string			_reason_phrase;

	std::string			_date;
	std::string			_server_name;
	std::string			_content_length;
	std::string			_content_type;
	std::string			_connection;


	std::string			_startLine;
	int					_method;
	std::string			_methodArgument;
	int					_errorCode;
	requestLineInfos	_requestLine;
	int					_lineCount;
	long				_contentLength;
	std::string			_content;

};

#endif
