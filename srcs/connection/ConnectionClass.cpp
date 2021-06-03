#include "ConnectionClass.hpp"

ConnectionClass::ConnectionClass(void)
{
	return;
}

ConnectionClass::ConnectionClass(ConnectionClass const& to_copy): _socketNbr(to_copy._socketNbr), _status(to_copy._status)
{
	_initializeBuffer();
	return;	
}

ConnectionClass::ConnectionClass(int socknum): _socketNbr(socknum)
{
	_status = CO_ISOPEN;
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
	_status = to_copy._status;
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


/* This function reads on the socket and assembles responses. It returns a pair composed of:
	-the last return value of recv
	-an std::string containing the full request
	 
	It tries to implement a "circular buffer";
	It maintains a "beginning index" and an "end index" and it just sequentially adds data on the buffer. 

	if the flow of data is over, the content between "deb" (beginning) and "end" is appended to the std::string, 
	"deb" is set to the current value of "end" to mark the beginning of the next request, and the pair 
	is returned.

	if, after a few requests, the buffer is full, the current request data is appended to the string, and
	"deb" and  "end" are set to 0. The next data will be written at the beginning of the buffer, and so on.

 */

std::pair<int, std::string>			ConnectionClass::receiveRequest()
{
//	int								read_ret;
	std::pair<int, std::string>		return_pair;
	int								size_to_append;

	if (_buffer.cap - _buffer.end <= SINGLE_READ_SIZE)
	{
		return_pair.second.append(&(_buffer.buf[_buffer.deb]), _buffer.end - _buffer.deb);
		_buffer.deb = 0;
		_buffer.end = 0;
	}
	while ((return_pair.first = recv(_socketNbr, &(_buffer.buf[_buffer.end]), SINGLE_READ_SIZE, 0)) > 0)
	{
		_buffer.end += return_pair.first;
		if (return_pair.first < SINGLE_READ_SIZE)
			break;
		if (_buffer.cap - _buffer.end <= SINGLE_READ_SIZE)
		{
			return_pair.second.append(&(_buffer.buf[_buffer.deb]), _buffer.end - _buffer.deb);
			_buffer.deb = 0;
			_buffer.end = 0;
		}
	}
	if (return_pair.first == -1)
	{
		perror("recv");
		return (return_pair);
	}
	else if (return_pair.first == 0)
	{
		return (return_pair);
	}
	size_to_append = _buffer.end - _buffer.deb;
	if (size_to_append > 0)
		return_pair.second.append(&(_buffer.buf[_buffer.deb]), size_to_append);
	_buffer.deb = _buffer.end;
	return (return_pair);
}

int				ConnectionClass::sendResponse(std::string response)
{
	return (send(_socketNbr, response.c_str(), response.length(), 0));
}

int				ConnectionClass::closeConnection(void)
{
	int return_value;

	return_value = close(_socketNbr);
	if (return_value == 0)
		_status = CO_ISCLOSED;
	return (return_value);
}

int				ConnectionClass::getStatus(void)
{
	return (_status);
}