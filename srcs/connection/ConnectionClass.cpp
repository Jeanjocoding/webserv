/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionClass.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/04 17:42:52 by asablayr          #+#    #+#             */
/*   Updated: 2021/09/10 16:23:44 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include "ConnectionClass.hpp"
#include "webserv.hpp"

ConnectionClass::ConnectionClass(void)
{
	_hasRestBuffer = 0;
	_hasRestRequest = 0;
	_hasBegRest = 0;
	_isHandlingBody = 0;
	_isParsingContent = 0;
	_ContentLeftToRead = 0;
	_isChunking = 0;
	_isReadingChunknbr = 0;
	_leftChunkedToRead = 0;
	_isPersistent = 1;
	_hasRead = 0;
	_isProcessingLastNL = 0;
	_isProcessingTrailers = 0;
	_timer = time(0);
	_hasToWriteOnPipe = 0;
	_hasToReadOnPipe = 0;
	_hasDoneCgi = 0;
	_cgiOutput_len = 0;
	_cgiOutput = 0;
	return;
}

ConnectionClass::ConnectionClass(ConnectionClass const& to_copy): _socketNbr(to_copy._socketNbr), _servers(to_copy._servers), _status(to_copy._status), _isPersistent(to_copy._isPersistent), _hasRestRequest(to_copy._hasRestRequest), _hasRestBuffer(to_copy._hasRestBuffer), _hasBegRest(to_copy._hasBegRest), _timer(to_copy._timer)
{
//	*this = to_copy;
	_isHandlingBody = to_copy._isHandlingBody;
	_isParsingContent = to_copy._isParsingContent;
	_ContentLeftToRead = to_copy._ContentLeftToRead;
	_isChunking = to_copy._isChunking;
	_isReadingChunknbr = to_copy._isReadingChunknbr;
	_leftChunkedToRead = to_copy._leftChunkedToRead;
	_hasRead = to_copy._hasRead;
	_isProcessingLastNL = to_copy._isProcessingLastNL;
	if (to_copy._hasRestBuffer)
		_restBuffer = new std::string(*(to_copy._restBuffer));
	if (to_copy._hasBegRest)
		_beginningRestBuffer = new std::string(*(to_copy._beginningRestBuffer));
	if (to_copy._hasRestRequest )
		_incompleteRequest = new HttpRequest(*(to_copy._incompleteRequest));
	_isPersistent = to_copy._isPersistent;
	_isProcessingTrailers = to_copy._isProcessingTrailers;
	_hasToWriteOnPipe = to_copy._hasToWriteOnPipe;
	_hasToReadOnPipe = to_copy._hasToReadOnPipe;
	_hasDoneCgi = to_copy._hasDoneCgi;
	_hasToWriteOnPipe = 0;
	_hasToReadOnPipe = 0;
	_hasDoneCgi = 0;
/*	if (_cgiOutput_len)
	{
		delete [] _cgiOutput;
		_cgiOutput_len = 0;
		_cgiOutput = 0;
	}*/
	_cgiOutput_len = 0;
	_cgiOutput = 0;
	if (to_copy._cgiOutput_len)
	{
		append_to_buffer(&_cgiOutput, _cgiOutput_len, to_copy._cgiOutput, to_copy._cgiOutput_len);
	}
	_request_pipeline = to_copy._request_pipeline;
	return;
}

ConnectionClass::ConnectionClass(int socknum, serverClass* server): _socketNbr(socknum)
{
	_status = CO_ISOPEN;
	_servers.push_back(server);
	_hasRestBuffer = 0;
	_hasRestRequest = 0;
	_hasBegRest = 0;
	_isHandlingBody = 0;
	_isParsingContent = 0;
	_ContentLeftToRead = 0;
	_isChunking = 0;
	_isReadingChunknbr = 0;
	_leftChunkedToRead = 0;
	_isPersistent = 1;
	_isProcessingLastNL = 0;
	_hasRead = 0;
	_isProcessingTrailers = 0;
	_timer = time(0);
	_hasToWriteOnPipe = 0;
	_hasToReadOnPipe = 0;
	_hasDoneCgi = 0;
	_cgiOutput_len = 0;
	_cgiOutput = 0;
	return;	
}

ConnectionClass::ConnectionClass(int socknum): _socketNbr(socknum)
{
	_status = CO_ISOPEN;
	_hasRestBuffer = 0;
	_hasRestRequest = 0;
	_hasBegRest = 0;
	_isHandlingBody = 0;
	_isParsingContent = 0;
	_ContentLeftToRead = 0;
	_isChunking = 0;
	_isReadingChunknbr = 0;
	_leftChunkedToRead = 0;
	_isPersistent = 1;
	_isProcessingLastNL = 0;
	_hasRead = 0;
	_isProcessingTrailers = 0;
	_timer = time(0);
	_hasToWriteOnPipe = 0;
	_hasToReadOnPipe = 0;
	_hasDoneCgi = 0;
	_cgiOutput_len = 0;
	_cgiOutput = 0;
	return;	
}

ConnectionClass::~ConnectionClass(void)
{
//	std::cout << "connection: destructor called" << std::endl;
	if (_hasRestBuffer)
		delete _restBuffer;
	if (_hasRestRequest)
		delete _incompleteRequest;
	if (_hasBegRest)
		delete _beginningRestBuffer;
	if (_cgiOutput_len)
	{
		delete [] _cgiOutput;
		_cgiOutput_len = 0;
		_cgiOutput = 0;
	}
	return;
}

ConnectionClass&	ConnectionClass::operator=(ConnectionClass const& to_copy)
{
	_socketNbr = to_copy._socketNbr;
	_status = to_copy._status;
	_servers = to_copy._servers;
	_isPersistent = to_copy._isPersistent;
	_hasRestBuffer = to_copy._hasRestBuffer;
	_hasRestRequest = to_copy._hasRestRequest;
	_hasBegRest = to_copy._hasBegRest;
	_hasRead = to_copy._hasRead;
	_isProcessingLastNL = to_copy._isProcessingLastNL;
	_isProcessingTrailers = to_copy._isProcessingTrailers;
	if (_hasRestBuffer)
		_restBuffer = new std::string(*(to_copy._restBuffer));
	if (_hasRestRequest)
		_incompleteRequest = new HttpRequest(*(to_copy._incompleteRequest));
	if (to_copy._hasBegRest)
		_beginningRestBuffer = new std::string(*(to_copy._beginningRestBuffer));
	_timer = to_copy._timer;
	_hasToWriteOnPipe = to_copy._hasToWriteOnPipe;
	_hasToReadOnPipe = to_copy._hasToReadOnPipe;
	_hasDoneCgi = to_copy._hasDoneCgi;	
	if (_cgiOutput_len)
	{
		delete [] _cgiOutput;
		_cgiOutput_len = 0;
		_cgiOutput = 0;
	}
	if (to_copy._cgiOutput_len)
	{
		append_to_buffer(&_cgiOutput, _cgiOutput_len, to_copy._cgiOutput, to_copy._cgiOutput_len);
	}
	_request_pipeline = to_copy._request_pipeline;
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

//	std::cout << "in read buffer" << std::endl;
	if (_hasRestRequest) // select the saved request as the current request and add the saved buffer to the current buffer
	{
		currentRequest = *_incompleteRequest;
		delete _incompleteRequest;
		_hasRestRequest = 0;
	}

	/** si je suis en train de gérer le body, je ne dois pas repasser par get_next_request, je reprend ici,
	 * cette série de if/else va permettre a l'algorithme de reprendre de la ou il en était dans le parsing 
	 */
	if (_isHandlingBody)
	{
		/** ici , je n'ai pas besoin de save de buffer, car je lirais toujours au maximum ce dont j'ai besoin
		 * car si je suis ici c'est que la elcture s'est interrompue au moment de lire le body */ 
//		_print_content_info(buffer, currentRequest, "in read buffer, beg isHandling");
		if (_isParsingContent)
		{
			read_ret = _read_request_content(currentRequest, buffer);
		}
		else
		{
			read_ret = _getChunkedData(currentRequest, buffer);
//			std::cout << "read_ret: " << read_ret << std::endl;
//			std::cout << "NEED TO HANDLE TRAILERS" << std::endl;

		}
		if (read_ret == -1 || read_ret == 0)
			return (read_ret);
		/** a partir de la, save le buffer redevient pertinenent car il ne s'agit plus de la gestion du "body" */

		/** sert a lire le dernier CRLF, c'est utile si on est en chunk encoding et il n'y a pas de trailer */
		if (_isProcessingLastNL) 
		{
//			std::cout << " I process last nl" << std::endl;
			read_ret = _last_nl_procedure(buffer);
			if (read_ret == 0 || read_ret == -1)
				return (read_ret);
			else if (read_ret == SAVE_REQUEST)
			{
				_save_request_and_buffer(currentRequest, buffer);
				return (1);
			}
			_isProcessingLastNL = 0;
//			return (1);
		}
		if (_isProcessingTrailers)
		{
			read_ret = _readTrailers(buffer, currentRequest);
			if (read_ret == 0 || read_ret == -1 || read_ret == HTTP_ERROR)
				return (read_ret);
			else if (read_ret == SAVE_REQUEST)
			{
				_save_request_and_buffer(currentRequest, buffer);
				return (SAVE_REQUEST);
			}
			_isProcessingTrailers = 0;
//			_print_content_info(currentRequest, "after trailers");
		}

		/** cela signifie que la requête a été completement lue, je peux push sur le pipeline */
		if (!_isHandlingBody)
		{
			if (currentRequest.isChunked())
				currentRequest.setContentLength(currentRequest.getCurrentContentLength());
			requestPipeline.push_back(currentRequest);
			if (read_ret != HTTP_ERROR)	
				requestPipeline.back().setValidity(1);
		}
//		_print_content_info(buffer, currentRequest, "in read buffer, after isHandling");
		if (!_hasRestBuffer && buffer.end > buffer.deb) // si j'ai lu tout le content mais qu'il reste du buffer
			_save_only_buffer(buffer);
		return (1); 
		
	}

	/** ces deux ifs (doublon par rapport à au-dessus) permettent également de reprendre au bon en droit dans l'éxécution de l'algorithme */
	if (_isProcessingTrailers)
	{
		read_ret = _readTrailers(buffer, currentRequest);
		if (read_ret == 0 || read_ret == -1 || read_ret == HTTP_ERROR)
			return (read_ret);
		else if (read_ret == SAVE_REQUEST)
		{
			_save_request_and_buffer(currentRequest, buffer);
			return (SAVE_REQUEST);
		}
		if (currentRequest.isChunked())
			currentRequest.setContentLength(currentRequest.getCurrentContentLength());
		requestPipeline.push_back(currentRequest);
		if (read_ret != HTTP_ERROR)	
			requestPipeline.back().setValidity(1);
		_isProcessingTrailers = 0;
		if (buffer.end > buffer.deb)
			_save_only_buffer(buffer);
		return (1);
	}
	if (_isProcessingLastNL)
	{
//		print_request(currentRequest);
		read_ret = _last_nl_procedure(buffer);
		if (read_ret == 0 || read_ret == -1)
			return (read_ret);
		else if (read_ret == SAVE_REQUEST)
		{
			_save_request_and_buffer(currentRequest, buffer);
			return (1);
		}
		if (currentRequest.isChunked())
			currentRequest.setContentLength(currentRequest.getCurrentContentLength());
		requestPipeline.push_back(currentRequest);
		if (read_ret != HTTP_ERROR)	
			requestPipeline.back().setValidity(1);
		_isProcessingLastNL = 0;
		return (1);
	}

	/** a partir de la commence la procédure "standard", qui commence par lire les headers pour former les requêtes.
	 * Si le read suivant lis l'ensemble du pipeline, ce qui devrait se passer la majeure partie du temps, la partie
	 * supérieure ne devrait pas être utile
	 */
	read_ret = recv(_socketNbr, &(buffer.buf[buffer.end]), SINGLE_READ_SIZE, 0);
	_hasRead = 1;
	if (read_ret == -1)
	{	
		perror("read in read_buffer"); // a faire partout pour le debugging?
		return (-1);
	}
	if (read_ret == 0)
		return (0);
	buffer.end += read_ret;
	length_parsed += read_ret;
	buffer.buf[buffer.end] = '\0';
	/* parsing all the requests contained in one single read **/
	while (buffer.deb < buffer.end) // je chope toutes les requêtes qui sont dans le buffer
	{
//		_printBufferInfo(buffer, "before gnr: ");
		getnr_ret = _get_next_request(buffer, currentRequest);
		if (getnr_ret == -1)
			return (-1);
		else if (getnr_ret == 0)
			return (0);
		else if (getnr_ret == SAVE_REQUEST)
			return (1); // EN SUIS-JE SUR ?
		if (currentRequest.isChunked())
			currentRequest.setContentLength(currentRequest.getCurrentContentLength());
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

//	std::cout << "string passed to _parse_line: " << test_string << std::endl;
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
int		ConnectionClass::_read_line(readingBuffer& buffer, HttpRequest& currentRequest)
{
	int		crlf_index;
//	int		read_ret;

//	std::cout << "in read_line" << std::endl;
//	std::cout << "has rest buffer: " << _hasRestBuffer <<  std::endl;
//	_printBufferInfo(buffer, "in read line");
	int deb_read = buffer.deb;

	/** A PROTEGER DES SEGFAULTS/BUGS SI JE SUIS AU BOUT DU BUFFER */
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n' && !_hasRestBuffer)
	{
		buffer.deb += 2;
		return (2);
	}
//	std::cout << "_hasRestBuffer: " << _hasRestBuffer << std::endl;
	if (_hasRestBuffer && _restBuffer->length() && (*_restBuffer)[_restBuffer->length() - 1] == '\r' 
		&& buffer.buf[buffer.deb] == '\n')
	{
		if (_restBuffer->length() == 1)
		{
			buffer.deb += 1;
			_hasRestBuffer = 0;
			_restBuffer->clear();
			delete _restBuffer;
			return (2);
		}
		if (_parse_line(_restBuffer->c_str(), _restBuffer->length() - 1, currentRequest) == HTTP_ERROR)
			return (HTTP_ERROR);
		buffer.deb += 1;
		_hasRestBuffer = 0;
		_restBuffer->clear();
		delete _restBuffer;
		return (1);
	}
	if ((crlf_index = _findInBuf("\r\n", buffer.buf, 2, buffer.end, deb_read)) == -1)
			return (SAVE_REQUEST); 
/*	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb = crlf_index + 2;
		return (2);
	}*/
	if (_hasRestBuffer)
	{
		_restBuffer->append(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb);
		if (_parse_line(_restBuffer->c_str(), _restBuffer->length(), currentRequest) == HTTP_ERROR)
			return (HTTP_ERROR);
		_restBuffer->clear();
		delete _restBuffer;
		_restBuffer = 0;
		_hasRestBuffer = 0;
	}
	else
	{
		if (_parse_line(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb, currentRequest) == HTTP_ERROR)
			return (HTTP_ERROR);
	}
	buffer.deb = crlf_index + 2;
//	_printBufferInfo(buffer, "end read_line");
	return (1);
}

/** read or recv do not guarantee that all the bytes asked will be read. This one does, or it returns an error */
/*int		ConnectionClass::_guaranteedRead(int fd, int to_read, std::string& str_buffer)
{
	int 	read_ret;
	int		bytes_read = 0;
	int		bytes_left = to_read;
	char	buffer[to_read + 1];

	while (bytes_left)
	{
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
}*/

int		ConnectionClass::_read_line_trailer(readingBuffer& buffer, HttpRequest& currentRequest)
{
	int		crlf_index;
//	int		read_ret;

//	std::cout << "in read_line" << std::endl;
//	std::cout << "has rest buffer: " << _hasRestBuffer <<  std::endl;
//	_printBufferInfo(buffer, "in read line");
	int deb_read = buffer.deb;

	/** A PROTEGER DES SEGFAULTS/BUGS SI JE SUIS AU BOUT DU BUFFER */
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n' && !_hasRestBuffer)
	{
		buffer.deb += 2;
		return (2);
	}
	if (_hasRestBuffer && _restBuffer->length() && (*_restBuffer)[_restBuffer->length() - 1] == '\r' 
		&& buffer.buf[buffer.deb] == '\n')
	{
		if (_restBuffer->length() == 1)
		{
			buffer.deb += 1;
			_hasRestBuffer = 0;
			_restBuffer->clear();
			delete _restBuffer;
			return (2);
		}
		if (_parseTrailerLine(_restBuffer->c_str(), _restBuffer->length() - 1, currentRequest) == HTTP_ERROR)
			return (HTTP_ERROR);
		buffer.deb += 1;
		_hasRestBuffer = 0;
		_restBuffer->clear();
		delete _restBuffer;
		return (1);
	}
	if ((crlf_index = _findInBuf("\r\n", buffer.buf, 2, buffer.end, deb_read)) == -1)
			return (SAVE_REQUEST); 
/*	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb = crlf_index + 2;
		return (2);
	}*/
	if (_hasRestBuffer)
	{
		_restBuffer->append(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb);
		if (_parseTrailerLine(_restBuffer->c_str(), _restBuffer->length(), currentRequest) == HTTP_ERROR)
			return (HTTP_ERROR);
		_restBuffer->clear();
		delete _restBuffer;
		_restBuffer = 0;
		_hasRestBuffer = 0;
	}
	else
	{
		if (_parseTrailerLine(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb, currentRequest) == HTTP_ERROR)
			return (HTTP_ERROR);
	}
	buffer.deb = crlf_index + 2;
//	_printBufferInfo(buffer, "end read_line");
	return (1);
}

int		ConnectionClass::_read_chunked_line(readingBuffer& buffer, std::string& line)
{
	int		crlf_index;
//	int		read_ret;

//	std::cout << "in read_chunked line" << std::endl;
//	std::cout << "has rest buffer: " << _hasRestBuffer <<  std::endl;
//	_printBufferInfo(buffer, "in read chunked line");
	int deb_read = buffer.deb;

	/** A PROTEGER DES SEGFAULTS/BUGS SI JE SUIS AU BOUT DU BUFFER */
	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n' && !_hasRestBuffer)
	{
		buffer.deb += 2;
		return (2);
	}
	if (_hasRestBuffer && _restBuffer->length() && (*_restBuffer)[_restBuffer->length() - 1] == '\r' 
		&& buffer.buf[buffer.deb] == '\n')
	{
		if (_restBuffer->length() == 1)
		{
			buffer.deb += 1;
			_hasRestBuffer = 0;
			_restBuffer->clear();
			delete _restBuffer;
			return (2);
		}
		line.append(_restBuffer->c_str(), _restBuffer->length() - 1);
		buffer.deb += 1;
		_hasRestBuffer = 0;
		_restBuffer->clear();
		delete _restBuffer;
		return (1);
	}
	if ((crlf_index = _findInBuf("\r\n", buffer.buf, 2, buffer.end, deb_read)) == -1)
			return (SAVE_REQUEST); 
/*	if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
	{
		buffer.deb = crlf_index + 2;
		return (2);
	}*/
	if (_hasRestBuffer)
	{
//		std::cout << "rest buffer before strategic append: " << *_restBuffer << std::endl;
		_restBuffer->append(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb);
//		std::cout << "rest buffer after strategic append: " << *_restBuffer << std::endl;
//		line.append(_restBuffer->c_str(), _restBuffer->length() - 1);
		line.append(_restBuffer->c_str(), _restBuffer->length());
		_restBuffer->clear();
		delete _restBuffer;
		_restBuffer = 0;
		_hasRestBuffer = 0;
	}
	else
		line.append(&(buffer.buf[buffer.deb]), crlf_index - buffer.deb);
	buffer.deb = crlf_index + 2;
	return (1);
}

/** rather explicit title, the code is more or less duplicated with read_content but having one function 
 * for both would have also been a pain with if/elses, and less efficient */
int		ConnectionClass::_readAndAppendChunkBlock(HttpRequest& currentRequest, readingBuffer& buffer)
{
	std::string	request_content;
//	int		to_read;
	int 		read_ret;
	int		size_append;

// pb: ne finis pas mais ne retourne pas save request visiblement

	/** PAS OUBLIER DE GERER BUF.DEB ET BUF.END **/
//	_print_content_info(buffer, currentRequest, "in read and append");
	if (buffer.end > buffer.deb)  // determin if there is unporocessed data remaining in the buffer
	{
		int diff = buffer.end - buffer.deb; 
		if (diff >= _ContentLeftToRead) // determins if the whole chunk is in the buffer
		{
			if (_ContentLeftToRead - 2)
			{
				currentRequest.appendToContent(&(buffer.buf[buffer.deb]), _ContentLeftToRead - 2);
			}
			buffer.deb += _ContentLeftToRead;
			_ContentLeftToRead = 0;
			return (1);
		}
		else
		{
			if (diff <= (_ContentLeftToRead - 2))  // ne prend pas en compte le crlf, cst la diff avec au dessus
			{
//				std::cout << "diff: " << diff << std::endl;
//				std::cout << "content before appending: " << currentRequest.getContent() << std::endl;
				currentRequest.appendToContent(&(buffer.buf[buffer.deb]), diff);
//				std::cout << "content after appending: " << currentRequest.getContent() << std::endl;
				_ContentLeftToRead -= diff;
//				buffer.deb += diff;
			}
			else
			{
				currentRequest.appendToContent(&(buffer.buf[buffer.deb]), _ContentLeftToRead - 2);
				_ContentLeftToRead = 1;
			}
			buffer.deb += diff; // allows not adding twice the same data (is it really relevant? because buffer not used by guar_Read)
//			buffer.deb = buffer.end;  // PERMET DE MONTRER QUE TOUT A ETE PROCESS
			if (!_hasRead)
			{
				if (_ContentLeftToRead < SINGLE_READ_SIZE)
					read_ret = recv(_socketNbr, &(buffer.buf[buffer.deb]), _ContentLeftToRead, 0);
				else
					read_ret = recv(_socketNbr, &(buffer.buf[buffer.deb]), SINGLE_READ_SIZE, 0);
				if (read_ret == 0 || read_ret == -1)
					return (read_ret);
				buffer.end += read_ret;
				_ContentLeftToRead -= read_ret;
				_hasRead = 1;
				int size_append = (buffer.end - buffer.deb -2);
				if (size_append > 0)
					currentRequest.appendToContent(&(buffer.buf[buffer.deb]), size_append);
				buffer.deb = buffer.end;  // PERMET DE MONTRER QUE TOUT A ETE PROCESS
				return (1);
			}
			else
			{
				return (_save_only_request(currentRequest));
			}
		}
	}
	else 
	{
		if (!_hasRead)
		{
			if (_ContentLeftToRead < SINGLE_READ_SIZE)
				read_ret = recv(_socketNbr, &(buffer.buf[buffer.deb]), _ContentLeftToRead, 0);
			else
				read_ret = recv(_socketNbr, &(buffer.buf[buffer.deb]), SINGLE_READ_SIZE, 0);
			if (read_ret == 0 || read_ret == -1)
				return (read_ret);
			buffer.end += read_ret;
			_hasRead = 1;
			if (read_ret == _ContentLeftToRead || read_ret == _ContentLeftToRead - 1)
				size_append = _ContentLeftToRead - 2;
			else
				size_append = read_ret;
			_ContentLeftToRead -= read_ret;
//			size_append = buffer.end - buffer.deb - 2;
			if (size_append > 0)
				currentRequest.appendToContent(&(buffer.buf[buffer.deb]), size_append);
			buffer.deb = buffer.end; //sur de sur?
			if (_ContentLeftToRead)
				return (_save_only_request(currentRequest));

		}
		else
		{
			return (_save_only_request(currentRequest)); // only request car normalement tout est aved dans request
		}
	}
	return (1);
}

/** sert a vider le buffer du dernier crlf et/ ou le lire sur le socket */
/*int		ConnectionClass::_processRemainingCrlf(readingBuffer& buffer)
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
	
}*/

int		ConnectionClass::_last_nl_procedure(readingBuffer& buffer)
{
	int		left_inbuf = buffer.end - buffer.deb;
	int ret_read;

//	std::cout << "processing last nl" << std::endl;
	if (_hasRestBuffer)
	{
//		std::cout << "rest buffer: " << *_restBuffer << std::endl;
		std::memmove(buffer.buf, _restBuffer->c_str(), _restBuffer->length());
		buffer.end += _restBuffer->length(); // cette ligne a résolu le bug
		left_inbuf = buffer.end - buffer.deb;
		delete _restBuffer;
		_restBuffer = 0;
		_hasRestBuffer = 0;
	}
//	_printBufferInfo(buffer, "in last nl deb");
	std::cout << "left inbuf: "  << left_inbuf << std::endl;
	if (left_inbuf < 2)
	{
		if (!_hasRead)
		{
			//besoin de protection?
			ret_read = recv(_socketNbr, &(buffer.buf[buffer.end]), 2 - left_inbuf, 0);
			buffer.end += ret_read;
			if (ret_read == 0 || ret_read == -1)
				return (ret_read);
			if (buffer.end - buffer.deb < 2)
				return (SAVE_REQUEST);
		}
		else
			return (SAVE_REQUEST);
	}
	_printBufferInfo(buffer, "before error test in last nl");
	if (buffer.buf[buffer.deb] != '\r' || buffer.buf[buffer.deb + 1] != '\n')
		return (HTTP_ERROR);
	buffer.deb += 2;
	_isProcessingLastNL = 0;
	return (1);	
}

/** this is the main for getting chunk sizes, reading on buffer or socket accordingly, and adding
 * it all to the content of the HttpRequest */
int		ConnectionClass::_getChunkedData(HttpRequest& currentRequest, readingBuffer& buffer)
{

	std::string	line_hex;
	int	read_ret;
	int 	ret_chunked;
	long	nbred;
	int 	last_reached = 0;
//	int	diff;

//	std::cout << "is in _getChunked" << std::endl;
//	_print_content_info(buffer, currentRequest, "in get chunked");
	if (!_ContentLeftToRead)
	{
		_isReadingChunknbr = 1;
		if (_hasRestBuffer || _hasRead == 0) // signifie que j'ai pas encore lu sur cette itération
		{
			read_ret = recv(_socketNbr, buffer.buf, 10, 0);
//			std::cout << "red ret after recv: " << read_ret << std::endl;
			if (read_ret == 0 || read_ret == -1)
				return (read_ret);
			buffer.end += read_ret;
//			_printBufferInfo(buffer, "after recv");
			_hasRead = 1;
			
		}
//		std::cout << "calling read cunk line here" << std::endl;
		read_ret = _read_chunked_line(buffer, line_hex);
//		std::cout << "line hex: " << line_hex << std::endl;
		if (read_ret == SAVE_REQUEST)
			return (_save_request_and_buffer(currentRequest, buffer));
		_isReadingChunknbr = 0;
		if (line_hex.find_first_not_of("0123456789ABCDEFabcdef") != line_hex.npos)
			return (_invalidRequestProcedure(currentRequest, 400));
		nbred = strtol(line_hex.c_str(), NULL, 16);
		line_hex.clear();
//		std::cout << "nbred not in loop" << nbred << std::endl;
		if (!nbred)
		{
			last_reached = 1;
			_ContentLeftToRead = 0;
			_isChunking = 0;
			_isHandlingBody = 0;
			if (!currentRequest.HasTrailers())
				_isProcessingLastNL = 1;
			else
				_isProcessingTrailers = 1;
			return (1);
		}
		_ContentLeftToRead = nbred + 2; // déduire buffer (je le fais problement déjà apres);
/*		if (nbred == 0)
			return (currentRequest.getContent().length());
		diff = buffer.end - buffer.deb;
		if (diff)
		{
			if (diff >= nbred)

		}*/
	}
	while (_ContentLeftToRead > 0) // boucle nécessaire car il faut que ça marche quand je parcours un long buffer
	{
//		_print_content_info(buffer, currentRequest, "in read and append");
		ret_chunked = _readAndAppendChunkBlock(currentRequest, buffer);
//		_print_content_info(buffer, currentRequest, "out of read and append");
//		std::cout << "ret reand and append: " << ret_chunked << std::endl;
		if (ret_chunked == 0 || ret_chunked == -1)
			return (ret_chunked);
		if (ret_chunked == SAVE_REQUEST)
			return (SAVE_REQUEST);
		if (last_reached == 1)
		{
			_ContentLeftToRead = 0;
			_isChunking = 0;
			_isHandlingBody = 0;
			if (!currentRequest.HasTrailers())
				_isProcessingLastNL = 1;
			else
				_isProcessingTrailers = 1;
			break;
		}
		_isReadingChunknbr = 1;
		read_ret = _read_chunked_line(buffer, line_hex);
		if (read_ret == SAVE_REQUEST)
			return (_save_request_and_buffer(currentRequest, buffer)); // ici, je dois sauvegarder requête car ça n'est pas fait dans la fonction
		_isReadingChunknbr = 0;
		if (line_hex.find_first_not_of("0123456789ABCDEFabcdef") != line_hex.npos)
			return (_invalidRequestProcedure(currentRequest, 400));
//		std::cout << "line_hexin loop: " << line_hex << std::endl;
		nbred = strtol(line_hex.c_str(), NULL, 16);
//		std::cout << "nbred in loop: " << nbred << std::endl;
		line_hex.clear();
		if (!nbred)
		{
			last_reached = 1;
			_ContentLeftToRead = 0;
			_isChunking = 0;
			_isHandlingBody = 0;
			if (!currentRequest.HasTrailers())
				_isProcessingLastNL = 1;
			return (1);
//			last_reached = 1;
//			break;
		}
		_ContentLeftToRead = nbred + 2;
	}
	return (currentRequest.getCurrentContentLength());
}

void		ConnectionClass::_print_content_info(readingBuffer& buffer, HttpRequest& currentRequest, std::string message)
{
	std::cout << " ------------------ CONTENT INFO: " << message << "-------------- " << std::endl;
	std::cout << "_hasRead: " << _hasRead << std::endl;
	std::cout << "_hasRestBuffer: " << _hasRestBuffer << std::endl;
	if (_hasRestBuffer)
		std::cout << "rest buffer: " << *_restBuffer << std::endl;
	std::cout << "_isHandlingBody: " << _isHandlingBody << std::endl;
	std::cout << "_isParsingContent: " << _isParsingContent << std::endl;
	std::cout << "_ContentLeftToRead: " << _ContentLeftToRead << std::endl;
	std::cout << "_isChunking: " << _isChunking << std::endl;
	std::cout << "_isReadingChunkNbr: " << _isReadingChunknbr << std::endl;
//:w
	std::cout << "_isParsingContent: " << _isParsingContent << std::endl;
	if (currentRequest.getCurrentContentLength())
	{
		std::cout << "content in request: ";
		write(1, currentRequest.getContent(), currentRequest.getCurrentContentLength());
		std::cout << std::endl;
	}
	std::cout << "content length in request: " << currentRequest.getCurrentContentLength() << std::endl;
	std::cout << "buffer.deb: " << buffer.deb << ", buffer.end: " << buffer.end << std::endl;
	std::cout << "buffer from buf.deb to buf.end: ";
	write(1, &(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
	std::cout << "end content info -"<< std::endl << std::endl;

}

void		ConnectionClass::_print_content_info(HttpRequest& currentRequest, std::string message)
{
	std::cout << " ------------------ CONTENT INFO: " << message << "-------------- " << std::endl;
	std::cout << "_hasRead: " << _hasRead << std::endl;
	std::cout << "_isHandlingBody: " << _isHandlingBody << std::endl;
	std::cout << "_isParsingContent: " << _isParsingContent << std::endl;
	std::cout << "_ContentLeftToRead: " << _ContentLeftToRead << std::endl;
	std::cout << "_isChunking: " << _isChunking << std::endl;
	std::cout << "_isReadingChunkNbr: " << _isReadingChunknbr << std::endl;
//	std::cout << "_isParsingContent: " << _isParsingContent << std::endl;
	if (currentRequest.getCurrentContentLength())
	{
		std::string to_print(currentRequest.getContent(), currentRequest.getCurrentContentLength());
		std::cout << "content in request: " << to_print << std::endl;
	}
	std::cout << "content length in request: " << currentRequest.getCurrentContentLength() << std::endl;
	std::cout << "end content info -"<< std::endl << std::endl;

}

int		ConnectionClass::_read_request_content(HttpRequest& CurrentRequest, readingBuffer& buffer)
{

	std::string	request_content;
//	int		to_read;
	int 		read_ret;
	int 		diff = buffer.end - buffer.deb;

//	std::cout << "in read_Request_Content" << std::endl;
//	_print_content_info(buffer, CurrentRequest, "deb read request");
	if (diff) // if part of the content is already in the buffer
	{
//		int diff = buffer.end - buffer.deb;
		if (diff >= _ContentLeftToRead) // if all the content is already in the buffer, no need to read on the socket
		{
			CurrentRequest.appendToContent(&(buffer.buf[buffer.deb]), CurrentRequest.getContentLength());
			buffer.deb += _ContentLeftToRead;
			_isParsingContent = 0;
			_isHandlingBody = 0;
			_ContentLeftToRead = 0;
//			if (buffer)
//			_save_only_buffer(buffer);
			return (1);
		}
		else // need to save, data missing
		{
			CurrentRequest.appendToContent(&(buffer.buf[buffer.deb]), diff);
			buffer.deb += diff;
			_ContentLeftToRead -= diff;
			return (_save_only_request(CurrentRequest));
		}
	}
	else // all the content must be read from the socket
	{
		if (!_hasRead)
		{
			if (_ContentLeftToRead < SINGLE_READ_SIZE)
				read_ret = recv(_socketNbr, &(buffer.buf[buffer.deb]), _ContentLeftToRead, 0);
			else
				read_ret = recv(_socketNbr, &(buffer.buf[buffer.deb]), SINGLE_READ_SIZE, 0);
			if (read_ret == 0 || read_ret == -1)
				return (read_ret);
			buffer.end += read_ret;
			_hasRead = 1;
//			_print_content_info(buffer, CurrentRequest, "before appending");
			CurrentRequest.appendToContent(&(buffer.buf[buffer.deb]), read_ret);
//			std::cout << "content: " << CurrentRequest.getContent() << std::endl;
			_ContentLeftToRead -= read_ret;
			buffer.deb = buffer.end;
			if (_ContentLeftToRead)
				return (_save_only_request(CurrentRequest));
			else
			{
				_isHandlingBody = 0;
				_isParsingContent = 0;
				return (CurrentRequest.getContentLength());
			}
		}
		else
			return (_save_only_request(CurrentRequest));
	}
	return (1);

}

/** there is data remaining in the buffer but not enough to start parsing a new request so 
 * we will only need to copy the buffer when we go back */
void		ConnectionClass::_save_only_buffer(readingBuffer& buffer)
{
	_restBuffer = new std::string(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
	_hasRestBuffer = 1;
}

/** when we run out of data while we were constructing an HttpRequest object, and we don't want 
 * to keep reading to avoid monopolizing cpu for only one connection, we save the request and 
 * the buffer and go back at processing them at next iteration */
int		ConnectionClass::_save_request_and_buffer(HttpRequest& currentRequest, readingBuffer& buffer)
{
	_incompleteRequest = new HttpRequest(currentRequest);
	_hasRestRequest = 1;
	if (_hasRestBuffer)
	{
		_restBuffer->append(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
		_hasRestBuffer = 1;
	}
	else if (buffer.end > buffer.deb)
	{
		_restBuffer = new std::string(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
		_hasRestBuffer = 1;
	}
	else
		_hasRestBuffer = 0;
//	if (_hasRestBuffer)
//		std::cout << "rest buffer: " << *_restBuffer << std::endl;
	return (SAVE_REQUEST);
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

/** read and parse trailers while they exist */ 
int		ConnectionClass::_readTrailers(readingBuffer& buffer, HttpRequest& currentRequest)
{
	int read_ret;

	if (!_hasRead)
	{
		buffer.deb = 0;
		read_ret = recv(_socketNbr, buffer.buf, SINGLE_READ_SIZE, 0);
		_hasRead = 1;
		if (read_ret == 0 || read_ret == -1)
			return (read_ret);
		buffer.end += read_ret;
	}
	read_ret = _read_line_trailer(buffer, currentRequest);
	if (read_ret == 0 || read_ret == -1 || read_ret == HTTP_ERROR)
		return (read_ret);
	if (read_ret == SAVE_REQUEST)
		return (SAVE_REQUEST);
	while (read_ret == 1)
	{
		read_ret = _read_line_trailer(buffer, currentRequest);	
		if (read_ret == 0 || read_ret == -1 || read_ret == HTTP_ERROR)
			return (read_ret);
		if (read_ret == SAVE_REQUEST)
			return (SAVE_REQUEST);
	}
	_isProcessingTrailers = 0;
	return (2);
}

int		ConnectionClass::_saveBegRestProcedure(HttpRequest& currentRequest, readingBuffer& buffer)
{
	_incompleteRequest = new HttpRequest(currentRequest);
	_hasRestRequest = 1;
	_beginningRestBuffer = new std::string(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
	return (SAVE_REQUEST);
}

int		ConnectionClass::_save_only_request(HttpRequest& currentRequest)
{
	_hasRestRequest = 1;
	_incompleteRequest = new HttpRequest(currentRequest);	
	return (SAVE_REQUEST);
}

/** get next request in the buffer or in the socket. if no_read_mode == NO_READ_MODE_ACTIVATED, it will 
 * not read on the socket after it runs out of data in the buffer, it will save everything and come back
 * next select iteration */ 
int		ConnectionClass::_get_next_request(readingBuffer &buffer, HttpRequest& currentRequest)
{
	int	ret_read_line;
	int	ret_read_content;	

// il faut mobiliser le reste ici!
// si il y a du reste qui n'est pas contenu, c'est forcément plus petit qu'une ligne

//	_printBufferInfo(buffer, "gnr");

	/** Tout ce 'if' sert uniquement au cas ou je commence à lire une requete et il me reste juste un bout de CRLF dans le buffer.
	 * il est recommandé par la RFC de tolérer un crlf perdu en debut de requête, donc ça sert à ça
	*/

//	if (_isHandlingBody)
	if (!(currentRequest.getLineCount()) && !_hasRestBuffer) // si c'est le tout debut de la requete et que j'ai toujours pas eu de caractere autre que CRLF
	{
		if (_hasBegRest)
		{
			std::cout << "IN ANNOYING PROCEDURE, NOT TESTED" << std::endl;
			int i = 0;
//			std::cout << "has beg rest" << std::endl;
			_beginningRestBuffer->append(&(buffer.buf[buffer.deb]), buffer.end - buffer.deb);
			if ((*_beginningRestBuffer)[i] == '\r' && (*_beginningRestBuffer)[i + 1] == '\n')
				i += 2;
			if ((*_beginningRestBuffer)[i] == '\r' && (*_beginningRestBuffer)[i + 1] == '\n')
				return (_invalidRequestProcedure(currentRequest, 400));
			std::memmove(buffer.buf, &(_beginningRestBuffer[i]), _beginningRestBuffer->length() - i);
			buffer.deb = 0;
			buffer.end = _beginningRestBuffer->length() - i;
			delete _beginningRestBuffer;
			_hasBegRest = 0;
		}
		else if (buffer.deb == '\r')
		{
			std::cout << "IN ANNOYING PROCEDURE, NOT TESTED" << std::endl;
			if ((buffer.end - buffer.deb) < 4)
				return (_saveBegRestProcedure(currentRequest, buffer));
			if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n')
			{
				buffer.deb += 2;
			}
			if (buffer.buf[buffer.deb] == '\r' && buffer.buf[buffer.deb + 1] == '\n') // c'est la
			{
				if (!(currentRequest.getLineCount()))
					return (_invalidRequestProcedure(currentRequest, 400));
			}
		}
	}
	while ((ret_read_line = _read_line(buffer, currentRequest)) == 1)
	{
		currentRequest.incrementLineCount();
		if (currentRequest.getLineCount() > MAX_HEAD_LINES)
		{
//			std::cout << "Too many header lines, need to send a bad request. for now, _get_next_request returns -1" 
//				<< std::endl;
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
			_isHandlingBody = 1;
			_isParsingContent = 1;
			_ContentLeftToRead = currentRequest.getContentLength();
			ret_read_content = _read_request_content(currentRequest, buffer);
			if (ret_read_content == -1 || ret_read_content == 0)
			{
				_isHandlingBody = 0;
				_isParsingContent = 0;
				std::cout << "ret_read_content returned -1 OR 0, meaning an error occured. For now, get_next_request forwards this ret_value" << std::endl;
				return (ret_read_content);
			}
			else if (ret_read_content == SAVE_REQUEST)
			{
				return (SAVE_REQUEST); //PROCÉDURE SAVE SE FAIT DANS LA FUNC
			}
			_isHandlingBody = 0;
			_isParsingContent = 0;
		}
		else if (currentRequest.isChunked())
		{
			_isHandlingBody = 1;
			_isChunking = 1;
			int chunk_ret = _getChunkedData(currentRequest, buffer);
			if (chunk_ret == 0 || chunk_ret == -1 || chunk_ret == HTTP_ERROR)
			{
				_isHandlingBody = 0;
				_isChunking = 0;
				return (chunk_ret);
			}
			else if (chunk_ret == SAVE_REQUEST)
			{
				return (_save_only_request(currentRequest));
			}
			_isHandlingBody = 0;
			_isChunking = 0;
			if (currentRequest.HasTrailers())
			{
				_isProcessingTrailers = 1;
				chunk_ret = _readTrailers(buffer, currentRequest);
				if (chunk_ret == 0 || chunk_ret == -1 || chunk_ret == HTTP_ERROR)
					return (chunk_ret);
				else if (chunk_ret == SAVE_REQUEST)
				{
					_save_request_and_buffer(currentRequest, buffer);
					return (SAVE_REQUEST);
				}
				_isProcessingTrailers = 0;
			}
			else
			{
				_isProcessingLastNL = 1;
				chunk_ret = _last_nl_procedure(buffer);
				if (chunk_ret == 0 || chunk_ret == -1 || chunk_ret == HTTP_ERROR)
					return (chunk_ret);
				else if (chunk_ret == SAVE_REQUEST)
				{
					_save_request_and_buffer(currentRequest, buffer);
					return (SAVE_REQUEST);
				}
				_isProcessingLastNL = 0;
			}
		}
		return (1);
	}
	if (ret_read_line == SAVE_REQUEST)
	{
//		std::cout << "save request returned by read_line" << std::endl;
		_save_request_and_buffer(currentRequest, buffer);
		return (SAVE_REQUEST);
	}
	std::cout << "unexpected return in _get_next_request" << std::endl; //debugging, should not happen
	return (1);
}

/** Read on the socket and fills the vector received in argument with parsed requests under the form
 * of HttpRequest objects */
int			ConnectionClass::receiveRequest(void)
{
	readingBuffer	buffer;
	int		read_ret;

	_hasRead = 0;
	_initializeBuffer(buffer);
	read_ret = _read_buffer(buffer, _request_pipeline); // fonction principale
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
		_status = CO_ISCLOSED;
		std::cout << "the connection has been closed" << std::endl;
		return (0);
	}
	if (_request_pipeline.size())
		_status = CO_ISREADY;
	return (1);
}

int			ConnectionClass::sendResponse(std::string response)
{
	if (send(_socketNbr, response.c_str(), response.length(), 0) == -1)
	{
		std::perror("send");
		closeConnection();
		return (-1);
	}
//	std::cout << "erasing request that starts with: " << _request_pipeline[0].getStartLine() << std::endl;
	_request_pipeline.erase(_request_pipeline.begin());
	if (_request_pipeline.empty())
	{
		if (isPersistent())
			setStatus(CO_ISDONE);
		else
			closeConnection();
	}
	resetTimer();
	return (0);
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
//	int empty_read_value;
	int return_value;

//	std::cout << "close connection is called" << std::endl;
	shutdown(_socketNbr, SHUT_WR);
//		perror("shutdown");
//	empty_read_value = _emptyReadBuffers();
	usleep(30);
	shutdown(_socketNbr, SHUT_RD);
//		perror("shutdown");
	return_value = close(_socketNbr);
/*	if (return_value == 0)
		_status = CO_ISCLOSED;
	else
		perror("close");*/
	_status = CO_ISCLOSED;
	return (return_value);
}

void	ConnectionClass::print_pipeline()
{
	size_t i = 0;

	std::cout << std::endl;
	std::cout <<  " ----------------- FULL REQUEST PIPELINE -------------- " << std::endl;

	std::cout << "WARNING: persistence is an attribute of the connection, not of the request, therefore if \
the last request is not persistent, all request might be marked as non persistent, even though their \
header should imply otherwise." << std::endl;
	std::cout << std::endl;
	while (i < _request_pipeline.size())
	{	
		std::cout << "                 REQUEST NBR:  " << i << std::endl;
		std::cout << std::endl;
		std::cout << "START LINE: "  << _request_pipeline[i].getStartLine() << std::endl;
		std::cout << "URI: " << _request_pipeline[i].getRequestLineInfos().target << std::endl;
		std::cout << std::endl;
		std::cout << "HEADERS: " << std::endl;
		_request_pipeline[i].printHeaders();
		std::cout << std::endl;
		std::cout << "ENCODINGS: " << std::endl;
		print_vec(_request_pipeline[i].getModifyableTE());
		std::cout << std::endl;
		std::cout << "BODY: " << _request_pipeline[i].getContent() << std::endl;
		std::cout << std::endl;
		std::cout << "TRAILERS: " << std::endl;
		_request_pipeline[i].printTrailers();
		std::cout << std::endl;
		std::cout << "validity: " << _request_pipeline[i].isValid() << std::endl;
		std::cout << "persistence: " << isPersistent() << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		i++;
	}
	std::cout <<  "              ------------------------------              " << std::endl;
}

void			ConnectionClass::setServers(std::vector<serverClass*> servers, int fd)
{
	for (std::vector<serverClass*>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		std::cout << (*it)->_server_name << " : " << (*it)->_server_socket << std::endl;
		if ((*it)->_server_socket == fd)
			_servers.push_back(*it);
	}
}

serverClass*	ConnectionClass::getServer(std::string server_name)
{
	for (std::vector<serverClass*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		if ((*it)->_server_name == server_name)
			return *it;
	return _servers[0];
}

int				ConnectionClass::getStatus(void) const
{
	return (_status);
}

void			ConnectionClass::setStatus(int state)
{
	_status = state;
}

bool				ConnectionClass::isPersistent() const
{
	return (_isPersistent);
}

HttpRequest const&	ConnectionClass::getRequest(unsigned int request_number) const
{
	if (request_number >= _request_pipeline.size())
		request_number = _request_pipeline.size() - 1;
	return (_request_pipeline[request_number]);
}

time_t			ConnectionClass::getTimer(void) const
{
	return _timer;
}

void			ConnectionClass::resetTimer(void)
{
	_timer = time(0);
}

void			ConnectionClass::setHasToWriteOnPipe(int value)
{
	_hasToWriteOnPipe = value;
}

int			ConnectionClass::HasToWriteOnPipe()
{
	return (_hasToWriteOnPipe);
}

void			ConnectionClass::setHasToReadOnPipe(int value)
{
	_hasToReadOnPipe = value;
}

int			ConnectionClass::HasToReadOnPipe()
{
	return (_hasToReadOnPipe);
}

void			ConnectionClass::setHasDoneCgi(int value)
{
	_hasDoneCgi = value;
}

int			ConnectionClass::HasDoneCgi()
{
	return (_hasDoneCgi);
}

void			ConnectionClass::setInputFd(int value)
{
	_input_fd = value;
}

int			ConnectionClass::getInputFd()
{
	return (_input_fd);
}

void			ConnectionClass::setOutputFd(int value)
{
	_output_fd = value;
}

int			ConnectionClass::getOutputFd()
{
	return (_output_fd);
}

void			ConnectionClass::setChildPid(int value)
{
	_childPid = value;
}

int			ConnectionClass::getChildPid()
{
	return (_childPid);
}