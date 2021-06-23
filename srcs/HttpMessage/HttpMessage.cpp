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
	
}

HttpMessage&	HttpMessage::operator=(HttpMessage const& to_copy)
{
	_headers = to_copy._headers;
	_body = to_copy._body;
	return (*this);

}

void		HttpMessage::addHeader(std::pair<std::string, std::string>& header)
{
	_headers.insert(header);
}

void		HttpMessage::printHeaders()
{
	std::map<std::string, std::string>::iterator itdeb = _headers.begin();
	std::map<std::string, std::string>::iterator itend = _headers.end();

	std::cout << "print all headers: " << std::endl;
	while (itdeb != itend)
	{
		std::cout << (*itdeb).first << " : " << (*itdeb).second << std::endl;
		itdeb++;
	}
	return;
}