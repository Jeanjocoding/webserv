/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 21:54:40 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/27 19:16:29 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utility>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <cstdio>
#include <vector>

#include "webserv.hpp"
#include "ConnectionUtils.hpp"
#include "ConnectionClass.hpp"
#include "HttpResponse.hpp"


void	print_pipeline(std::vector<HttpRequest>& requestPipeline, ConnectionClass& connection)
{
	size_t i = 0;

	std::cout << std::endl;
	std::cout <<  " ----------------- FULL REQUEST PIPELINE -------------- " << std::endl;
	std::cout << std::endl;
	while (i < requestPipeline.size())
	{	
		std::cout << "                 REQUEST NBR:  " << i << std::endl;
		std::cout << std::endl;
		std::cout << "START LINE: "  << requestPipeline[i].getStartLine() << std::endl;
		std::cout << "URI: " << requestPipeline[i].getRequestLineInfos().target << std::endl;
		std::cout << std::endl;
		std::cout << "HEADERS: " << std::endl;
		requestPipeline[i].printHeaders();
		std::cout << std::endl;
		std::cout << "ENCODINGS: " << std::endl;
		print_vec(requestPipeline[i].getModifyableTE());
		std::cout << std::endl;
		if (requestPipeline[i].isChunked())
			std::cout << "content length of chunked: " << requestPipeline[i].getCurrentContentLength() << std::endl;
		std::cout << "BODY: ";
	std::string to_print(requestPipeline[i].getContent(), requestPipeline[i].getCurrentContentLength());
	std::cout << to_print << std::endl;
		std::cout << "TRAILERS: " << std::endl;
		requestPipeline[i].printTrailers();
		std::cout << std::endl;
		std::cout << "validity: " << requestPipeline[i].isValid() << std::endl;
		std::cout << "persistence: " << connection.isPersistent() << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;
		i++;
	}
	std::cout <<  "              ------------------------------              " << std::endl;
}

void	print_request(HttpRequest& request)
{
	std::cout << std::endl;
	std::cout <<  " ----------------- FULL REQUEST -------------- " << std::endl;
	std::cout << std::endl;
	std::cout << "START LINE: "  << request.getStartLine() << std::endl;
	std::cout << "URI: " << request.getRequestLineInfos().target << std::endl;
	std::cout << std::endl;
	std::cout << "HEADERS: " << std::endl;
	request.printHeaders();
	std::cout << std::endl;
	std::cout << "ENCODINGS: " << std::endl;
	print_vec(request.getModifyableTE());
	std::cout << std::endl;
	if (request.isChunked())
		std::cout << "content length of chunked: " << request.getCurrentContentLength() << std::endl;
	std::cout << std::endl;
	std::cout << "BODY: ";
	std::string to_print(request.getContent(), request.getCurrentContentLength());
	std::cout << to_print << std::endl;
	std::cout << std::endl;
	std::cout << "TRAILERS: " << std::endl;
	request.printTrailers();
	std::cout << std::endl;
	std::cout << "validity: " << request.isValid() << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout <<  "              ------------------------------              " << std::endl;
}

static void	send_error(unsigned short error_nb, std::map<unsigned short, std::string> const& error_map, ConnectionClass& connection)
{
	HttpResponse response = HttpResponse(error_nb, error_map.find(error_nb)->second);
	response.setConnectionStatus(false);
	connection.sendResponse(response.toString());
}

static HttpResponse answer_cgi_get(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	char*			output;
	t_CgiParams		params;
	size_t			body_begin = 0;
	size_t			output_len = 0;
	std::ifstream	body;

	setCgiParams(params, request, location);
	body.open(params.scriptFilename.c_str());
	if (!body.is_open())
		return HttpResponse(404, location.getErrorPage(404));
	launchCgiScript(params, request, location, &output, output_len);
	add_header_part(response, output, output_len, body_begin);
	response.setBody(&(output[body_begin]));
	response.setHeader();
	return response;
}

static HttpResponse	answer_get(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	std::string		tmp = location.getRoot();// Put the root working directory in tmp
	//TODO
	
	tmp.append(request.getRequestLineInfos().target);// Append the requested  uri
	std::cout << "answering get request\ntrying to get file : " << tmp << std::endl;

	if (location.isCGI())
		return answer_cgi_get(request, location);
	if (request.getRequestLineInfos().target == location.getUri() + "/" ||
		(request.getRequestLineInfos().target == location.getUri() && *(--request.getRequestLineInfos().target.end()) == '/'))// If index is requested
	{
		tmp.append(location.getIndex());// Append the name of the index file
		std::ifstream body;
		body.open(tmp.c_str());
		std::cout << "tmp : " << tmp << std::endl;
		if (!body.is_open())
		{
			if (location.autoIndexIsOn())
			{
				response.setBody(location.getAutoIndex());//TODO to code
				response.setHeader(200);
			}
			else
				response = HttpResponse(404, location.getErrorPage(404));
		}
		else
		{
			try// Try to input requested file in response body
			{
				tmp = std::string(std::istreambuf_iterator<char>(body), std::istreambuf_iterator<char>());
				response.setBody(tmp.begin(), tmp.end());
				response.setHeader(200);
			}
			catch (std::ios_base::failure const& e)// If requested file is a folder return 404
			{
				response = HttpResponse(404, location.getErrorPage(404));
			}
		}
	}
	else // If request is not index
	{
		std::ifstream body;// Creates a buffer to put file content
		body.open(tmp.c_str());//might put all this part in the HttpResponse object
		if (!body.is_open())
			response = HttpResponse(404, location.getErrorPage(404));// If requested file is not open return 404
		else
		{
			try// Try to input requested file in response body
			{
				tmp = std::string(std::istreambuf_iterator<char>(body), std::istreambuf_iterator<char>());
				response.setBody(tmp.begin(), tmp.end());
				response.setHeader(200);
			}
			catch (std::ios_base::failure const& e)// If requested file is a folder return 404
			{
				response = HttpResponse(404, location.getErrorPage(404));
			}
		}
	}
	return response;
}

static HttpResponse	answer_delete(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	std::string		tmp = location.getRoot();

	//TODO
	tmp.append(request.getRequestLineInfos().target);
	std::cout << "answering delete request\n";
	return response;
}

static HttpResponse	answer_redirection(HttpRequest const& request, LocationClass const& location)
{
	std::string tmp;
	if (location.getRedirectUrl().find('$') != std::string::npos)
	{
		tmp = request.getRequestLineInfos().target;// Put original requested uri in tmp
//		if (!(location.getUri() == "/" && request.getRequestLineInfos().target == "/"))
//			tmp.erase(0, location.getUri().size());
		tmp.erase(0, location.getUri().size());// Remove the location part of the url
		tmp.insert(0, location.getRedirectUrl(), 0, location.getRedirectUrl().find('$'));// Put redirect uri at begining of requested uri
	}
	else
		tmp = location.getRedirectUrl();
//	std::cout << "redirecting " << request.getRequestLineInfos().target << " to " << location.getRedirectUrl()<< " resulting in : " << tmp << std::endl;//for test
	return HttpResponse(location.getRedirectCode(), tmp);// Send redirect response
}

void	answer_connection(ConnectionClass& connection)
{
	serverClass& server = *connection._server;
	HttpResponse response;
	if (connection._request_pipeline.empty())
	{
		connection.setStatus(CO_ISDONE);
		return ;
	}
	HttpRequest& request = connection._request_pipeline[0];
	if (!request.isValid())//TODO check why is invalid and respond accordingly
		return send_error(400, server._default_error_pages, connection);
	print_request(request);
	LocationClass location = server.getLocation(request.getRequestLineInfos().target);//TODO
	if (!location.methodIsAllowed(request.getMethod()))
	{
		std::cerr << "forbiden Http request method on location " << location.getUri() << std::endl;
		return send_error(405, location.getErrorMap(), connection);
	}
	if (location.isRedirect())//TODO redirect request
	{
		connection.sendResponse(answer_redirection(request, location).toString());// Send redirect response
		return ;
	}
	switch (request.getMethod())// Generate the HttpResponse depending on HttpMethod
	{
		case GET_METHOD :
			response = answer_get(request, location);
			break;
		case POST_METHOD :
			response = answer_post(request, location);
			break;
		case DELETE_METHOD :
			response = answer_delete(request, location);
			break;
		default :
			return send_error(501, location.getErrorMap(), connection);
	}
	if (!connection.isPersistent())
		response.setConnectionStatus(false);
	connection.sendResponse(response.toString());// Handles all of the response sending and adjust the connection accordingly (cf: pop request list close connection etc...)
}
