#ifndef HTTPMESSAGE_H
# define HTTPMESSAGE_H

#include <iostream>
#include <map>
#include "http_method.hpp"
#include "utils.hpp"


class HttpMessage {

public:
	HttpMessage(void);
	HttpMessage(HttpMessage const& to_copy);
	HttpMessage(std::string str_message);
	virtual ~HttpMessage(void);

	HttpMessage&	operator=(HttpMessage const& to_copy);

	void								addHeader(std::pair<std::string, std::string>& header);
	void								addTrailingHeader(std::pair<std::string, std::string>& header);
	std::multimap<std::string, std::string> const&			getHeaders(void) const;
	std::multimap<std::string, std::string> const&			getTrailingHeaders(void) const;
	void								printHeaders();
	void								printTrailers();
	void								clear();
		long				getContentLength(void) const;
	void				setContentLength(long length);
	void				setContent(std::string const& req_content);
	char				*getContent() const;
	int				hasContent(void) const;
	void				setHasContent(bool hasContent);
	void				appendToContent(std::string& to_append);
	void				appendToContent(char *str, int len);
	long/* const&	*/		getCurrentContentLength() const;


protected:
	std::multimap<std::string, std::string>				_headers;
	std::multimap<std::string, std::string>				_trailing_headers;
	long								_contentLength;
	long								_currentContentLength;
	char								*_content;
	std::string							_protocol;
	std::string							_stringMessage;
	bool								_hasBody;

};

#endif
