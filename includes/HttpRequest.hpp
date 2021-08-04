#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include <vector>
#include "HttpMessage.hpp"

typedef struct s_requestLineInfos
{
	std::string		method;
	int			int_method;
	std::string		target;
	std::pair<int, int>	protocol;
}		requestLineInfos;


class HttpRequest: public HttpMessage {

public:

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
	int				getErrorCode() const;
	bool				isValid() const;
	void				setProtocolVersion(int bigVersion, int smallVersion);
	int				getLineCount() const;
	void				incrementLineCount();
	void				setStartLine(std::string const& line);
	std::string const&		getStartLine(void) const;
	int					getMethod(void) const;
	long				getContentLength(void) const;
	void				setContentLength(long length);
	void				setContent(std::string const& req_content);
	std::string const&		getContent() const;
	requestLineInfos const&		getRequestLineInfos(void) const;
	int				hasContent(void) const;
	void				setHasContent(bool hasContent);
	void				setHasTE(bool value);
	bool				HasTE(void) const;
	std::vector<std::string>&	getModifyableTE();
	std::vector<std::string>&	getModifyableTrailers();
	void				setIsChunked(bool value);
	bool				isChunked() const;
	void				appendToContent(std::string& to_append);
	void				appendToContent(char *str, int len);
	void				setHasTrailer(bool value);
	bool				HasTrailers() const;
	std::vector<std::string>&	getModifyableConnectionOptions();



private:
	bool				_isValid;
	std::string			_startLine;
	int				_method;
	std::string			_methodArgument;
	int				_errorCode;
	requestLineInfos		_requestLine;
	int				_lineCount;
	long				_contentLength;
	std::string			_content;
	bool				_hasBody;
	std::vector<std::string>	_transferEncodings;
	std::vector<std::string>	_trailers;
	std::vector<std::string>	_connectionOptions;
	bool				_hasTE;
	bool				_isChunked;
	bool				_hasTrailers;

	int				_parseMethodLine(void);
	int				_parseHeaderBlock(std::string str_headers);
};

#endif
