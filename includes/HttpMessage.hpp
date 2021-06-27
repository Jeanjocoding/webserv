#ifndef HTTPMESSAGE_H
# define HTTPMESSAGE_H

#include <iostream>
#include <map>

class HttpMessage {

public:
	HttpMessage(void);
	HttpMessage(HttpMessage const& to_copy);
	HttpMessage(std::string str_message);
	virtual ~HttpMessage(void);

	HttpMessage&	operator=(HttpMessage const& to_copy);

	void				addHeader(std::pair<std::string, std::string>& header);
	void				printHeaders();
	void				clear();


protected:
	std::multimap<std::string, std::string>				_headers;
	std::string							_body;
	std::string							_protocol;
	std::string							_stringMessage;

};

#endif
