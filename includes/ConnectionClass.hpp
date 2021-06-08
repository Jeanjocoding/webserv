/*	This class is used to handle the socket-level part of individual connections. For now 
	it only reads and assemble requests, write responses and closes 
	connections. In the future, it could alson be used to store information 
	about connection status, addresses, timestamps or logs recording
	activity (if it happens to be useful)

	How to use it, schematically:


	// we create a map that centralizes all the individual connections,
	// the key is the socket fd, the mapped type is the connection object

	std::map<int, ConnectionClass>	connection_map;

	** we are now inside the main loop **

	// a new client arrives and gives me a fd:
	s2 = accept(bla bla bla); 

	// I initialize the connection object with the socket fd and insert it in the map:
	connection_map.insert(std::pair<int, ConnectionClass>(s2, ConnectionClass(s2)));

	** some code happens, and suddenly, we have some data to read on our fd, and 
	** we want to handle the connection:                                         

	// I send the right connection object to the connection handle:
	handle_connection(connection_map[fd]);

*/



#ifndef CONNECTIONCLASS_H
# define CONNECTIONCLASS_H

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include "serverClass.hpp"
#define	READING_BUF_SIZE 64 //ces tailles sont très petites
#define	SINGLE_READ_SIZE 8	// pour voir plus facilement les bugs
#define CO_ISOPEN 1
#define CO_ISCLOSED 2

/*
Ce buffer/structure sert à lire et concaténer les données reçues dans le 
socket. c'est un buffer circulaire (je crois), du coup ça 
limite les appels a malloc, memset et compagnie. 
*/
struct readingBuffer
{
	char			buf[READING_BUF_SIZE];
	int				deb;
	int				end;
	static int const		cap = READING_BUF_SIZE;
};

class ConnectionClass {

public:
	ConnectionClass(ConnectionClass const& to_copy);

	/* only useful constructor: initializes with socket fd */
	ConnectionClass(int socknum, serverClass* server);


	~ConnectionClass(void);

	ConnectionClass&	operator=(ConnectionClass const& to_copy);

	/* main function, reads on the socket and returns a pair containing:
		- return value of the last "recv" call (to see if it failed or if the connection was closed)
		- std::string containing the request*/
	std::pair<int, std::string>			receiveRequest(void);

	int				sendResponse(std::string response);
	int				closeConnection(void);
	int				_socketNbr;
	serverClass*	_server;
	int				getStatus();

	// this constructor should be private, but it doesn't work for now when it is.
	//need to fix, probably an unwanted copy at some point.
	ConnectionClass(void); 


private:
	typedef struct readingBuffer readingBuffer;

	readingBuffer			_buffer;

	/* connection status, we'll see if we really need it */
	int				_status;

	void			_initializeBuffer();

	/* method not used yet: */
	int				_findInBuf(char *to_find, char *buf, char len);
};

#endif
