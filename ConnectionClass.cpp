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