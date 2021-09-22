#include "HttpRequest.hpp"

HttpRequest::HttpRequest(void): HttpMessage()
{
	_lineCount = 0;
	_isValid = 0;
//	_contentLength = 0;
	_hasTE = 0;
	_hasTrailers = 0;
//	_hasBody = 0;
	_isChunked = 0;
//	_currentContentLength = 0;
//	_content = 0;
	return;	
}

HttpRequest::HttpRequest(HttpRequest const& to_copy) : HttpMessage(to_copy)
{
	*this = to_copy;
}

/*HttpRequest::HttpRequest(std::string str_message) : HttpMessage(str_message)
{
	parseFromString(_stringMessage);
}*/

HttpRequest::~HttpRequest(void)
{
	clear();
}



HttpRequest&	HttpRequest::operator=(HttpRequest const& to_copy)
{
//	clear(); //POSSIBLE FUITE ICI DU A CE COMMENTAIRE
	HttpMessage::operator=(to_copy)	;
//	HttpMessage::_headers = to_copy._headers;
	_isValid = to_copy._isValid;
	_startLine = to_copy._startLine;
	_method = to_copy._method;
	_methodArgument = to_copy._methodArgument;
	_requestLine = to_copy._requestLine;
	_errorCode = to_copy._errorCode;
	_lineCount = to_copy._lineCount;
//	_contentLength = to_copy._contentLength;
//	_currentContentLength = 0;
//	append_to_buffer(&_content, _currentContentLength, to_copy._content, to_copy._currentContentLength);
	_hasBody = to_copy._hasBody;
	_hasTE = to_copy._hasTE;
	_transferEncodings = to_copy._transferEncodings;
	_isChunked = to_copy._isChunked;
	_hasTrailers = to_copy._hasTrailers;
	_trailers = to_copy._trailers;
	_connectionOptions = to_copy._connectionOptions;
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
//	if (_currentContentLength)
//		delete _content;
//	_content = 0;
	_requestLine.protocol.first = 0;
	_requestLine.protocol.second = 0;
	_lineCount = 0;
//	_contentLength = 0;
	_hasBody = 0;
	_hasTE = 0;
	_transferEncodings.clear();
	_isChunked = 0;
	_trailers.clear();
	_hasTrailers = 0;
	_connectionOptions.clear();
	_currentContentLength = 0;

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
	_requestLine.target = target;
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
	return (_requestLine.int_method);//why request line ?
}

/*void		HttpRequest::setContentLength(long content_length)
{
	_contentLength = content_length;
}

long		HttpRequest::getContentLength(void) const
{
	return (_contentLength);
}

void		HttpRequest::setContent(std::string const& req_content)
{
	append_to_buffer(&_content, _currentContentLength, (char*)req_content.c_str(), req_content.length());
}

char	*HttpRequest::getContent() const
{
	return (_content);
}*/

requestLineInfos const&		HttpRequest::getRequestLineInfos(void) const
{
	return (_requestLine);
}

/*int			HttpRequest::hasContent(void) const
{
	return (_hasBody);
}

void			HttpRequest::setHasContent(bool hasContent)
{
	_hasBody = hasContent;
}*/

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

/*void			HttpRequest::appendToContent(std::string& to_append)
{
	append_to_buffer(&_content, _currentContentLength, (char*)to_append.c_str(), to_append.length());
}

void			HttpRequest::appendToContent(char *str, int len)
{
	append_to_buffer(&_content, _currentContentLength, str, len);
}*/

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

/*long		HttpRequest::getCurrentContentLength() const
{
	return (_currentContentLength);
}*/
