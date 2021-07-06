/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionClass.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/04 17:42:52 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/05 15:10:59 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include "ConnectionClass.hpp"


ConnectionClass::ConnectionClass(void)
{
//	std::cout << "connection created " << std::endl;
	_hasRest = 0;
	return;
}

ConnectionClass::ConnectionClass(ConnectionClass const& to_copy): _socketNbr(to_copy._socketNbr), _server(to_copy._server), _status(to_copy._status), _isPersistent(to_copy._isPersistent), _hasRest(to_copy._hasRest)
{
	if (_hasRest)
	{
		_restBuffer = new std::string(*(to_copy._restBuffer));
		_incompleteRequest = new HttpRequest(*(to_copy._incompleteRequest));
	}
	return;	
}

ConnectionClass::ConnectionClass(int socknum, serverClass* server): _socketNbr(socknum), _server(server)
{
	_status = CO_ISOPEN;
	_hasRest = 0;
	return;	
}

ConnectionClass::~ConnectionClass(void)
{
	if (_hasRest)
	{
		delete _restBuffer;
		delete _incompleteRequest;
	}
	return;
}

ConnectionClass&	ConnectionClass::operator=(ConnectionClass const& to_copy)
{
	_socketNbr = to_copy._socketNbr;
	_status = to_copy._status;
	_server = to_copy._server;
	_isPersistent = to_copy._isPersistent;
	_hasRest = to_copy._hasRest;
	if (_hasRest)
	{
		_restBuffer = new std::string(*(to_copy._restBuffer));
		_incompleteRequest = new HttpRequest(*(to_copy._incompleteRequest));
	}
	return (*this);
}


/* not used for now, we'll see later: */
int				ConnectionClass::_findInBuf(std::string to_find, char *buf, int findlen, int index_end, int begsearch)
{
	int		i = begsearch;
	int 	j = 0;
	std::string	debug_string(buf, begsearch, index_end - begsearch);
//	std::cout << "string parsed in findinbuf: " << debug_string << " -- end of string " << std::endl;
	while (i < index_end)
	{
//		std::cout << "buf[" << i << "] : " << buf[i] << std::endl;
		while (buf[i] == to_find[j])
		{
			if (j == (findlen - 1))
			{
//				std::cout << "find in buf returns: " << i - 1 << std::endl;
				return (i - 1);
			}
			i++;
			j++;
		}
		if (j)
			j = 0;
		i++;
	}
//	std::cout << "find in buf returns: -1" << std::endl;
	return (-1);
}

void				ConnectionClass::_initializeBuffer(readingBuffer& buffer)
{
	std::memset(buffer.buf, 0, READING_BUF_SIZE + 1);
	buffer.deb = 0;
	buffer.end = 0;
}

void				ConnectionClass::_printBufferInfo(readingBuffer& buffer, std::string msg)
{
	std::cout << std::endl;
	std::cout << "--------------------------------" << std::endl;
	std::cout << "BUFFER INFOS - " << msg << " : " << std::endl;
	std::cout << "full buffer: " << buffer.buf << std::endl;
	std::cout << "buffer.deb: " << buffer.deb << std::endl;;
	std::cout << "buffer.end: " << buffer.end << std::endl;
	std::string str_buffer(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
	std::cout << "buffer from buf.deb to buf.end: " << str_buffer << std::endl;;
	std::cout << "--------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
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
	int	req_count = 0;
	int read_ret;

//	std::cout << std::endl;
//	std::cout << "arriving in _read_buffer" << std::endl;

//	_printBufferInfo(buffer, "in _read_buffer");
//	std::cout << std::endl;
	HttpRequest	currentRequest;

	if (_hasRest)
	{
//		std::cout << "there is rest!" << std::endl;
		std::memmove(buffer.buf, _restBuffer->c_str(), _restBuffer->length());
		buffer.end = _restBuffer->length();
		length_parsed += _restBuffer->length();
		delete _restBuffer;
		if (_hasRest == REQUEST_AND_BUFF_REST)
		{
			currentRequest = *_incompleteRequest;
			delete _incompleteRequest;
		}
//		_printBufferInfo(buffer, " rest buffer");
		_hasRest = 0;
	}

	read_ret = recv(_socketNbr, &(buffer.buf[buffer.end]), SINGLE_READ_SIZE, 0);
//	std::cout << "buffer received by read: " << std::endl;
	if (read_ret == -1)
	{	
		perror("read"); // a faire partout pour le debugging?
		return (-1);
	}
	if (read_ret == 0)
		return (0);
//	buffer.deb = 0;	
	buffer.end += read_ret;
	length_parsed += read_ret;
	buffer.buf[buffer.end] = '\0';

	//* procédure insatisfaisante, il faut réussir a faire en sorte que ça s'arrete une fois la dernière reuqête lue: */
	while ((length_parsed < SINGLE_READ_SIZE && length_parsed < buffer.end) || req_count == 0) // je chope toutes les requêtes qui sont dans le buffer
	{
//		std::cout << "length_parsed: " << length_parsed << std::endl;
//		std::cout << "reading_buf_size: " << SINGLE_READ_SIZE << ", buffer.end: " << buffer.end << ", req_count: " << req_count << std::endl;
		getnr_ret = _get_next_request(buffer, currentRequest, length_parsed, NO_READ_MODE_DISABLED);
		if (getnr_ret == -1)
			return (-1);
		if (getnr_ret == 0)
			return (0);
		if (getnr_ret == HTTP_ERROR)
			return (HTTP_ERROR);
		requestPipeline.push_back(currentRequest);
//		std::cout << "pipeline length: " << requestPipeline.size() << std::endl;
		req_count++;
		currentRequest.clear();
	}
//	_printBufferInfo(buffer, "before no read gnr");
	if (buffer.deb >= buffer.end)
	{
//		std::cout << "there seems to be nothing left to parse after the last request. it probably means there was no pipelining at all" << std::endl;
		return (1);
	}
	getnr_ret = _get_next_request(buffer, currentRequest, length_parsed, NO_READ_MODE_ACTIVATED);
	if (getnr_ret == -1)
		return (-1);
	else if (getnr_ret == 0)
		return (0);
	else if (getnr_ret == HTTP_ERROR)
		return (HTTP_ERROR);
	else if (getnr_ret == SAVE_REQUEST)
		return (1);
	requestPipeline.push_back(currentRequest);
	if (buffer.deb < buffer.end)
	{
//		std::cout << "there will be a buffer rest " << std::endl;
		_save_only_buffer(buffer);
	}
	return (1);
}

/* 	this function launches a  rather slow procedure but that is used very rarely. it reads on the
***	socket and append buffers to a std::string object until crlf is found. after, it moves
***	the rest of the string (after the crlf)	in the readingBuffer. buf and set buffer.deb and buffer.end
***	accordingly */
int		ConnectionClass::_read_long_line(std::string& str, readingBuffer& buffer, int& length_parsed)
{
	char buf[SINGLE_READ_SIZE + 1];
	int read_ret;
	size_t	pos;

//	std::cout << "we're inside _read_long_line" << std::endl;
	while ((read_ret = recv(_socketNbr, buf, SINGLE_READ_SIZE, 0)) > 0)
	{
		buf[read_ret] = '\0';
		length_parsed += read_ret;
		str.append(buf);
//		std::cout << "str after append: " << std::endl;
		pos = str.find("\r\n", str.length() - (SINGLE_READ_SIZE + 2));
		if (pos != str.npos)
		{
			std::memmove(buffer.buf, &(str.data()[pos + 2 ]), str.length() - (pos + 2)); // j'ajoute au buffer ce qu'il y avait après le crlf
			buffer.deb = 0;
			buffer.end = str.length() - (pos + 2);
			str.erase(pos, str.length()); // j'enleve ce qu'il y avait après le crlf dans la string
			return (1);
		}
		if (str.length() > MAX_LINE_LENGTH)
		{
//			std::cout << "bad request a gerer dans la fonction std_readline" << std::endl;
			return (-1);
		}
	}
	if (read_ret == -1)
		return (-1);
	if (read_ret == 0)
		return (0);
	std::cout << "unpredicted return in _read_long_line" << std::endl;
	return (1);
}

int		ConnectionClass::_invalidRequestProcedure(HttpRequest& currentRequest, int errorCode)
{
	std::cout << "invalid request procedure is called" << std::endl;
	currentRequest.setValidity(0);
	currentRequest.setErrorCode(errorCode);
	return (-1);
}

/*bool		ConnectionClass::_hasRightFrequency(const char *line, int len, std::string target, int targetFrequency)
{
	int		count;

	while (pos)

}*/

int		ConnectionClass::_parseProtocol(HttpRequest& currentRequest, std::string& protocol)
{
	int	bigVersion = 0;
	int	smallVersion = 0;
	int 	index = 5;
	int	length = protocol.length();

//	std::cout << "protocol string recieved: " << protocol << std::endl;
	if (protocol.length() < 6)
		return (_invalidRequestProcedure(currentRequest, 400));
	if (protocol.find("HTTP/") != 0)
		return (_invalidRequestProcedure(currentRequest, 400));
	while (protocol[index] != '.')
	{
//		std::cout << "protocol[" << index <<"] : " << protocol[index] << std::endl;
		if (!isdigit(protocol[index]))
			return (_invalidRequestProcedure(currentRequest, 400));
		bigVersion = bigVersion * 10 + protocol[index] - '0';
		index++;
		if (index == length)
			return (_invalidRequestProcedure(currentRequest, 400));
	}
	index++;
	if (index == length)
		return (_invalidRequestProcedure(currentRequest, 400));
	while (index < length)
	{
//		std::cout << "protocol[" << index <<"] : " << protocol[index] << std::endl;
		if (!isdigit(protocol[index]))
			return (_invalidRequestProcedure(currentRequest, 400));
		smallVersion = smallVersion * 10 + protocol[index] - '0';
		index++;
//		if (index == length)
//			return (_invalidRequestProcedure(currentRequest, 400));
	}
//	std::cout << "protocol version post atoi: " <<  bigVersion << ":" << smallVersion << std::endl;
	currentRequest.setProtocolVersion(bigVersion, smallVersion);
	if (bigVersion < 1 || (bigVersion == 1 && smallVersion == 0))
		_isPersistent = 0;
	else
		_isPersistent = 1;
	return (1);
}

int		ConnectionClass::_parse_first_line(const char *line, int len, HttpRequest& currentRequest)
{
//	std::string	to_parse(line, len);
	int		index = 0;
	int		lastWordIndex;
	int		protocol_length = 0;

	while (line[index] != ' ')
	{
		if (!std::isupper(line[index]))
			return (_invalidRequestProcedure(currentRequest, 400));
		index++;
		if (index >= len)
		return (_invalidRequestProcedure(currentRequest, 400));

	}
	std::string method(line, index);
	index++;
	if (line[index] == ' ')
			return (_invalidRequestProcedure(currentRequest, 400));
	lastWordIndex = index;
	while (line[index] != ' ')
	{
		index++;
		if (index >= len)
			return (_invalidRequestProcedure(currentRequest, 400));
	}
	std::string target(line, lastWordIndex, index - lastWordIndex);
	index++;
	if (line[index] == ' ')
			return (_invalidRequestProcedure(currentRequest, 400));
	lastWordIndex = index;
	while (index < len)
	{
		protocol_length++;
		if (line[index] == ' ' || protocol_length > 20)
			return (_invalidRequestProcedure(currentRequest, 400));
		index++;
	}
	std::string protocol(line, lastWordIndex, protocol_length);
	if (method != "GET" && method != "POST" && method != "DELETE")
		return (_invalidRequestProcedure(currentRequest, 501));
	if (target.length() > MAX_URI_SIZE)
		return (_invalidRequestProcedure(currentRequest, 414));
	if (_parseProtocol(currentRequest, protocol) == -1)
		return (-1);
	else
	{
		currentRequest.addRequestLine(method, target);
//		std::cout << "method: " << method << std::endl;
//		std::cout << "target: " << target << std::endl;
		std::string startLine(line, len);
		currentRequest.setStartLine(startLine);
		return (1);
	}
}

int		ConnectionClass::_caseInsensitiveComparison(std::string s1, std::string s2) const
{
	if (s1.length() != s2.length())
		return (0);
	int	i = -1;
	int	len = s1.length();
	while (++i < len)
	{
//		std::cout << "s1[" << i << "]: " << s1[i] << " - s2[" << i << "]: " << s2[i] << std::endl;
		if (s1[i] >= 65 && s1[i] <= 90)
		{
			if (s1[i] == s2[i] - 32)
				continue;
		}
		else if (s1[i] >= 97 && s1[i] <= 122)
		{
//			std::cout << ""
			if (s1[i] == s2[i] + 32)
				continue;
		}
		if (s1[i] != s2[i])
			return (0);
	}
	return (1);
}



int		ConnectionClass::_findAndParseContentHeaders(HttpRequest& currentRequest, std::pair<std::string, std::string> const& header)
{
	if (_caseInsensitiveComparison(header.first, "Transfer-Encoding"))
	{
		std::cout << "the request has ther TE header" << std::endl;
		ft_strsplit_and_trim(header.second, currentRequest.getModifyableTE());
		print_vec(currentRequest.getModifyableTE());
		currentRequest.setHasTE(1);
		if (_caseInsensitiveComparison(currentRequest.getModifyableTE().back(), "Chunked"))
		{
			currentRequest.setIsChunked(1);
			currentRequest.setContentLength(0);
			currentRequest.setHasContent(1);
			std::cout << "the request is chunked" << std::endl;
			return (1);
		}
		else
			return (_invalidRequestProcedure(currentRequest, 400));
	}
	if (_caseInsensitiveComparison(header.first, "Content-Length"))
	{
		if (header.second.find_first_not_of("0123456789") != header.second.npos)
			return (_invalidRequestProcedure(currentRequest, 400));
//		std::cout << " PASSED FIRT CHECK" << std::endl;
		long nbred = strtol(header.second.c_str(), NULL, 10);
		if (currentRequest.getContentLength())
		{
			if (currentRequest.getContentLength() != nbred)
				return (_invalidRequestProcedure(currentRequest, 400));
		}
		else
		{
			if (currentRequest.HasTE())
				return (0);
			currentRequest.setContentLength(nbred);
		}
		currentRequest.setHasContent(1);
		return (1);
	}
	return (0);
}

int		ConnectionClass::_parseHeaderLine(const char *line, int len, HttpRequest& currentRequest)
{
	int	index = 0;
	int	deb_value;
	int	end_value;

	std::pair<std::string, std::string>	header;
	
	while (line[index] != ':')
	{
		if (line[index] == ' ')
			return (_invalidRequestProcedure(currentRequest, 400));
		index++;
		if (index == len)
			return (_invalidRequestProcedure(currentRequest, 400));
	}
	header.first.append(line, index);
	index++;
	if (index == len)
		return (_invalidRequestProcedure(currentRequest, 400));
	while (line[index] == ' ') // je vire les espaces au début
	{
		index++;
		if (index == len)
			return (_invalidRequestProcedure(currentRequest, 400));
	}
	deb_value = index;
	index = len - 1;
//	std::cout << "line[" << index << "]: " << line[index] << std::endl;
	while (line[index] == ' ') //je vire les espaces à la fin
	{
//		std::cout << "line[" << index << "]: " << line[index] << std::endl;
		index--;
	}
	end_value = index + 1;
	header.second.append(&(line[deb_value]), end_value - deb_value);

//	std::cout << std::endl;
//	std::cout << "I REACHED CONTENT LENGTH PART" << std::endl;
//	std::cout << std::endl;
	if (_findAndParseContentHeaders(currentRequest, header) == -1)
		return (-1);

/*	if (_caseInsensitiveComparison(header.first, "Content-Length"))
	{
//		std::cout << "header.first: " << header.first << std::endl;
//		std::cout << "header.second: -" << header.second << "-" << std::endl;
		if (header.second.find_first_not_of("0123456789") != header.second.npos)
			return (_invalidRequestProcedure(currentRequest, 400));
//		std::cout << " PASSED FIRT CHECK" << std::endl;
		long nbred = strtol(header.second.c_str(), NULL, 10);
		if (currentRequest.getContentLength())
		{
			if (currentRequest.getContentLength() != nbred)
				return (_invalidRequestProcedure(currentRequest, 400));
		}
		else
			currentRequest.setContentLength(nbred);
//		std::cout << "ascii content length: " << header.second << " - long content-length: " << currentRequest.getContentLength() << std::endl;
	}*/
	currentRequest.addHeader(header);

	return (1);
	


}

int		ConnectionClass::_parse_line(const char *line, int len, HttpRequest &currentRequest)
{
	std::string	test_string(line, len);

//	std::cout << "the line passed to the parser is: " << test_string << std::endl;
	if (currentRequest.getLineCount() == 0)
		return (_parse_first_line(line, len, currentRequest));
	else
		return (_parseHeaderLine(line, len, currentRequest));
	return (1);
}

//int		ConnectionClass::_read_line_no_read()

int		ConnectionClass::_read_line(readingBuffer& buffer, int& length_parsed, HttpRequest& currentRequest, bool no_read_mode)
{
	int		crlf_index;
	int		read_ret;

	int deb_read = buffer.deb;
//	buffer.line_deb = buffer.deb;
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
//		std::cout << " _read_line returned 2 because it considers it read the whole header part" << std::endl;
		buffer.deb += 2;
		return (2);
	}
	while ((crlf_index = _findInBuf("\r\n", buffer.buf, 2, buffer.end, deb_read)) == -1)
	{
//		std::cout << std::endl;
//		std::cout << "crlf was not found in buffer" << std::endl;
//		_printBufferInfo(buffer, "in _read_line, before ifs");
		if (no_read_mode == NO_READ_MODE_ACTIVATED)
		{
//			std::cout << "_Read_line returns save_request" << std::endl;
			return (SAVE_REQUEST); // j'interrompt le processus de lecture pour empêcher un client pipeliner de monopoliser le serveur.
		}
		if ((buffer.end + SINGLE_READ_SIZE) <  READING_BUF_SIZE) // je vérifie que j'ai de la place dans mon buffer
		{
//			std::cout << "there is room for another read" << std::endl;
//			_printBufferInfo(buffer, "in _read_line,1st condition");

			/* code duplication with next condition */
			read_ret = recv(_socketNbr, &(buffer.buf[buffer.end]), SINGLE_READ_SIZE, 0);
//			std::cout << "read_ret: " << read_ret << std::endl;
			if (read_ret == -1)
				return (-1);
			if (read_ret == 0)
				return (0);
			if (deb_read)
				deb_read = buffer.end - 1; // j'enleve 1 au cas ou crlf est coupé en 2 ; a protéger segfault			
			buffer.end += read_ret;
			length_parsed += read_ret;
		}
		else if (buffer.deb > (READING_BUF_SIZE / 2)) // je regarde si ça vaut le coup de faire le memmove
		{
//			std::cout << "there is no room for another read, but we can memmove" << std::endl;
//			_printBufferInfo(buffer, "before memmove");
			std::memmove(buffer.buf, &(buffer.buf[buffer.deb]), buffer.end - buffer.deb); // le -1 sert a gérer crlf coupé en 2 encore une fois
			buffer.end -= buffer.deb;
			buffer.deb = 0;
			deb_read = buffer.end - 1;
//			_printBufferInfo(buffer, "after memmove");

			/*code duplication with previous condition */
			read_ret = recv(_socketNbr, &(buffer.buf[buffer.end]), SINGLE_READ_SIZE, 0);
			if (read_ret == -1)
				return (-1);
			if (read_ret == 0)
				return (0);
			if (deb_read)
				deb_read = buffer.end - 1; // j'enleve 1 au cas ou crlf est coupé en 2 ; a protéger segfault			
			buffer.end += read_ret;
			length_parsed += read_ret;
		}
		else
		{
//			std::cout << "we need to launch long line procedure: " << std::endl;
//			_printBufferInfo(buffer, "before long_line procedure");

			std::string	long_request_string;
			int		long_line_length;

			long_request_string.append(buffer.buf, buffer.deb, buffer.end - buffer.deb);
			read_ret = _read_long_line(long_request_string, buffer, length_parsed);
			if (read_ret == -1)
				return (-1);
			if (read_ret == 0)
				return (0);
			long_line_length = long_request_string.length();
//			std::cout << "line optained through long line procedure: " << long_request_string << std::endl;
//			std::cout << "line length: " << long_line_length << std::endl;
			if (_parse_line(long_request_string.c_str(), long_line_length, currentRequest) == -1)
				return (HTTP_ERROR);
//			buffer.deb = 0;
//			buffer.end = 0;
			deb_read = 0;
//			_printBufferInfo(buffer, "after long line procedure");
			return (1);
		}
	}
//	std::cout << "out of main loop, we found a line" << std::endl;
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
//		std::cout << " _read_line returned 2 because it considers it read the whole header part" << std::endl;
		buffer.deb = crlf_index + 2;
		return (2);
	}
	if (_parse_line(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb, currentRequest) == -1)
		return (HTTP_ERROR);
//	_printBufferInfo(buffer, "after main loop and line found");
	buffer.deb = crlf_index + 2;
	return (1);
}

int		ConnectionClass::_check_header_compliancy(HttpRequest& CurrentRequest)
{
	(void)CurrentRequest; //a supprimer
//	std::cout << "_check_header_compliancy has not been implemented yet. it always returns 1" << std::endl;
	return (1);
}

/*int		ConnectionClass::_guaranteedRead(int fd, int to_read, std::string& str_buffer)
{
	int 	read_ret;
	int	bytes_read = 0;
	int	bytes_left = to_read;
	char	*buffer;

//	str_buffer.reserve(to_read);
	if (!(buffer = (char*)malloc(sizeof(char) * to_read + 1)))
		exit(EXIT_FAILURE);
	while (bytes_left)
	{
		// POTENTIAL BLOCK HERE IF CONTENT-LENGTH HIGHER THAN CONTENT AND BLCOKING FDS
		read_ret = recv(fd, &(buffer[bytes_read]), bytes_left, 0);
		if (read_ret == -1)
		{
			free(buffer);
			buffer = NULL;
			return (-1);
		}
		else if (read_ret == 0)
		{
			free(buffer);
			buffer = NULL;
			return (0);
		}
		bytes_left -= read_ret;
		bytes_read += read_ret;
	}
	buffer[bytes_read] = '\0';
	str_buffer.append(buffer);
	free(buffer);
	buffer = NULL;
	return (bytes_read);
}
*/
int		ConnectionClass::_guaranteedRead(int fd, int to_read, std::string& str_buffer)
{
	int 	read_ret;
	int		bytes_read = 0;
	int		bytes_left = to_read;
	char	buffer[to_read + 1];

	while (bytes_left)
	{
		/** POTENTIAL BLOCK HERE IF CONTENT-LENGTH HIGHER THAN CONTENT AND BLCOKING FDS **/
		read_ret = recv(fd, &(buffer[bytes_read]), bytes_left, 0);
		if (read_ret == -1)
		{
			return (-1);
		}
		else if (read_ret == 0)
		{
			return (0);
		}
		bytes_left -= read_ret;
		bytes_read += read_ret;
	}
	buffer[bytes_read] = '\0';
	str_buffer.append(buffer);
	return (bytes_read);
}

int		ConnectionClass::_read_chunked_line(readingBuffer& buffer, int& length_parsed, HttpRequest& currentRequest, int read_size, std::string& line)
{
	int		crlf_index;
	int		read_ret;
	int 		deb_read;

	int deleteThisDebuggingShit = currentRequest.isValid();
	deleteThisDebuggingShit += 1;
	deb_read = buffer.deb;
	while ((crlf_index = _findInBuf("\r\n", buffer.buf, 2, buffer.end, deb_read)) == -1)
	{
		if ((buffer.end + read_size) <  READING_BUF_SIZE) // je vérifie que j'ai de la place dans mon buffer
		{
			read_ret = recv(_socketNbr, &(buffer.buf[buffer.end]), read_size, 0);
			if (read_ret == -1)
				return (-1);
			if (read_ret == 0)
				return (0);
			if (deb_read)
				deb_read = buffer.end - 1; // j'enleve 1 au cas ou crlf est coupé en 2 ; a protéger segfault			
			buffer.end += read_ret;
			length_parsed += read_ret;
		}
		else if (buffer.deb > (READING_BUF_SIZE / 2)) // je regarde si ça vaut le coup de faire le memmove
		{
			std::memmove(buffer.buf, &(buffer.buf[buffer.deb]), buffer.end - buffer.deb); // le -1 sert a gérer crlf coupé en 2 encore une fois
			buffer.end -= buffer.deb;
			buffer.deb = 0;
			deb_read = buffer.end - 1;
			read_ret = recv(_socketNbr, &(buffer.buf[buffer.end]), read_size, 0);
			if (read_ret == -1)
				return (-1);
			if (read_ret == 0)
				return (0);
			if (deb_read)
				deb_read = buffer.end - 1; // j'enleve 1 au cas ou crlf est coupé en 2 ; a protéger segfault			
			buffer.end += read_ret;
			length_parsed += read_ret;
		}
		else
		{
			int		long_line_length;

			line.append(buffer.buf, buffer.deb, buffer.end - buffer.deb);
			read_ret = _read_long_line(line, buffer, length_parsed);
			if (read_ret == -1)
				return (-1);
			if (read_ret == 0)
				return (0);
			long_line_length = line.length();
			deb_read = 0;
			return (1);
		}
	}
/*	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb = crlf_index + 2;
		return (2);
	}*/
	line.clear();
	line.append(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb);
	std::cout << "line found by chunk is: " << line << std::endl;
//	_printBufferInfo(buffer, "after main loop and line found");
	buffer.deb = crlf_index + 2;
	return (1);
}

/* int		ConnectionClass::_getChunkedData(HttpRequest& currentRequest, int fd, readingBuffer& buffer, int& length_parsed)
{

} */

int		ConnectionClass::_read_request_content(HttpRequest& CurrentRequest, readingBuffer& buffer ,int& length_parsed)
{

	std::string	request_content;
	int		to_read;
	int 		read_ret;
//	buffer.deb = buffer.deb + 1 - 1; // a virer
//	std::cout << "_read_request_content has not been implemented yet. it always returns 1" << std::endl;

//	std::cout << "reading request content " << std::endl;
//	_printBufferInfo(buffer, "in reader");
	/** PAS OUBLIER DE GERER BUF.DEB ET BUF.END **/
	if (buffer.end > buffer.deb)
	{
		int diff = buffer.end - buffer.deb;
		if (diff >= CurrentRequest.getContentLength())
		{
			request_content.append(&(buffer.buf[buffer.deb]), CurrentRequest.getContentLength());
			CurrentRequest.setContent(request_content);
			buffer.deb += CurrentRequest.getContentLength();
//			std::cout << "request_content: " << request_content << std::endl;
//			std::cout << ": " << request_content << std::endl;
//			_printBufferInfo(buffer, "after read_content");
			return (request_content.length());
		}
		else
		{
			request_content.append(&(buffer.buf[buffer.deb]), diff);
			buffer.deb += diff;
			to_read = CurrentRequest.getContentLength() - diff;
			read_ret = _guaranteedRead(_socketNbr, to_read, request_content);
			length_parsed += read_ret;
			if (read_ret == 0 || read_ret == 1)
				return (read_ret);
			else
			{
				CurrentRequest.setContent(request_content);
				return (request_content.length());
			}
		}
	}
	else 
	{
		read_ret = _guaranteedRead(_socketNbr, CurrentRequest.getContentLength(), request_content);
		length_parsed += read_ret;
		if (read_ret == 0 || read_ret == 1)
			return (read_ret);
		else
		{
			CurrentRequest.setContent(request_content);
			return (request_content.length());
		}
	}
	return (1);

}

void		ConnectionClass::_save_only_buffer(readingBuffer& buffer)
{
	_restBuffer = new std::string(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
	_hasRest = BUFF_REST;
}

void		ConnectionClass::_save_request_and_buffer(HttpRequest& currentRequest, readingBuffer& buffer)
{
	_incompleteRequest = new HttpRequest(currentRequest);
	_restBuffer = new std::string(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
	_hasRest = REQUEST_AND_BUFF_REST;
}

int		ConnectionClass::_get_next_request(readingBuffer &buffer, HttpRequest& currentRequest, int& length_parsed, bool no_read_mode)
{
	int	ret_read_line;
	int	ret_read_content;	
//	int	line_count = 0;
	//faire une protection contre segfaulkt/buffer overflow ici

//	std::cout << "inside get_next_request. no_read mode = "<< no_read_mode << std::endl;
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb += 2;
		buffer.deb += 2;
		length_parsed += 2;
	}
	// ici aussi:
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
//		std::cout << "need to handle an invalid request in _get_next_request" << std::endl;
		return (-1); // non, il faudrait autre chose
	}
	while ((ret_read_line = _read_line(buffer, length_parsed, currentRequest, no_read_mode)) == 1)
	{
		currentRequest.incrementLineCount();
//		std::cout << "_read_line returned 1, we're supposed to have read a line" << std::endl;
		if (currentRequest.getLineCount() > MAX_HEAD_LINES)
		{
			std::cout << "Too many header lines, need to send a bad request. for now, _get_next_request returns -1" 
				<< std::endl;
			return (-1);
		}
	}
	if (ret_read_line == -1)
	{
		std::cout << "_read_line returned -1" << std::endl;
		return (-1);
	}
	else if (ret_read_line == 0)
	{
		std::cout << "the whole connection has been processed" << std::endl;
		return (0);
	}
	else if (ret_read_line == HTTP_ERROR)
	{
		std::cout << "error found during parsing, get_next_request returns HTTP_ERROR" << std::endl;
		return (HTTP_ERROR);
	}
	if (ret_read_line == 2)
	{
//		currentRequest.printHeaders();
		if (_check_header_compliancy(currentRequest) == -1)
		{
			std::cout << "headers are not compliant, need to setup a bad request procedure. for now, function returns -1" << std::endl;
			return (-1);
		}
		if (currentRequest.getContentLength())
		{
			ret_read_content = _read_request_content(currentRequest, buffer, length_parsed);
			if (ret_read_content == -1 || ret_read_content == 0)
			{
				std::cout << "ret_read_content returned -1 OR 0, meaning an error occured. For now, get_next_request forwards this ret_value" << std::endl;
				return (ret_read_content);
			}
		}
		return (1);
	}
	if (ret_read_line == SAVE_REQUEST)
	{
		_save_request_and_buffer(currentRequest, buffer);
//		std::cout << "rest buffer: " << *_restBuffer << std::endl;
		return (SAVE_REQUEST);
	}
	std::cout << "unexpected return in _get_next_request" << std::endl;
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

	_initializeBuffer(buffer);
//	std::cout << "connection class with socket " << _socketNbr << "is in recieve request" << std::endl;
/*	if (_hasRest)
	{
		std::cout << "there is rest!" << std::endl;
		std::memmove(buffer.buf, _restBuffer->c_str(), _restBuffer->length());
		buffer.end = _restBuffer->length();
		delete _restBuffer;
		requestPipeline.push_back(*_incompleteRequest);
		delete _incompleteRequest;
		_hasRest = 0;
	}*/
/*	read_ret = recv(_socketNbr, buffer.buf, SINGLE_READ_SIZE, 0);
	std::cout << "buffer received by read: " << std::endl;
	if (read_ret == -1)
	{	
		perror("read"); // a faire partout pour le debugging?
		return (-1);
	}
	if (read_ret == 0)
		return (0);
	buffer.deb = 0;	
	buffer.end = read_ret;
	buffer.buf[read_ret] = '\0';*/
	read_ret = _read_buffer(buffer, requestPipeline);
	if (read_ret == HTTP_ERROR)
	{
		std::cout << "an invalid request has been detected" << std::endl;
		return (HTTP_ERROR);
	}
	else if (read_ret == TCP_ERROR)
	{
		std::cout << "a TCP error has occured" << std::endl;
		return (TCP_ERROR);
	}
	else if (read_ret == 0)
	{
		std::cout << "the connection has been closed" << std::endl;
		return (0);
	}

	return (1);
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
