#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "HttpMessage.hpp"

struct startLine_infos
{
	std::string	method;
	std::string	target;
	std::string	protocol;
};


class HttpRequest: public HttpMessage {

public:
	HttpRequest(void);
	HttpRequest(HttpRequest const& to_copy);
	HttpRequest(std::string str_message);
	~HttpRequest(void);

	HttpRequest&	operator=(HttpRequest const& to_copy);
	virtual int					parseFromString(std::string str_request);
	virtual std::string const&	toString(void) const;
	void				clear();

private:
	bool			_isValid;
	std::string		_methodLine;
	int				_method;
	std::string		_methodArgument;

	int				_parseMethodLine(void);
	int				_parseHeaderBlock(std::string str_headers);


};

#endif
