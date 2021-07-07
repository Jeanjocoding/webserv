#include "HttpMessage.hpp"

HttpMessage::HttpMessage(void)
{
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
	_body.clear();
	_protocol.clear();
	_stringMessage.clear();
	_trailing_headers.clear();
}

HttpMessage&	HttpMessage::operator=(HttpMessage const& to_copy)
{
//	std::multimap<std::string, std::string>::const_iterator	itdeb = to_copy._headers.begin();
//	std::multimap<std::string, std::string>::const_iterator	itend = to_copy._headers.end();

//	std::cout << "header size: " << to_copy._headers.size() << std::endl;
	_headers = to_copy._headers;
//	if (to_copy._headers.size())
//	{
//		std::cout << "first: " << (*itdeb).first <<std::endl;
//		_headers.insert(*itdeb);
//	}
	_trailing_headers = to_copy._trailing_headers;
	_body = to_copy._body;
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

//	std::cout << "print all headers: " << std::endl;
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