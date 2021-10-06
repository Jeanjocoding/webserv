/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 21:54:40 by asablayr          #+#    #+#             */
/*   Updated: 2021/10/06 12:28:49 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utility>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <cstdio>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

#include "webserv.hpp"
#include "ConnectionUtils.hpp"
#include "ConnectionClass.hpp"
#include "HttpResponse.hpp"


void	print_request(HttpRequest& request)
{
	std::cout << std::endl;
	std::cout <<  " ----------------- FULL REQUEST -------------- " << std::endl;
	std::cout << std::endl;
	std::cout << "START LINE: "  << request.getStartLine() << std::endl;
	std::cout << "URI: " << request.getRequestLineInfos().target << std::endl;
	std::cout << "QUER_STRING: " << request.getRequestLineInfos().query_string << std::endl;
	std::cout << std::endl;
	std::cout << "HEADERS: " << std::endl;
	request.printHeaders();
	std::cout << std::endl;
	std::cout << "ENCODINGS: " << std::endl;
	print_vec(request.getModifyableTE());
	std::cout << std::endl;
	if (request.isChunked() || request.getContentLength())
		std::cout << "current content length: " << request.getCurrentContentLength() << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "TRAILERS: " << std::endl;
	request.printTrailers();
	std::cout << std::endl;
	std::cout << "validity: " << request.isValid() << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout <<  "              ------------------------------              " << std::endl;
}

bool		is_index_requested(std::string const& requested_uri)
{
	struct stat file_info;

	stat(requested_uri.c_str(), &file_info);
	if (S_ISDIR(file_info.st_mode))
		return true;
	else
		return false;
}

static void	send_error(unsigned short error_nb, std::map<unsigned short, std::string> const& error_map, ConnectionClass& connection)
{
	HttpResponse response = HttpResponse(error_nb, error_map.find(error_nb)->second);
	connection.sendResponse(response.toString());
}

static HttpResponse& answer_cgi_get(HttpRequest const& request, LocationClass const& location, ConnectionClass& connection)
{
	t_CgiParams		params;
	int				retset;
	struct stat		st_stat;

	retset = setCgiParams(params, request, location);
	if (retset == EXTENSION_NOT_VALID)
	{
		delete connection._currentResponse;
		connection._currentResponse = new HttpResponse(405, location.getErrorPage(405));
		return (*(connection._currentResponse));
	}
	else if ( retset == FILE_NOT_FOUND || stat(params.scriptFilename.c_str(), &st_stat) == -1)
	{
		delete connection._currentResponse;
		connection._currentResponse = new HttpResponse(404, location.getErrorPage(404));
		return (*(connection._currentResponse));
	}
	if (ExecAndSetPipes(params, location, connection) == -1)
	{
		delete connection._currentResponse;
		connection._currentResponse = new HttpResponse(500, location.getErrorPage(500));
		return (*(connection._currentResponse));
	}
	return  (*connection._currentResponse);
}

static HttpResponse&	answer_get(HttpRequest const& request, LocationClass const& location, ConnectionClass& connection)
{
	std::string		tmp = location.getRoot();
	struct stat		file_infos;
	
	tmp.append(request.getRequestLineInfos().target);
	if (request.isCGI())
		return answer_cgi_get(request, location, connection);
	if (is_index_requested(tmp))
	{
		if (*(--tmp.end()) != '/')
			tmp.append("/");
		tmp.append(location.getIndex());
		std::ifstream body;
		body.open(tmp.c_str());
		if (!body.is_open())
		{
			if (location.autoIndexIsOn())
			{
				tmp.erase(tmp.rfind('/') + 1, tmp.size());
				tmp = location.getAutoindex(tmp);
				connection._currentResponse->setBody(tmp.begin(), tmp.end());
				connection._currentResponse->setHeader(200);
			}
			else
			{
				delete connection._currentResponse;
				connection._currentResponse = new HttpResponse(404, location.getErrorPage(404));
			}
		}
		else
		{
			stat(tmp.c_str(), &file_infos);
			if (S_ISREG(file_infos.st_mode))
			{
				tmp = std::string(std::istreambuf_iterator<char>(body), std::istreambuf_iterator<char>());
				connection._currentResponse->setBody(tmp.begin(), tmp.end());
				connection._currentResponse->setHeader(200);
			}
			else
			{
				delete connection._currentResponse;
				connection._currentResponse = new HttpResponse(404, location.getErrorPage(404));
			}
		}
	}
	else
	{
		std::ifstream body;
		body.open(tmp.c_str());
		if (!body.is_open())
		{
			delete connection._currentResponse;
			connection._currentResponse = new HttpResponse(404, location.getErrorPage(404));
		}
		else
		{
			stat(tmp.c_str(), &file_infos);
			if (S_ISREG(file_infos.st_mode))
			{
				tmp = std::string(std::istreambuf_iterator<char>(body), std::istreambuf_iterator<char>());
				connection._currentResponse->setBody(tmp.begin(), tmp.end());
				connection._currentResponse->setHeader(200);
			}
			else
			{
				delete connection._currentResponse;
				connection._currentResponse = new HttpResponse(404, location.getErrorPage(404));
			}
		}
	}
	return *connection._currentResponse;
}

static HttpResponse	answer_redirection(HttpRequest const& request, LocationClass const& location)
{
	std::string tmp;
	if (location.getRedirectUrl().find('$') != std::string::npos)
	{
		tmp = request.getRequestLineInfos().target;
		tmp.erase(0, location.getUri().size());
		tmp.insert(0, location.getRedirectUrl(), 0, location.getRedirectUrl().find('$'));
	}
	else
		tmp = location.getRedirectUrl();
	return HttpResponse(location.getRedirectCode(), tmp);
}

void	answer_connection(ConnectionClass& connection)
{
	if (connection._request_pipeline.empty())
	{
		connection.setStatus(CO_ISDONE);
		return ;
	}
	HttpRequest& request = connection._request_pipeline[0];
	serverClass& server = (request.getHeaders().find("Host") != request.getHeaders().end()) ? *(connection.getServer(request.getHeaders().find("Host")->second)) : *(connection.getServer());
	if (!request.isValid())
		return send_error(request.getErrorCode(), server._default_error_pages, connection);
	LocationClass const& location = *(connection._request_pipeline[0].getLocation());
	if (connection.HasToWriteOnPipe() || connection.HasToReadOnPipe())
		return;
	else if (connection.HasDoneCgi())
	{
			if (!connection.isPersistent() || location.getKeepaliveTimeout() == 0)
				connection._currentResponse->setConnectionStatus(false);
			if (connection.hasCgiError())
			{
				delete connection._currentResponse;
				connection.setHasDoneCgi(0);
				return send_error(500, location.getErrorMap(), connection);
			}
			size_t body_beginning = 0;
			add_header_part((*connection._currentResponse), connection._cgiOutput, connection._cgiOutput_len, body_beginning);
			connection._currentResponse->setBody(&(connection._cgiOutput[body_beginning]), connection._cgiOutput_len - body_beginning);
			connection._currentResponse->setHeader(200);
			connection.sendResponse(connection._currentResponse->toString());
			delete connection._currentResponse;
			delete [] connection._cgiOutput;
			connection._cgiOutput = 0;
			connection._cgiOutput_len = 0;
			connection.setHasDoneCgi(0);
			return;
	}
	connection._currentResponse = new HttpResponse();
	if (request.getMethod() == -1)
	{
		delete connection._currentResponse;
		return send_error(501, location.getErrorMap(), connection);
	}
	else if (!location.methodIsAllowed(request.getMethod()))
	{
		delete connection._currentResponse;
		return send_error(405, location.getErrorMap(), connection);
	}
	if (location.isRedirect())
	{
		location.printLocation();
		connection.sendResponse(answer_redirection(request, location).toString());
		return ;
	}
	switch (request.getMethod())
	{
		case GET_METHOD :
			answer_get(connection._request_pipeline[0], location, connection);
			if (connection.HasToWriteOnPipe() && !connection._currentResponse->isError())
				return;
			break;
		case POST_METHOD :
			answer_post(request, location, connection);
			if (connection.HasToWriteOnPipe())
				return;
			else
				break;
		case DELETE_METHOD :
			*connection._currentResponse = answer_delete(request, location);
			break;
		default :
			return send_error(501, location.getErrorMap(), connection);
	}
	if (!connection.isPersistent() || location.getKeepaliveTimeout() == 0)
		connection._currentResponse->setConnectionStatus(false);
	connection.sendResponse(connection._currentResponse->toString());
	delete connection._currentResponse;
}
