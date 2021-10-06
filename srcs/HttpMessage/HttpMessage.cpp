#include "HttpMessage.hpp"

HttpMessage::HttpMessage(void)
{
	_contentLength = 0;
	_hasBody = 0;
	_currentContentLength = 0;
	_content = 0;
	return;	
}

HttpMessage::HttpMessage(HttpMessage const& to_copy)
{
	(*this) = to_copy;
}

HttpMessage::HttpMessage(std::string str_message): _stringMessage(str_message)
{
	return;
}

HttpMessage::~HttpMessage(void)
{
	clear();
}

void		HttpMessage::clear(void)
{
	_headers.clear();
	if (_currentContentLength)
		delete [] _content;
	_content = 0;
	_contentLength = 0;
	_protocol.clear();
	_stringMessage.clear();
	_trailing_headers.clear();
}

HttpMessage&	HttpMessage::operator=(HttpMessage const& to_copy)
{
	_headers = to_copy._headers;
	_trailing_headers = to_copy._trailing_headers;
	_contentLength = to_copy._contentLength;
	_currentContentLength = 0;
	if (to_copy._currentContentLength)
		append_to_buffer(&_content, _currentContentLength, to_copy._content, to_copy._currentContentLength);
	return (*this);

}

void		HttpMessage::addHeader(std::pair<std::string, std::string>& header)
{
	_headers.insert(header);
}

void		HttpMessage::printHeaders()
{
	std::multimap<std::string, std::string>::iterator itdeb = _headers.begin();
	std::multimap<std::string, std::string>::iterator itend = _headers.end();

	while (itdeb != itend)
	{
		std::cout << (*itdeb).first << " : " << (*itdeb).second << std::endl;
		itdeb++;
	}
	return;
}

void		HttpMessage::printTrailers()
{
	std::multimap<std::string, std::string>::iterator itdeb = _trailing_headers.begin();
	std::multimap<std::string, std::string>::iterator itend = _trailing_headers.end();

	std::cout << "print all trailers - length of trailers: " << _trailing_headers.size() << std::endl;
	while (itdeb != itend)
	{
		std::cout << (*itdeb).first << " : " << (*itdeb).second << std::endl;
		itdeb++;
	}
	return;
}

std::multimap<std::string, std::string> const&	HttpMessage::getHeaders(void) const
{
	return (_headers);
}

void		HttpMessage::addTrailingHeader(std::pair<std::string, std::string>& trailer)
{
	_trailing_headers.insert(trailer);
}

std::multimap<std::string, std::string> const&	HttpMessage::getTrailingHeaders(void) const
{
	return (_trailing_headers);
}

int			HttpMessage::hasContent(void) const
{
	return (_hasBody);
}

void			HttpMessage::setHasContent(bool hasContent)
{
	_hasBody = hasContent;
}

void		HttpMessage::setContentLength(long content_length)
{
	_contentLength = content_length;
}

long		HttpMessage::getContentLength(void) const
{
	return (_contentLength);
}

void		HttpMessage::setContent(std::string const& req_content)
{
	append_to_buffer(&_content, _currentContentLength, (char*)req_content.c_str(), req_content.length());
}

char	*HttpMessage::getContent() const
{
	return (_content);
}

void			HttpMessage::appendToContent(std::string& to_append)
{
	append_to_buffer(&_content, _currentContentLength, (char*)to_append.c_str(), to_append.length());
}

void			HttpMessage::appendToContent(char *str, int len)
{
	append_to_buffer(&_content, _currentContentLength, str, len);
}

long 		HttpMessage::getCurrentContentLength() const
{
	return (_currentContentLength);
}
