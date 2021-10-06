#include "HttpRequest.hpp"

HttpRequest::HttpRequest(void): HttpMessage()
{
	_lineCount = 0;
	_isValid = 0;
	_hasTE = 0;
	_hasTrailers = 0;
	_isChunked = 0;
	return;	
}

HttpRequest::HttpRequest(HttpRequest const& to_copy) : HttpMessage(to_copy)
{
	*this = to_copy;
}

HttpRequest::~HttpRequest(void)
{
	clear();
}

HttpRequest&	HttpRequest::operator=(HttpRequest const& to_copy)
{
	clear();
	HttpMessage::operator=(to_copy)	;
	_isValid = to_copy._isValid;
	_startLine = to_copy._startLine;
	_method = to_copy._method;
	_methodArgument = to_copy._methodArgument;
	_requestLine = to_copy._requestLine;
	_errorCode = to_copy._errorCode;
	_lineCount = to_copy._lineCount;
	_hasBody = to_copy._hasBody;
	_hasTE = to_copy._hasTE;
	_transferEncodings = to_copy._transferEncodings;
	_isChunked = to_copy._isChunked;
	_hasTrailers = to_copy._hasTrailers;
	_trailers = to_copy._trailers;
	_connectionOptions = to_copy._connectionOptions;
	_location = to_copy._location;
	return (*this);
}

void		HttpRequest::clear(void)
{
	HttpMessage::clear();
	_isValid = 0;
	_startLine.clear();
	_methodArgument.clear();
	_errorCode = 0;
	_requestLine.method.clear();
	_requestLine.target.clear();
	_requestLine.protocol.first = 0;
	_requestLine.protocol.second = 0;
	_lineCount = 0;
	_hasBody = 0;
	_hasTE = 0;
	_transferEncodings.clear();
	_isChunked = 0;
	_trailers.clear();
	_hasTrailers = 0;
	_connectionOptions.clear();
	_currentContentLength = 0;
	_location = 0;
}

void				HttpRequest::addRequestLine(std::string& method, std::string& target)
{
	_requestLine.method = method;	
	if (method == "GET")
		_requestLine.int_method = GET_METHOD;
	else if (method == "POST")
		_requestLine.int_method = POST_METHOD;
	else if (method == "DELETE")
		_requestLine.int_method = DELETE_METHOD;
	else
		_requestLine.int_method = -1;
	if (target.find("?") == std::string::npos)
		_requestLine.target = target;
	else
	{
		_requestLine.query_string = std::string(target, target.find("?") + 1);
		_requestLine.target = std::string(target, 0, target.find("?"));
	}
}

void				HttpRequest::setValidity(bool validity)
{
	_isValid = validity;
}

void				HttpRequest::setErrorCode(int errorCode)
{
	_errorCode = errorCode;
}

int				HttpRequest::getErrorCode() const
{
	return (_errorCode);
}

int				HttpRequest::getLineCount() const
{
	return (_lineCount);
}

void				HttpRequest::incrementLineCount(void)
{
	_lineCount++;
}


void				HttpRequest::setProtocolVersion(int bigVersion, int smallVersion)
{
	_requestLine.protocol.first = bigVersion;
	_requestLine.protocol.second = smallVersion;
}

void		HttpRequest::setStartLine(std::string const& line)
{
	_startLine = line;
}

std::string const& HttpRequest::getStartLine(void) const
{
	return (_startLine);
}

int			HttpRequest::getMethod(void) const
{
	return (_requestLine.int_method);
}

requestLineInfos const&		HttpRequest::getRequestLineInfos(void) const
{
	return (_requestLine);
}

std::vector<std::string>& HttpRequest::getModifyableTE()
{
	return (_transferEncodings);
}

void			HttpRequest::setIsChunked(bool value)
{
	_isChunked = value;
}

bool			HttpRequest::isChunked(void) const
{
	return (_isChunked);
}

void			HttpRequest::setHasTE(bool value)
{
	_hasTE = value;
}

bool			HttpRequest::HasTE(void) const
{
	return (_hasTE);
}

bool			HttpRequest::isValid(void) const
{
	return (_isValid);
}

bool			HttpRequest::isCGI(void) const
{
	if (_location->isCGI() || _method == POST_METHOD)
		return true;
	return (false);
}

void			HttpRequest::setLocation(LocationClass* location)
{
	_location = location;
}

LocationClass*	HttpRequest::getLocation(void)
{
	return _location;
}

void			HttpRequest::setHasTrailer(bool value)
{
	_hasTrailers = value;
}

bool			HttpRequest::HasTrailers(void) const
{
	return (_hasTrailers);
}

std::vector<std::string>& 	HttpRequest::getModifyableTrailers(void)
{
	return (_trailers);
}

std::vector<std::string>&	HttpRequest::getModifyableConnectionOptions(void)
{
	return (_connectionOptions);
}