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