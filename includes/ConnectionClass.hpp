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
#include <utility>
#include <vector>
#include <ctime>
#include <sys/socket.h>
#include <unistd.h>
#include "HttpMessage.hpp"
#include "HttpRequest.hpp"
#include "serverClass.hpp"
#include "ConnectionUtils.hpp"

#define	READING_BUF_SIZE  12289
#define	SINGLE_READ_SIZE 4096
#define EMPTYBUF_READ_SIZE 4096
#define MAX_READ_BEFORE_FORCE_CLOSE 25000
#define	MAX_LINE_LENGTH 12000	// POUR SECURITY
#define	MAX_HEAD_LINES 100	// POUR SECURITY
#define	MAX_URI_SIZE 1000 // POUR SECURITY
#define CO_ISOPEN 1
#define CO_ISREADY 2
#define CO_ISDONE 3
#define CO_ISCLOSED 4
#define TCP_ERROR -1
#define HTTP_ERROR -2
#define FORCE_CLOSE_NEEDED -3
#define CONNECTION_CLOSED 0
#define NO_READ_MODE_DISABLED 0
#define NO_READ_MODE_ACTIVATED 1
#define	SAVE_REQUEST -4
#define BUFF_REST 1
#define REQUEST_AND_BUFF_REST 2


/*
Ce buffer/structure sert à lire et concaténer les données reçues dans le 
socket. c'est un buffer circulaire (je crois), du coup ça 
limite les appels a malloc, memset et compagnie. 
*/
struct readingBuffer
{
	char			buf[READING_BUF_SIZE + 1];
	/* 	IMPORTANT: deb corresponds to the index of the first character of the relevant
	***	buffer part (usually beginnin of the current line), however end corresponds to the
	***	index of the character AFTER the last character read.  */
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
//	std::pair<int, std::string>			receiveRequest(void);
	int				receiveRequest(void);
//	int				answerRequest(void);
//	int				answerRequest(HttpRequest& request);//might be const

	int				sendResponse(std::string response);//move to private
	int				closeConnection(void);
	HttpRequest const&	getRequest(unsigned int request_number = 0) const;
	time_t			getTimer() const;
	void			resetTimer();
	int				getStatus(void) const;
	void			setStatus(int state);
	bool			isPersistent(void) const;
	void		print_pipeline();

	// this constructor should be private, but it doesn't work for now when it is.
	//need to fix, probably an unwanted copy at some point.
	ConnectionClass(void); 

	std::vector<HttpRequest> _request_pipeline;//might try to switch back to private
	int				_socketNbr;
	serverClass*	_server;


private:
	typedef struct readingBuffer readingBuffer;

//	std::vector<HttpRequest> _request_pipeline;
	HttpRequest	*_incompleteRequest;
	std::string	*_restBuffer;
	std::string	*_beginningRestBuffer;
	std::string	_currentLine;

//	readingBuffer			_buffer;

	/* connection status, we'll see if we really need it */
	int				_status;
	bool				_isPersistent;
	int				_hasRestRequest;
	int				_hasRestBuffer;
	int				_hasBegRest;

	int				_hasRead;
	int				_isHandlingBody;

	int				_isParsingContent;
	int				_ContentLeftToRead;


	int				_isChunking;
	int				_isReadingChunknbr;
	int				_leftChunkedToRead;

	int				_isProcessingLastNL;
	int				_isProcessingTrailers;

	time_t			_timer;

	int		_read_long_line(std::string& str, readingBuffer& buffer, int& length_parsed);
	int		_read_buffer(readingBuffer& buffer, std::vector<HttpRequest>& requestPipeline);
	int		_read_line(readingBuffer& buffer, HttpRequest& currentRequest);
	int		_get_next_request(readingBuffer &buffer, HttpRequest& currentRequest);
	void		_initializeBuffer(readingBuffer& buffer);
	int		_read_first_line(readingBuffer& buffer, int& length_parsed, HttpRequest& currentRequest);
	int		_parse_line(const char *line, int len, HttpRequest& currentRequest);
	int		_parse_first_line(const char *line, int len, HttpRequest& currentRequest);
	int		_parseHeaderLine(const char *line, int len, HttpRequest& currentRequest);
	int		_check_header_compliancy(HttpRequest& CurrentRequest);
	int		_parseProtocol(HttpRequest& currentRequest, std::string& protocol);
	int		_read_request_content(HttpRequest& CurrentRequest, readingBuffer& buffer);
	void				_printBufferInfo(readingBuffer& buffer, std::string msg);
	int		_invalidRequestProcedure(HttpRequest& currentRequest, int errorCode);
	int				_findInBuf(std::string to_find,char *buf, int findlen, int buflen, int begsearch);
	int		_save_request_and_buffer(HttpRequest& currentRequest, readingBuffer& readingBuffer);
	void		_save_only_buffer(readingBuffer& readingBuffer);
	int		_caseInsensitiveComparison(std::string s1, std::string s2) const;
	int		_guaranteedRead(int fd, int to_read, std::string& str_buffer);
	int		_getChunkedData(HttpRequest& currentRequest, readingBuffer& buffer);
	int		_read_chunked_line(readingBuffer& buffer, std::string& line);
		int				_findAndParseContentHeaders(HttpRequest& currentRequest, std::pair<std::string, std::string> const& header);
	int		_readAndAppendChunkBlock(HttpRequest& currentRequest, readingBuffer& buffer);
	int		_processRemainingCrlf(readingBuffer& buffer);
	int		_readTrailers(readingBuffer& buffer, HttpRequest& currentRequest);
	int		_findInTrailers(std::string& to_find, HttpRequest& currentRequest);
	int		_read_line_trailer(readingBuffer& buffer, HttpRequest& currentRequest);
	int		_parseTrailerLine(const char *line, int len, HttpRequest& currentRequest);
	int		_findAndParsePersistanceHeaders(HttpRequest& currentRequest, std::pair<std::string, std::string> const& header);
	int		_emptyReadBuffers() const;
	int		_saveBegRestProcedure(HttpRequest& currentRequest, readingBuffer& buffer);
	int		_keepReadingContent(HttpRequest& currentRequest, readingBuffer& buffer);
	int		_save_only_request(HttpRequest& currentRequest);
	void		_print_content_info(readingBuffer& buffer, HttpRequest& currentRequest, std::string message);
	void		_print_content_info(HttpRequest& currentRequest, std::string message);
	int		_last_nl_procedure(readingBuffer& buffer);
};

#endif
