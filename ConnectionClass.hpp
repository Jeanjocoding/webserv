#ifndef CONNECTIONCLASS_H
# define CONNECTIONCLASS_H

#include <iostream>
#include <sys/socket.h>
#define	READING_BUF_SIZE 64
#define	SINGLE_READ_SIZE 8


struct readingBuffer
{
	char			buf[READING_BUF_SIZE];
	int				deb;
	int				end;
	static int const		cap = READING_BUF_SIZE;
};

class RecvErrorException: public std::exception {
	const char *what() const throw()
	{
		return "recv returned -1";
	}
};

class ConnectionClosedException: public std::exception {
	const char *what() const throw()
	{
		return "connection closed by client";
	}
};


class ConnectionClass {

public:
	typedef RecvErrorException RecvExcept;
	typedef	ConnectionClosedException CloseExcept;
	ConnectionClass(ConnectionClass const& to_copy);
	ConnectionClass(int socknum);
	~ConnectionClass(void);

	ConnectionClass&	operator=(ConnectionClass const& to_copy);
	std::string			receiveRequest(void);
	void				sendResponse(std::string response);
	void				closeConnection(void);
	int				_socketNbr;

	ConnectionClass(void);
private:
	typedef struct readingBuffer readingBuffer;

	readingBuffer	_buffer;
//	char			_readingBuffer[READING_BUF_SIZE];
	int				_BufIndex;
	int				_findInBuf(char *to_find, char *buf, char len);
	void			_initializeBuffer();

};

#endif
