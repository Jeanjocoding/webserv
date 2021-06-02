#include "ConnectionClass.hpp"

ConnectionClass::ConnectionClass(void)
{
	return;
}

ConnectionClass::ConnectionClass(ConnectionClass const& to_copy): _socketNbr(to_copy._socketNbr)
{
	_initializeBuffer();
	return;	
}

ConnectionClass::ConnectionClass(int socknum): _socketNbr(socknum)
{
	_initializeBuffer();
	return;	
}

ConnectionClass::~ConnectionClass(void)
{
	return;
}

ConnectionClass&	ConnectionClass::operator=(ConnectionClass const& to_copy)
{
	_socketNbr = to_copy._socketNbr;
	return (*this);
}


/* not used for now, we'll see later: */
int				_findInBuf(char *to_find, char *buf, int findlen, int buflen, int begsearch)
{
	int		i = begsearch;
	int 	j = 0;

	while (i < buflen)
	{
		while (buf[i] == to_find[j])
		{
			if (j == (findlen - 1))
				return (i + 1);
			i++;
			j++;
		}
		if (j)
			j = 0;
		i++;
	}
	return (-1);
}

void				ConnectionClass::_initializeBuffer()
{
	std::memset(_buffer.buf, 0, READING_BUF_SIZE);
	_buffer.deb = 0;
	_buffer.end = 0;
}


/* This function reads on the socket and assembles responses. It tries to implement a "circular buffer".
	It maintains a "beginning index" and an "end index" and it just sequentially adds data on the buffer. 

	if the flow of data is over, the content between "deb" (beginning) and "end" is appended to the std::string, 
	"deb" is set to the current value of "end" to mark the beginning of the next request, and the std::string 
	is returned.

	if, after a few requests, the buffer is full, the current request data is appended to the string, and
	"deb" and  "end" are set to 0. The next data will be written at the beginning of the buffer, and so on.

	In the current version, it throws exceptions if recv returns 0 or -1
 */
std::string			ConnectionClass::receiveRequest()
{
	int				read_ret;
	std::string		str_request;
	int				size_to_append;

	if (_buffer.cap - _buffer.end <= SINGLE_READ_SIZE)
	{
		str_request.append(&(_buffer.buf[_buffer.deb]), _buffer.end - _buffer.deb);
		_buffer.deb = 0;
		_buffer.end = 0;
	}
	while ((read_ret = recv(_socketNbr, &(_buffer.buf[_buffer.end]), SINGLE_READ_SIZE, 0)) > 0)
	{
		_buffer.end += read_ret;
		if (read_ret < SINGLE_READ_SIZE)
			break;
		if (_buffer.cap - _buffer.end <= SINGLE_READ_SIZE)
		{
			str_request.append(&(_buffer.buf[_buffer.deb]), _buffer.end - _buffer.deb);
			_buffer.deb = 0;
			_buffer.end = 0;
		}
	}
	if (read_ret == -1)
	{
		perror("recv");
		throw RecvExcept();
	}
	else if (read_ret == 0)
	{
		throw CloseExcept();
	}
	size_to_append = _buffer.end - _buffer.deb;
	if (size_to_append > 0)
		str_request.append(&(_buffer.buf[_buffer.deb]), size_to_append);
	_buffer.deb = _buffer.end;
	return (str_request);
}