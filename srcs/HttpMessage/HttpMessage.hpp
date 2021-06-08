#ifndef HTTPMESSAGE_H
# define HTTPMESSAGE_H

#include <iostream>
#include <map>

class HttpMessage {

public:
	HttpMessage(void);
	HttpMessage(HttpMessage const& to_copy);
	~HttpMessage(void);

	HttpMessage&	operator=(HttpMessage const& to_copy);

	virtual int					parseFromString(std::string) = 0;
	virtual std::string& const	toString(void) const = 0;

private:
	std::map<std::string, std::string>	_headers;
	std::string							_body;
	std::string							_method_status;
};

#endif
