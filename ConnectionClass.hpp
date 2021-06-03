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

	// here is how the handle connection could work (without error handling etc):
	void	handle_connection(ConnectionClass& connection)
	{
		std::string str_request;
		std::string str_response;

		str_request = connection.receiveRequest();
		str_response = parse_and_execute_request(str_request);
		connection.sendResponse(str_response);
	}
*/



#ifndef CONNECTIONCLASS_H
# define CONNECTIONCLASS_H

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#define	READING_BUF_SIZE 64 //ces tailles sont très petites
#define	SINGLE_READ_SIZE 8	// pour voir plus facilement les bugs

/*
Ce buffer sert à lire et concaténer les données reçues dans le 
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

/* Pour l'instant je gère les erreurs et les fermetures de
connection avec des exceptions, une alternative pourrait être
de renvoyer une pair<retour de read, contenu de la chaine>.
*/

/*	Exception lancée quand read retourne -1: */

/* class RecvErrorException: public std::exception {
	const char *what() const throw()
	{
		return "recv returned -1";
	}
}; */

/*	Exception lancée quand read retourne -1: */
/* class SendErrorException: public std::exception {
	const char *what() const throw()
	{
		return "send returned -1";
	}
}; */

/*	Exception lancée quand read retourne 0, et donc ferme si j'ai bien compris: */
/* class ConnectionClosedException: public std::exception {
	const char *what() const throw()
	{
		return "connection closed by client";
	}
}; */

class ConnectionClass {

public:
//	typedef RecvErrorException RecvExcept;
//	typedef	SendErrorException SendExcept;
//	typedef	ConnectionClosedException CloseExcept;
	ConnectionClass(ConnectionClass const& to_copy);

	/* only useful constructor: initializes with socket fd */
	ConnectionClass(int socknum);


	~ConnectionClass(void);

	ConnectionClass&	operator=(ConnectionClass const& to_copy);

	/* main function, reads on the socket and sends request on std::string format */
	std::pair<int, std::string>			receiveRequest(void);



	/* sends an std::string as a response to the client: */
	int				sendResponse(std::string response);



	int				closeConnection(void);


	int					_socketNbr;

	// this constructor should be private, but it doesn't work for now when it is.
	//need to fix, probably an unwanted copy at some point.
	ConnectionClass(void); 


private:
	typedef struct readingBuffer readingBuffer;

	readingBuffer	_buffer;

	/* method not used yet: */
	int				_findInBuf(char *to_find, char *buf, char len);

	/* initialize buffer when class is instantiated: */ 
	void			_initializeBuffer();

};

#endif
