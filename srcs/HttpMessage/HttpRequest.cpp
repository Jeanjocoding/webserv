#include "HttpRequest.hpp"

HttpRequest::HttpRequest(void)
{
	return;	
}

HttpRequest::HttpRequest(HttpRequest const& to_copy) : HttpMessage(to_copy)
{
	_isValid = to_copy._isValid;
	_methodLine = to_copy._methodLine;
	_method = to_copy._method;
	_methodArgument = to_copy._methodArgument;
}

HttpRequest::~HttpRequest(void)
{
	
}

HttpRequest&	HttpRequest::operator=(HttpRequest const& to_copy)
{
	HttpMessage::operator=(to_copy)	;
	_isValid = to_copy._isValid;
	_methodLine = to_copy._methodLine;
	_method = to_copy._method;
	_methodArgument = to_copy._methodArgument;
	return (*this);
}

int				HttpRequest::parseFromString(std::string str_request)
{
	int pos_body = str_request.find("\r\n\r\n");

	std::string head_block = str_request.substr(0, pos_body);

	if (pos_body + 4 < str_request.length())
	_body = str_request.substr(pos_body + 3);



}