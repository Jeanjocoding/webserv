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

HttpRequest::HttpRequest(std::string str_message) : HttpMessage(str_message)
{
	parseFromString(_stringMessage);
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

/* probablement optimisable sans trop de difficulté en cas de besoin */
int				HttpRequest::parseFromString(std::string str_request)
{
	size_t pos_body = str_request.find("\r\n\r\n");

	std::string head_block = str_request.substr(0, pos_body);
	std::cout << "head_block: " << head_block << std::endl;

	if (pos_body + 4 < str_request.length())
	{
		_body = str_request.substr(pos_body + 4);
		std::cout << "body: " << _body << std::endl;
	}
	size_t		pos_runner = head_block.find("\r\n");
	_methodLine = head_block.substr(0, pos_runner);
	std::cout << "method line: " << _methodLine << std::endl;

	std::string	header_string = head_block.substr(pos_runner);
	std::cout << "header string: " << header_string << std::endl;
	return (0);	
//	_parseHeaderBlock(header_string);
}

std::string const&		HttpRequest::toString() const
{
	return (_stringMessage);
}

