#include "HttpMessage.hpp"

HttpMessage::HttpMessage(void)
{
	return;	
}

HttpMessage::HttpMessage(HttpMessage const& to_copy)
{
	(*this) = to_copy;
}

HttpMessage::~HttpMessage(void)
{
	
}

HttpMessage&	HttpMessage::operator=(HttpMessage const& to_copy)
{
	_headers = to_copy._headers;
	_body = to_copy._body;
	_method_status = to_copy._method_status;
	return (*this);

}
