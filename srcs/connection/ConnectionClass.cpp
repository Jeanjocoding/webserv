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
	_hasRest = 0;
	_isPersistent = 1;
	return;
}

ConnectionClass::ConnectionClass(ConnectionClass const& to_copy): _socketNbr(to_copy._socketNbr), _server(to_copy._server), _status(to_copy._status), _isPersistent(to_copy._isPersistent), _hasRest(to_copy._hasRest)
{
	if (to_copy._hasRest)
	{
		_restBuffer = new std::string(*(to_copy._restBuffer));
		if (to_copy._hasRest == REQUEST_AND_BUFF_REST)
			_incompleteRequest = new HttpRequest(*(to_copy._incompleteRequest));
	}
	_isPersistent = to_copy._isPersistent;
	return;	
}

ConnectionClass::ConnectionClass(int socknum, serverClass* server): _socketNbr(socknum), _server(server)
{
	_status = CO_ISOPEN;
	_hasRest = 0;
	_isPersistent = 1;
	return;	
}

ConnectionClass::~ConnectionClass(void)
{
	if (_hasRest)
	{
		delete _restBuffer;
		if (_hasRest == REQUEST_AND_BUFF_REST)
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
		if (_hasRest == REQUEST_AND_BUFF_REST)
			_incompleteRequest = new HttpRequest(*(to_copy._incompleteRequest));
	}
	return (*this);
}


int				ConnectionClass::_findInBuf(std::string to_find, char *buf, int findlen, int index_end, int begsearch)
{
	int		i = begsearch;
	int 	j = 0;
	std::string	debug_string(buf, begsearch, index_end - begsearch);
	while (i < index_end)
	{
		while (buf[i] == to_find[j])
		{
			if (j == (findlen - 1))
			{
				return (i - 1);
			}
			i++;
			j++;
		}
		if (j)
			j = 0;
		i++;
	}
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
//	std::cout << "full buffer: " << buffer.buf << std::endl;
	std::cout << "buffer.deb: " << buffer.deb << std::endl;;
	std::cout << "buffer.end: " << buffer.end << std::endl;
	std::string str_buffer(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
	std::cout << "buffer from buf.deb to buf.end: " << str_buffer << std::endl;;
	std::cout << "--------------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
}

/** This function is like the "main" of the request reading/parsing procedure. The main idea
 * is that it reads a big buffer, parses all the requests contained in this buffer and addds
 * them to the requestpipeline given in parameter. 
 * - if there is not a full request on the buffer: keep reading on the socket until one is found 
 * 	or one of the size limits is reached
 * - if after reading and parsing all the requests on the buffer, there is an incomplete request left:
 * 	save the request object and the rest of the buffer for next select round. the rest will be added
 * 	to the buffer before starting to parse.
 * 
 **/

int		ConnectionClass::_read_buffer(readingBuffer& buffer, std::vector<HttpRequest>& requestPipeline)
{
	int	length_parsed = 0;
	int	getnr_ret = 0;
	int	req_count = 0;
	int read_ret;

	HttpRequest	currentRequest;

	if (_hasRest) // select the saved request as the current request and add the saved buffer to the current buffer
	{
		std::memmove(buffer.buf, _restBuffer->c_str(), _restBuffer->length());
		buffer.end = _restBuffer->length();
		length_parsed += _restBuffer->length();
		delete _restBuffer;
		if (_hasRest == REQUEST_AND_BUFF_REST)
		{
			currentRequest = *_incompleteRequest;
			delete _incompleteRequest;
		}
		_hasRest = 0;
	}

	read_ret = recv(_socketNbr, &(buffer.buf[buffer.end]), SINGLE_READ_SIZE, 0);
	if (read_ret == -1)
	{	
		perror("read"); // a faire partout pour le debugging?
		return (-1);
	}
	if (read_ret == 0)
		return (0);
	buffer.end += read_ret;
	length_parsed += read_ret;
	buffer.buf[buffer.end] = '\0';

	/* parsing all the requests contained in one single read **/
	while ((length_parsed < SINGLE_READ_SIZE && buffer.deb < buffer.end) || req_count == 0) // je chope toutes les requêtes qui sont dans le buffer
	{
//		_printBufferInfo(buffer, "before gnr: ");
		getnr_ret = _get_next_request(buffer, currentRequest, length_parsed, NO_READ_MODE_DISABLED);
		if (getnr_ret == -1)
			return (-1);
		if (getnr_ret == 0)
			return (0);
		requestPipeline.push_back(currentRequest);
		if (getnr_ret == HTTP_ERROR)
			return (HTTP_ERROR);
		else
			requestPipeline.back().setValidity(1);
		req_count++;
		if (!_isPersistent)
			return (1);
		currentRequest.clear();
	}
	if (buffer.deb >= buffer.end)
	{
		return (1);
	}
	/** here, I read what is left on the buffer but I don't read on the socket anymore, 
	 * if the request is not full, i save it and take it back nect select iteration **/
	getnr_ret = _get_next_request(buffer, currentRequest, length_parsed, NO_READ_MODE_ACTIVATED);
	if (getnr_ret == -1)
		return (-1);
	else if (getnr_ret == 0)
		return (0);
	else if (getnr_ret == SAVE_REQUEST)
		return (1);
	requestPipeline.push_back(currentRequest);
	if (getnr_ret == HTTP_ERROR)
		return (HTTP_ERROR);
	else
		requestPipeline.back().setValidity(1);
	if (buffer.deb < buffer.end)
	{
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

	while ((read_ret = recv(_socketNbr, buf, SINGLE_READ_SIZE, 0)) > 0)
	{
		buf[read_ret] = '\0';
		length_parsed += read_ret;
		str.append(buf);
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
	std::cout << "invalid request procedure is called. , persistence desactivated. headers:" << std::endl;
	currentRequest.printHeaders();
	std::cout << std::endl;
	currentRequest.setValidity(0);
	currentRequest.setErrorCode(errorCode);
	_isPersistent = 0;
	return (HTTP_ERROR);
}

int		ConnectionClass::_parseProtocol(HttpRequest& currentRequest, std::string& protocol)
{
	int	bigVersion = 0;
	int	smallVersion = 0;
	int 	index = 5;
	int	length = protocol.length();

	if (protocol.length() < 6)
		return (_invalidRequestProcedure(currentRequest, 400));
	if (protocol.find("HTTP/") != 0)
		return (_invalidRequestProcedure(currentRequest, 400));
	while (protocol[index] != '.')
	{
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
		if (!isdigit(protocol[index]))
			return (_invalidRequestProcedure(currentRequest, 400));
		smallVersion = smallVersion * 10 + protocol[index] - '0';
		index++;
	}
	currentRequest.setProtocolVersion(bigVersion, smallVersion);
	if (bigVersion < 1 || (bigVersion == 1 && smallVersion == 0))
		_isPersistent = 0;
	else
		_isPersistent = 1;
	return (1);
}

int		ConnectionClass::_parse_first_line(const char *line, int len, HttpRequest& currentRequest)
{
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
	if (_parseProtocol(currentRequest, protocol) == HTTP_ERROR)
		return (HTTP_ERROR);
	else
	{
		currentRequest.addRequestLine(method, target);
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
		if (s1[i] >= 65 && s1[i] <= 90)
		{
			if (s1[i] == s2[i] - 32)
				continue;
		}
		else if (s1[i] >= 97 && s1[i] <= 122)
		{
			if (s1[i] == s2[i] + 32)
				continue;
		}
		if (s1[i] != s2[i])
			return (0);
	}
	return (1);
}


/** parses headers related to content (Content-length and transfert-encoding) */
int		ConnectionClass::_findAndParseContentHeaders(HttpRequest& currentRequest, std::pair<std::string, std::string> const& header)
{
	if (_caseInsensitiveComparison(header.first, "Transfer-Encoding"))
	{
		ft_strsplit_and_trim(header.second, currentRequest.getModifyableTE());
//		print_vec(currentRequest.getModifyableTE());
		currentRequest.setHasTE(1);
		if (_caseInsensitiveComparison(currentRequest.getModifyableTE().back(), "Chunked"))
		{
			currentRequest.setIsChunked(1);
			currentRequest.setContentLength(0);
			currentRequest.setHasContent(1);
			return (1);
		}
		else
			return (_invalidRequestProcedure(currentRequest, 400));
	}
	else if (_caseInsensitiveComparison(header.first, "Trailer"))
	{
		ft_strsplit_and_trim(header.second, currentRequest.getModifyableTrailers());
//		print_vec(currentRequest.getModifyableTrailers());
		currentRequest.setHasTrailer(1);
		return (0);
	}
	else if (_caseInsensitiveComparison(header.first, "Content-Length"))
	{
		if (header.second.find_first_not_of("0123456789") != header.second.npos)
			return (_invalidRequestProcedure(currentRequest, 400));
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

int		ConnectionClass::_findAndParsePersistanceHeaders(HttpRequest& currentRequest, std::pair<std::string, std::string> const& header) 
{
	if (_caseInsensitiveComparison(header.first, "Connection"))
	{
		ft_strsplit_and_trim(header.second, currentRequest.getModifyableConnectionOptions());
		if (find_in_vec_insensitive(currentRequest.getModifyableConnectionOptions(), "close"))
			_isPersistent = 0;
		else if (find_in_vec_insensitive(currentRequest.getModifyableConnectionOptions(), "keep-alive"))
			_isPersistent = 1;
	}
	return (1);
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
	while (line[index] == ' ') //je vire les espaces à la fin
		index--;
	end_value = index + 1;
	header.second.append(&(line[deb_value]), end_value - deb_value);
	int find_ret = _findAndParseContentHeaders(currentRequest, header);
	if (find_ret == -1 || find_ret == HTTP_ERROR)
	{
		std::cout << "findAndparse returns an error" << std::endl;
		return (find_ret);
	}
	_findAndParsePersistanceHeaders(currentRequest, header);
	currentRequest.addHeader(header);
	return (1);
}

int		ConnectionClass::_parse_line(const char *line, int len, HttpRequest &currentRequest)
{
	std::string	test_string(line, len);

	if (currentRequest.getLineCount() == 0)
		return (_parse_first_line(line, len, currentRequest));
	else
		return (_parseHeaderLine(line, len, currentRequest));
	return (1);
}


/** It's a get_next_line, but very optimized as it almost never reallocates, 
 * and it very rarely do copy operations. For increased efficiency, the line is directly
 * sent to the parser and is not returned. Downside of these optilizations is more
 * reading/debugging complexity and less modularity. This code is more or less duplicated three times,
 * there are 3 different read_line. */
int		ConnectionClass::_read_line(readingBuffer& buffer, int& length_parsed, HttpRequest& currentRequest, bool no_read_mode)
{
	int		crlf_index;
	int		read_ret;

	int deb_read = buffer.deb;
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb += 2;
		return (2);
	}
	while ((crlf_index = _findInBuf("\r\n", buffer.buf, 2, buffer.end, deb_read)) == -1)
	{
		if (no_read_mode == NO_READ_MODE_ACTIVATED)
		{
			return (SAVE_REQUEST); // j'interrompt le processus de lecture pour empêcher un client pipeliner de monopoliser le serveur.
		}
		if ((buffer.end + SINGLE_READ_SIZE) <  READING_BUF_SIZE) // je vérifie que j'ai de la place dans mon buffer
		{
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
		else if (buffer.deb > (READING_BUF_SIZE / 2)) // je regarde si ça vaut le coup de faire le memmove
		{
			std::memmove(buffer.buf, &(buffer.buf[buffer.deb]), buffer.end - buffer.deb); // le -1 sert a gérer crlf coupé en 2 encore une fois
			buffer.end -= buffer.deb;
			buffer.deb = 0;
			deb_read = buffer.end - 1;
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
			std::string	long_request_string;
			int		long_line_length;

			long_request_string.append(buffer.buf, buffer.deb, buffer.end - buffer.deb);
			read_ret = _read_long_line(long_request_string, buffer, length_parsed);
			if (read_ret == -1)
				return (-1);
			if (read_ret == 0)
				return (0);
			long_line_length = long_request_string.length();
			if (_parse_line(long_request_string.c_str(), long_line_length, currentRequest) == HTTP_ERROR)
				return (HTTP_ERROR);
			deb_read = 0;
			return (1);
		}
	}
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb = crlf_index + 2;
		return (2);
	}
	if (_parse_line(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb, currentRequest) == HTTP_ERROR)
		return (HTTP_ERROR);
	buffer.deb = crlf_index + 2;
	return (1);
}

/** read or recv do not guarantee that all the bytes asked will be read. This one does, or it returns an error */
int		ConnectionClass::_guaranteedRead(int fd, int to_read, std::string& str_buffer)
{
	int 	read_ret;
	int		bytes_read = 0;
	int		bytes_left = to_read;
	char	buffer[to_read + 1];

	while (bytes_left)
	{
		/** POTENTIAL BLOCK HERE IF CONTENT-LENGTH HIGHER THAN CONTENT AND BLCOKING FDS */
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

/** more or less a duplication of _read_line, will maybe unify in a future but at least this saves 
 * a lot of if/else conditions and enforces the SRP principle */
int		ConnectionClass::_read_chunked_line(readingBuffer& buffer, int& length_parsed, int read_size, std::string& line)
{
	int		crlf_index;
	int		read_ret;
	int 		deb_read;

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
	line.clear();
	line.append(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb);
	buffer.deb = crlf_index + 2;
	return (1);
}

/** rather explicit title, the code is more or less duplicated with read_content but having one function 
 * for both would have also been a pain with if/elses, and less efficient */
int		ConnectionClass::_readAndAppendChunkBlock(HttpRequest& currentRequest, readingBuffer& buffer, int& length_parsed, int block_length)
{
	std::string	request_content;
	int		to_read;
	int 		read_ret;
	int		block_length_w_crlf = block_length + 2;

	/** PAS OUBLIER DE GERER BUF.DEB ET BUF.END **/
	if (buffer.end > buffer.deb)  // determin if there is unporocessed data remaining in the buffer
	{
		int diff = buffer.end - buffer.deb; 
		if (diff >= block_length_w_crlf) // determins if the whole chunk is in the buffer
		{
			request_content.append(&(buffer.buf[buffer.deb]), block_length);
			currentRequest.appendToContent(request_content);
			buffer.deb += block_length_w_crlf;
			return (request_content.length());
		}
		else
		{
			if (diff <= block_length)  // ne prend pas en compte le crlf, cst la diff avec au dessus
				request_content.append(&(buffer.buf[buffer.deb]), diff);
			else
				request_content.append(&(buffer.buf[buffer.deb]), block_length);
			buffer.deb += diff; // allows not adding twice the same data (is it really relevant? because buffer not used by guar_Read)
			to_read = block_length_w_crlf - diff; 
			read_ret = _guaranteedRead(_socketNbr, to_read, request_content);
			buffer.deb = buffer.end;  // PERMET DE MONTRER QUE TOUT A ETE PROCESS
			if (read_ret == 0 || read_ret == -1)
				return (read_ret);
			else
			{
				length_parsed += read_ret;
				if (request_content.length() > 2)
				{
					request_content.resize(request_content.length() - 2); // je n'ajoute pas les crlfs
					currentRequest.appendToContent(request_content);
				}
				return (request_content.length());
			}
		}
	}
	else 
	{
		read_ret = _guaranteedRead(_socketNbr, block_length_w_crlf, request_content);
		buffer.deb = buffer.end;  // PERMET DE MONTRER QUE TOUT A ETE PROCESS
		if (read_ret == 0 || read_ret == -1)
			return (read_ret);
		else
		{
			length_parsed += read_ret;
			request_content.resize(request_content.length() - 4); // je n'ajoute pas les crlfs
			currentRequest.appendToContent(request_content);
			return (request_content.length());
		}
	}
	return (1);
}

/** sert a vider le buffer du dernier crlf et/ ou le lire sur le socket */
int		ConnectionClass::_processRemainingCrlf(readingBuffer& buffer)
{
	std::string	trash;
	int		left_inbuf = buffer.end - buffer.deb;
	int ret_read;

	if (left_inbuf < 2)
	{
		ret_read = _guaranteedRead(_socketNbr, 2 - left_inbuf, trash);
		if (ret_read == 0 || ret_read == -1)
			return (ret_read);	
	}
	buffer.deb = buffer.end;
	return (1);
	
}

/** this is the main for getting chunk sizes, reading on buffer or socket accordingly, and adding
 * it all to the content of the HttpRequest */
int		ConnectionClass::_getChunkedData(HttpRequest& currentRequest, readingBuffer& buffer, int& length_parsed)
{

	std::string	line_hex;
	int	read_ret;
	int 	ret_chunked;

	read_ret = _read_chunked_line(buffer, length_parsed, 10, line_hex);
	if (read_ret == -1)
		return (-1);
	else if (read_ret == 0)
		return (0);


	if (line_hex.find_first_not_of("0123456789ABCDEFabcdef") != line_hex.npos)
		return (_invalidRequestProcedure(currentRequest, 400));
	long nbred = strtol(line_hex.c_str(), NULL, 16);

	while (nbred > 0)
	{
		ret_chunked = _readAndAppendChunkBlock(currentRequest, buffer, length_parsed, nbred);
		if (ret_chunked == 0 || ret_chunked == -1)
			return (ret_chunked);
		read_ret = _read_chunked_line(buffer, length_parsed, 10, line_hex);
		if (read_ret == -1)
			return (-1);
		else if (read_ret == 0)
			return (0);
		if (line_hex.find_first_not_of("0123456789ABCDEFabcdef") != line_hex.npos)
			return (_invalidRequestProcedure(currentRequest, 400));
		nbred = strtol(line_hex.c_str(), NULL, 16);
	}
	return (currentRequest.getContent().length());
}

int		ConnectionClass::_read_request_content(HttpRequest& CurrentRequest, readingBuffer& buffer ,int& length_parsed)
{

	std::string	request_content;
	int		to_read;
	int 		read_ret;

	if (buffer.end > buffer.deb) // if part of the content is already in the buffer
	{
		int diff = buffer.end - buffer.deb;
		if (diff >= CurrentRequest.getContentLength()) // if all the content is already in the buffer, no need to read on the socket
		{
			request_content.append(&(buffer.buf[buffer.deb]), CurrentRequest.getContentLength());
			CurrentRequest.setContent(request_content);
			buffer.deb += CurrentRequest.getContentLength();
			return (request_content.length());
		}
		else // need to read on the socket, data missing
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
	else // all the content must be read from the socket
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

/** there is data remaining in the buffer but not enough to start parsing a new request so 
 * we will only need to copy the buffer when we go back */
void		ConnectionClass::_save_only_buffer(readingBuffer& buffer)
{
	_restBuffer = new std::string(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
	_hasRest = BUFF_REST;
}

/** when we run out of data while we were constructing an HttpRequest object, and we don't want 
 * to keep reading to avoid monopolizing cpu for only one connection, we save the request and 
 * the buffer and go back at processing them at next iteration */
void		ConnectionClass::_save_request_and_buffer(HttpRequest& currentRequest, readingBuffer& buffer)
{
	_incompleteRequest = new HttpRequest(currentRequest);
	_restBuffer = new std::string(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
	_hasRest = REQUEST_AND_BUFF_REST;
}

int		ConnectionClass::_findInTrailers(std::string& to_find, HttpRequest& currentRequest)
{
	size_t i = 0;
	std::vector<std::string>& trail_vec = currentRequest.getModifyableTrailers();

	while (i < trail_vec.size())
	{
		if (_caseInsensitiveComparison(to_find, trail_vec[i]))
			return (1);
		i++;
	}
	return (0);
}

/** Trailers are header that come after a chunked request. this function gets them */
int		ConnectionClass::_parseTrailerLine(const char *line, int len, HttpRequest& currentRequest)
{
	int	index = 0;
	int	deb_value;
	int	end_value;

	std::pair<std::string, std::string>	trailer;

	while (line[index] != ':')
	{
		if (line[index] == ' ')
			return (_invalidRequestProcedure(currentRequest, 400));
		index++;
		if (index == len)
			return (_invalidRequestProcedure(currentRequest, 400));
	}
	trailer.first.append(line, index);
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
	while (line[index] == ' ') //je vire les espaces à la fin
	{
		index--;
	}
	end_value = index + 1;
	trailer.second.append(&(line[deb_value]), end_value - deb_value);
	if (!_findInTrailers(trailer.first, currentRequest))
		return(_invalidRequestProcedure(currentRequest, 400));
	if (currentRequest.getTrailingHeaders().size() > (currentRequest.getModifyableTrailers().size() * 3))
		return(_invalidRequestProcedure(currentRequest, 400));
	currentRequest.addTrailingHeader(trailer);
	return (1);
}

/** last duplication of _read_line, but for trailers */ 
int		ConnectionClass::_read_line_trailer(readingBuffer& buffer, int& length_parsed, HttpRequest& currentRequest)
{
	int		crlf_index;
	int		read_ret;

	int deb_read = buffer.deb;
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb += 2;
		return (2);
	}
	while ((crlf_index = _findInBuf("\r\n", buffer.buf, 2, buffer.end, deb_read)) == -1)
	{
		if ((buffer.end + SINGLE_READ_SIZE) <  READING_BUF_SIZE) // je vérifie que j'ai de la place dans mon buffer
		{
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
		else if (buffer.deb > (READING_BUF_SIZE / 2)) // je regarde si ça vaut le coup de faire le memmove
		{
			std::memmove(buffer.buf, &(buffer.buf[buffer.deb]), buffer.end - buffer.deb); // le -1 sert a gérer crlf coupé en 2 encore une fois
			buffer.end -= buffer.deb;
			buffer.deb = 0;
			deb_read = buffer.end - 1;

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
			std::string	long_request_string;
			int		long_line_length;

			long_request_string.append(buffer.buf, buffer.deb, buffer.end - buffer.deb);
			read_ret = _read_long_line(long_request_string, buffer, length_parsed);
			if (read_ret == -1)
				return (-1);
			if (read_ret == 0)
				return (0);
			long_line_length = long_request_string.length();
			if (_parseTrailerLine(long_request_string.c_str(), long_line_length, currentRequest) == HTTP_ERROR)
				return (HTTP_ERROR);
			deb_read = 0;
			return (1);
		}
	}
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb = crlf_index + 2;
		return (2);
	}
	if (_parseTrailerLine(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb, currentRequest) == HTTP_ERROR)
		return (HTTP_ERROR);
	buffer.deb = crlf_index + 2;
	return (1);
}

/** read and parse trailers while they exist */ 
int		ConnectionClass::_readTrailers(readingBuffer& buffer, int& length_parsed, HttpRequest& currentRequest)
{
	int read_ret;

	read_ret = _read_line_trailer(buffer, length_parsed, currentRequest);
	if (read_ret == 0 || read_ret == -1 || read_ret == HTTP_ERROR)
		return (read_ret);
	while (read_ret == 1)
	{
		read_ret = _read_line_trailer(buffer, length_parsed, currentRequest);	
		if (read_ret == 0 || read_ret == -1 || read_ret == HTTP_ERROR)
			return (read_ret);
	}
	return (2);
}

/** get next request in the buffer or in the socket. if no_read_mode == NO_READ_MODE_ACTIVATED, it will 
 * not read on the socket after it runs out of data in the buffer, it will save everything and come back
 * next select iteration */ 
int		ConnectionClass::_get_next_request(readingBuffer &buffer, HttpRequest& currentRequest, int& length_parsed, bool no_read_mode)
{
	int	ret_read_line;
	int	ret_read_content;	

	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb += 2;
//		buffer.deb += 2;
//		length_parsed += 2;
	}

	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		return (_invalidRequestProcedure(currentRequest, 400));
	}
	while ((ret_read_line = _read_line(buffer, length_parsed, currentRequest, no_read_mode)) == 1)
	{
		currentRequest.incrementLineCount();
		if (currentRequest.getLineCount() > MAX_HEAD_LINES)
		{
			std::cout << "Too many header lines, need to send a bad request. for now, _get_next_request returns -1" 
				<< std::endl;
			return (_invalidRequestProcedure(currentRequest, 400));
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
	if (ret_read_line == 2) // this means we successfully read all the header part. we can now read body, if there is body
	{
		if (currentRequest.getContentLength())
		{
			ret_read_content = _read_request_content(currentRequest, buffer, length_parsed);
			if (ret_read_content == -1 || ret_read_content == 0)
			{
				std::cout << "ret_read_content returned -1 OR 0, meaning an error occured. For now, get_next_request forwards this ret_value" << std::endl;
				return (ret_read_content);
			}
		}
		else if (currentRequest.isChunked())
		{
			int chunk_ret = _getChunkedData(currentRequest, buffer, length_parsed);
			if (chunk_ret == 0 || chunk_ret == -1 || chunk_ret == HTTP_ERROR)
				return (chunk_ret);
			if (currentRequest.HasTrailers())
			{
				chunk_ret = _readTrailers(buffer, length_parsed, currentRequest);
			if (chunk_ret == 0 || chunk_ret == -1 || chunk_ret == HTTP_ERROR)
				return (chunk_ret);
			}
		}
		return (1);
	}
	if (ret_read_line == SAVE_REQUEST)
	{
		_save_request_and_buffer(currentRequest, buffer);
		return (SAVE_REQUEST);
	}
	std::cout << "unexpected return in _get_next_request" << std::endl; //debugging, should not happen
	return (1);
}

/** Read on the socket and fills the vector received in argument with parsed requests under the form
 * of HttpRequest objects */
int			ConnectionClass::receiveRequest(std::vector<HttpRequest>& requestPipeline)
{
	readingBuffer	buffer;
	int		read_ret;

	_initializeBuffer(buffer);
	read_ret = _read_buffer(buffer, requestPipeline); // fonction principale
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

int			ConnectionClass::sendResponse(std::string response)
{
	return (send(_socketNbr, response.c_str(), response.length(), 0));
}

/** Empties the reading buffer before closing the connection */
int				ConnectionClass::_emptyReadBuffers() const
{
	int read_ret;
	int total_read = 0;
	char trashbuf[EMPTYBUF_READ_SIZE];

	while ((read_ret = recv(_socketNbr, trashbuf, EMPTYBUF_READ_SIZE, 0)) == EMPTYBUF_READ_SIZE)
	{
		total_read += read_ret;
		if (total_read > MAX_READ_BEFORE_FORCE_CLOSE)
			return (FORCE_CLOSE_NEEDED);
	}
	if (read_ret == 0)
		return (0);
	else if (read_ret == -1)
	{
//		perror("recv");
		return (-1);
	}
	else
		return (1);
}

/** this function tries to implement a "graceful close", as it is described in the RFC.
 * it first shuts down the write end of the socket, then it reads on the socket until there 
 * isn't anything left, then it shuts down the read end, and then it closes the socket. This 
 * procedure is supposed to minimize the risk of TCP connection reset */
int				ConnectionClass::closeConnection(void)
{
	int empty_read_value;
	int return_value;

//	std::cout << "close connection is called" << std::endl;
	shutdown(_socketNbr, SHUT_WR);
//		perror("shutdown");
	empty_read_value = _emptyReadBuffers();
	shutdown(_socketNbr, SHUT_RD);
//		perror("shutdown");
	return_value = close(_socketNbr);
	if (return_value == 0)
		_status = CO_ISCLOSED;
	else
		perror("close");
	return (return_value);
}

int				ConnectionClass::getStatus(void)
{
	return (_status);
}

bool				ConnectionClass::isPersistent() const
{
	return (_isPersistent);
}
