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
#include "HttpResponse.hpp"

#define	READING_BUF_SIZE  12289
#define	SINGLE_READ_SIZE 4096
#define EMPTYBUF_READ_SIZE 8192
#define MAX_READ_BEFORE_FORCE_CLOSE 1000000
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
	int				deb;
	int				end;
	static int const		cap = READING_BUF_SIZE;
};

class ConnectionClass {

public:
	ConnectionClass(ConnectionClass const& to_copy);

	ConnectionClass(int socknum, serverClass* server);
	ConnectionClass(int socknum);

	~ConnectionClass(void);

	ConnectionClass&	operator=(ConnectionClass const& to_copy);

	void			setServers(std::vector<serverClass*> server_list, int fd);
	serverClass*	getServer(std::string server_name = "");
	int				receiveRequest(void);

	int				sendResponse(std::string response);//move to private
	int				simpleCloseConnection(void);
	int				closeWriteConnection(void);
	int				closeReadConnection(void);
	bool				isClosing(void) const;
	HttpRequest const&	getRequest(unsigned int request_number = 0) const;
	time_t			getTimer() const;
	void			resetTimer();
	int				getStatus(void) const;
	void			setStatus(int state);
	bool			isPersistent(void) const;
	void			setHasToWriteOnPipe(int value);
	int			HasToWriteOnPipe();
	void			setHasToReadOnPipe(int value);
	int			HasToReadOnPipe();
	void			setHasDoneCgi(int value);
	int			HasDoneCgi();
	void			setInputFd(int value);
	int			getInputFd();			
	void			setOutputFd(int value);
	int			getOutputFd();
	void			setChildPid(int value);
	int			getChildPid();
	void			setCgiError(int value);
	int			hasCgiError() const;
	void		print_pipeline();

	ConnectionClass(void); 

	std::vector<HttpRequest>	_request_pipeline;
	int				_socketNbr;
	std::vector<serverClass*>	_servers;
	HttpResponse			*_currentResponse;
	char				*_cgiOutput;
	long				_cgiOutput_len;


private:
	typedef struct readingBuffer readingBuffer;

	HttpRequest	*_incompleteRequest;
	std::string	*_restBuffer;
	std::string	*_beginningRestBuffer;
	std::string	_currentLine;


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

	int				_hasToWriteOnPipe;
	int				_hasToReadOnPipe;
	int				_hasDoneCgi;

	int				_input_fd;
	int				_output_fd;
	int				_childPid;

	int				_isClosing;
	int				_nbrReadsSinceClose;

	int				_hasCgiError;

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
	int		_checkMaxBodyConformity(HttpRequest& currentRequest);
};

#endif
