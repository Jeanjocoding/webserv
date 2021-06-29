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
//	HttpRequest(std::string str_message);
	virtual ~HttpRequest(void);

	HttpRequest&	operator=(HttpRequest const& to_copy);
//	virtual int					parseFromString(std::string str_request);
//	virtual std::string const&	startLinetoString(void) const;
	void				clear();
	void				addRequestLine(std::string& method, std::string& target);
	void				setValidity(bool validity);
	void				setErrorCode(int responseCode);
	int const&				getErrorCode() const;
	bool				isValid() const;
	void				setProtocolVersion(int bigVersion, int smallVersion);
	int				getLineCount() const;
	void				incrementLineCount();
	void				setStartLine(std::string const& line);
	std::string const&		getStartLine(void) const;
	long				getContentLength(void) const;
	void				setContentLength(long length);
	void				setContent(std::string const& req_content);
	std::string const&		getContent() const;

private:
	bool			_isValid;
	std::string		_startLine;
	int				_method;
	std::string		_methodArgument;
	int			_errorCode;
	requestLineInfos	_requestLine;
	int			_lineCount;
	long			_contentLength;
	std::string		_content;

	int				_parseMethodLine(void);
	int				_parseHeaderBlock(std::string str_headers);
};

#endif
