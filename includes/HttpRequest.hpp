#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "HttpMessage.hpp"

struct s_requestLineInfos
{
	std::string		method;
	std::string		target;
	std::pair<int, int>	protocol;
};


class HttpRequest: public HttpMessage {

public:
	typedef struct s_requestLineInfos requestLineInfos;

	HttpRequest(void);
	HttpRequest(HttpRequest const& to_copy);
	HttpRequest(std::string str_message);
	virtual ~HttpRequest(void);

	HttpRequest&	operator=(HttpRequest const& to_copy);
	virtual int					parseFromString(std::string str_request);
	virtual std::string const&	toString(void) const;
	void				clear();
	void				addRequestLine(std::string& method, std::string& target);
	void				setValidity(bool validity);
	void				setErrorCode(int responseCode);
	int const&				getErrorCode() const;
	bool				isValid() const;
	void				setProtocolVersion(int bigVersion, int smallVersion);


private:
	bool			_isValid;
	std::string		_methodLine;
	int				_method;
	std::string		_methodArgument;
	int			_errorCode;
	requestLineInfos	_requestLine;

	int				_parseMethodLine(void);
	int				_parseHeaderBlock(std::string str_headers);
};

#endif
