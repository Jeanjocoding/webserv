#include <cstring>
#include "ConnectionClass.hpp"


ConnectionClass::ConnectionClass(void)
{
	return;
}

ConnectionClass::ConnectionClass(ConnectionClass const& to_copy): _socketNbr(to_copy._socketNbr), _server(to_copy._server), _status(to_copy._status)
{
	return;	
}

ConnectionClass::ConnectionClass(int socknum, serverClass* server): _socketNbr(socknum), _server(server)
{
	_status = CO_ISOPEN;
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
	_server = to_copy._server;
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

void				ConnectionClass::_initializeBuffer(readingBuffer buffer)
{
	std::memset(buffer.buf, 0, READING_BUF_SIZE + 1);
	buffer.deb = 0;
	buffer.end = 0;
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

//PENSER A GERER PIPELINING, SI JE READ PLUSIEURS REQUESTS À LA FOIS, JE LES PROCESS TOUTES OU
// J'EN FAIT UNE, JE PASSE À FD SUIVANT ET JE REVIENT AU TOUR D'APRÈS POUR CONTINUER?

//RÉFlexion à avoir sur la taille du read

int		ConnectionClass::_read_buffer(readingBuffer& buffer, std::vector<HttpRequest>& requestPipeline)
{
	int	length_parsed = 0;
	int	getnr_ret = 0;

	HttpRequest	currentRequest;
	while (length_parsed < READING_BUF_SIZE && length_parsed < buffer.end) // je chope toutes les requêtes qui sont dans le buffer
	{
		getnr_ret = _get_next_request(buffer, currentRequest, length_parsed);
		if (getnr_ret == -1)
			return (-1);
		if (getnr_ret == 0)
			return (0);
		requestPipeline.push_back(currentRequest);
		currentRequest.clear();
	}
	//rajouter procédure pour si le buffer s'arrête au milieu d'une requête. cette procédure sera surement
	// un dernier get_next_request
	return (1);
}

int		ConnectionClass::_read_long_line(std::string& str, readingBuffer buffer)
{
	char buf[SINGLE_READ_SIZE + 1];
	int read_ret;
	size_t	pos;

	while (read_ret = recv(_socketNbr, buf, SINGLE_READ_SIZE, 0) > 0)
	{
		buf[read_ret] = '\0';
		str.append(buf);
		pos = str.find("\r\n", str.length() - (SINGLE_READ_SIZE + 2));
		if (pos != str.npos)
		{
			std::memmove(buffer.buf, &(str.data()[pos + 2]), str.length() - (pos + 2)); // j'ajoute au buffer ce qu'il y avait après le crlf
			str.erase(pos, str.length()); // j'enleve ce qu'il y avait après le crlf dans la string

			return (1);
		}
		if (str.length() > MAX_LINE_LENGTH)
		{
			std::cout << "bad request a gerer dans la fonction std_readline" << std::endl;
			return (-1);
		}
	}
	if (read_ret == -1)
		return (-1);
	if (read_ret == 0)
		return (0);
}

int		ConnectionClass::_parse_line(const char *line, char len)
{
	std::string	test_string(line, len);

	std::cout << "the line passed to the parser is: " << test_string << std::endl;
	return;
}

int		ConnectionClass::_read_line(readingBuffer& buffer, int& length_parsed)
{
	int		crlf_index;
	int		read_ret;

	int deb_read = buffer.deb;
//	buffer.line_deb = buffer.deb;
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
		return (2);
	while ((crlf_index = _findInBuf("\r\n", buffer.buf, 2, buffer.end - deb_read, deb_read)) == -1)
	{
		if ((buffer.end + SINGLE_READ_SIZE) <  READING_BUF_SIZE) // je vérifie que j'ai de la place dans mon buffer
		{
			read_ret = recv(_socketNbr, &(buffer.buf[buffer.end]), SINGLE_READ_SIZE, 0);
			if (read_ret == -1)
				return (-1);
			if (read_ret == 0)
				return (0);
			buffer.end += read_ret;
			length_parsed += read_ret;
			deb_read = buffer.end - 1; // j'enleve 1 au cas ou crlf est coupé en 2 ; a protéger segfault			
		}
		else if (buffer.deb > (READING_BUF_SIZE / 2)) // je regarde si ça vaut le coup de faire le memmove
		{
			std::memmove(buffer.buf, &(buffer.buf[buffer.deb - 1]), buffer.end - (buffer.deb - 1)); // le -1 sert a gérer crlf coupé en 2 encore une fois
			buffer.end -= buffer.deb;
			buffer.deb = 0;
			deb_read = buffer.deb;
		}
		else
		{
			std::string	long_request_string;
			long_request_string.append(buffer.buf, buffer.deb, buffer.end - buffer.deb);
			read_ret = _read_long_line(long_request_string, buffer);
			if (read_ret == -1)
				return (-1);
			if (read_ret == 0)
				return (0);
			_parse_line(long_request_string.c_str(), long_request_string.length());
			return (1);

		}
	}
	_parse_line(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb);
	buffer.deb = crlf_index + 2;
	return (1);


}

int		ConnectionClass::_get_next_request(readingBuffer &buffer, HttpRequest& currentRequest, int& length_parsed)
{
	int ret_read_line;
	int	line_count = 0;
	//faire une protection contre segfaulkt/buffer overflow ici
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb += 2;
		buffer.deb += 2;
		length_parsed += 2;
	}
	// ici aussi:
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		std::cout << "need to handle an invalid request in _get_next_request" << std::endl;
		return (-1); // non, il faudrait autre chose
	}
	while (ret_read_line = _read_line(buffer, length_parsed) == 1)
		;
	if (ret_read_line == -1)
		return (-1);
	if (ret_read_line == 0)
		return (0);
	if (ret_read_line == 2)
		return (1);
}



/* lis une ligne. retourne 1 si une ligne a ete lue, -1 si un read a retourné 0, 0 si fermeture conn
**   de connexion, et 2 si tous les headers ont été lus */
/*int		ConnectionClass::_read_line(readingBuffer& buffer, int& length_parsed)
{
	int line_deb;
	int line_end;

	
}*/

int			ConnectionClass::receiveRequest(std::vector<HttpRequest>& requestPipeline)
{
	readingBuffer	buffer;
	int		read_ret;

	read_ret = recv(_socketNbr, buffer.buf, SINGLE_READ_SIZE, 0);
	if (read_ret == -1)
	{	
		perror("read"); // a faire partout pour le debugging?
		return (-1);
	}
	if (read_ret == 0)
		return (0);
	buffer.deb = 0;	
	buffer.end = read_ret;
	read_ret = _read_buffer(buffer, requestPipeline);
}

/*std::pair<int, std::string>			ConnectionClass::receiveRequest()
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
}*/

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
