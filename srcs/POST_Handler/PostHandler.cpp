#include "PostHandler.hpp"
#include "cgiLauncher.hpp"
#include <sstream>
#include <utility>
#include <fstream>

// 	adds cgi headers to other headers and sets index of the beginning of the messager body


int				_findStrIndex(std::string to_find, char *buf, size_t buffer_size)
{
	size_t		i = 0;
	size_t 	j = 0;
	while (i < buffer_size)
	{
		while (buf[i] == to_find[j])
		{
			if (j == to_find.length() - 1)
			{
				return (i - (to_find.length() - 1));
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

void	add_header_part(HttpResponse& response , char *str, size_t buffer_size ,size_t& body_beginning)
{
	body_beginning = _findStrIndex("\r\n\r\n", str, buffer_size);
//	std::cout << "body beginning: " << body_beginning << std::endl;
	std::string	header_part(str, body_beginning);
	std::string	header_string;
	size_t	next_crlf;
	size_t	field_value_separator_index;
	size_t pos = 0;

//	std::cout << "header part: " << header_part << std::endl;
	next_crlf = header_part.find("\r\n");
	while (next_crlf < body_beginning)
	{
		header_string = header_part.substr(pos, next_crlf - pos);
//		std::cout << "header string: " << header_string << std::endl;
		field_value_separator_index = header_string.find(':');
		std::pair<std::string, std::string> header_pair(header_string.substr(0, field_value_separator_index), header_string.substr(field_value_separator_index + 1)); //optimisable
		response.addHeader(header_pair);
		pos = next_crlf + 2;
		next_crlf = header_part.find("\r\n", next_crlf + 2);
		if (next_crlf == header_part.npos)
		{
			header_string = header_part.substr(pos, next_crlf - pos);
//			std::cout << "header string: " << header_string << std::endl;
			field_value_separator_index = header_string.find(':');
			std::pair<std::string, std::string> header_pair(header_string.substr(0, field_value_separator_index), header_string.substr(field_value_separator_index + 1)); //optimisable
			response.addHeader(header_pair);
			break;	
		}
//		std::cout << "next crlf: " << next_crlf << ", body_beginning: " << body_beginning << std::endl; 
	}
	body_beginning += 4; // je saute les crlf pour arriver direct au début du body
//	response.printHeaders();
}

void		setCgiParams(t_CgiParams& params, HttpRequest const& request, LocationClass const& location)
{
	std::string		tmp = location.getRoot();
	std::string	target = request.getRequestLineInfos().target;

	tmp.append(target);
	if ( target.length() > 4 && target.find(".php") == (target.length() - 4))
	{
		params.scriptFilename = tmp;
		params.scriptName = target.substr(target.find_last_of('/'));
	}
	else
	{
		params.scriptFilename = tmp.append(location.getIndex());
		params.scriptName = location.getIndex();
	}
//	std::cout << "tmp: " << tmp << std::endl;
	params.redirectStatus = "200";
	params.requestMethod = "POST";
	if (request.getContentLength())
	{
		std::stringstream	stream;
		stream << request.getContentLength();
		stream >> params.contentLength;
		params.contentType = (*(request.getHeaders().find("Content-Type"))).second;
	}
	params.pathInfo = target;
	params.serverName = location.getServerName(); // a modif
	params.serverProtocol = "HTTP/1.1";
	params.requestUri = target;
	params.serverName = location.getServerName(); // a modif
}

HttpResponse	answer_post(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;

	//TODO
	char		*output;
	t_CgiParams	params;
	size_t		body_beginning = 0;
	size_t		output_len = 0;
	std::ifstream body;

	setCgiParams(params, request, location);
	body.open(params.scriptFilename.c_str());
	if (!body.is_open())
	{
		response = HttpResponse(404, location.getErrorPage(404));
		body.close();
		return (response);
	}
	body.close();
	launchCgiScript(params, request, location, &output, output_len);
//	std::cout << "output: ";
//	write(1, output, output_len);
	add_header_part(response, output, output_len, body_beginning);
//	std::cout << "after header" << std::endl;
	response.setBody(&(output[body_beginning]), output_len - body_beginning);
	response.setHeader();
	
	return (response);
}