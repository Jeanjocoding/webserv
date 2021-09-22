/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 21:54:40 by asablayr          #+#    #+#             */
/*   Updated: 2021/09/22 11:59:25 by asablayr         ###   ########.fr       */
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
	if (request.isChunked() || request.getContentLength())
		std::cout << "current content length: " << request.getCurrentContentLength() << std::endl;
	std::cout << std::endl;
/*	std::cout << "BODY: ";
	write (1, request.getContent(), request.getCurrentContentLength());*/
//	std::string to_print(request.getContent(), request.getCurrentContentLength());
//	std::cout << to_print << std::endl;
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
//	response.setConnectionStatus(false);
	connection.sendResponse(response.toString());
}

static HttpResponse& answer_cgi_get(HttpRequest const& request, LocationClass const& location, ConnectionClass& connection)
{
//	HttpResponse	response;
//	char*			output;
	t_CgiParams		params;
//	size_t			body_begin = 0;
//	size_t			output_len = 0;
	std::ifstream	body;

//	std::cout << "in answer cgi get" << std::endl;
	setCgiParams(params, request, location);//TODO check with mate
	body.open(params.scriptFilename.c_str());
	if (!body.is_open())
	{
		delete connection._currentResponse;
		connection._currentResponse = new HttpResponse(404, location.getErrorPage(404));
		connection._currentResponse->setError(1);
		return	(*connection._currentResponse);
	}

	
	ExecAndSetPipes(params, location, connection);
//	launchCgiScript(params, request, location, &output, output_len);
//	add_header_part(response, output, output_len, body_begin);
//	write(1, output, output_len);
//	response.setBody(&(output[body_begin]), output_len - body_begin);
//	response.setHeader();
	return  (*connection._currentResponse);
}

static HttpResponse&	answer_get(HttpRequest const& request, LocationClass const& location, ConnectionClass& connection)
{
//	HttpResponse	response;
	std::string		tmp = location.getRoot();// Put the root working directory in tmp
	//TODO
	
	tmp.append(request.getRequestLineInfos().target);// Append the requested  uri
	if (location.isCGI())// If cgi is requested
		return answer_cgi_get(request, location, connection); //Return response returned by answer_cgi
	if (request.getRequestLineInfos().target == location.getUri() + "/" ||
		(request.getRequestLineInfos().target == location.getUri() && *(--request.getRequestLineInfos().target.end()) == '/'))// If index is requested
	{
		tmp.append(location.getIndex());// Append the name of the index file
		std::ifstream body;
		body.open(tmp.c_str());
		if (!body.is_open())
		{
			if (location.autoIndexIsOn())
			{
				tmp.erase(tmp.rfind('/') + 1, tmp.size());
				tmp = location.getAutoindex(tmp);
//				response.setBody(tmp.begin(), tmp.end());
//				response.setHeader(200);
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
			try// Try to input requested file in response body
			{
				tmp = std::string(std::istreambuf_iterator<char>(body), std::istreambuf_iterator<char>());
				connection._currentResponse->setBody(tmp.begin(), tmp.end());
				connection._currentResponse->setHeader(200);
			}
			catch (std::ios_base::failure const& e)// If requested file is a folder return 404
			{
				delete connection._currentResponse;
				connection._currentResponse = new HttpResponse(404, location.getErrorPage(404));
			}
		}
	}
	else // If request is not index
	{
		std::ifstream body;// Creates a buffer to put file content
		body.open(tmp.c_str());//might put all this part in the HttpResponse object
		if (!body.is_open())
		{
			delete connection._currentResponse;
			connection._currentResponse = new HttpResponse(404, location.getErrorPage(404));// If requested file is not open return 404
		}
		else
		{
			try// Try to input requested file in response body
			{
				tmp = std::string(std::istreambuf_iterator<char>(body), std::istreambuf_iterator<char>());
				connection._currentResponse->setBody(tmp.begin(), tmp.end());
				connection._currentResponse->setHeader(200);
			}
			catch (std::ios_base::failure const& e)// If requested file is a folder return 404
			{
				delete connection._currentResponse;
				connection._currentResponse = new HttpResponse(404, location.getErrorPage(404));// If requested file is not open return 404
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
		tmp = request.getRequestLineInfos().target;// Put original requested uri in tmp
		tmp.erase(0, location.getUri().size());// Remove the location part of the url
		tmp.insert(0, location.getRedirectUrl(), 0, location.getRedirectUrl().find('$'));// Put redirect uri at begining of requested uri
	}
	else
		tmp = location.getRedirectUrl();
	return HttpResponse(location.getRedirectCode(), tmp);// Send redirect response
}

void	answer_connection(ConnectionClass& connection)
{
	std::cout << "pipeline size: " << connection._request_pipeline.size() << std::endl;
	if (connection._request_pipeline.empty())
	{
		connection.setStatus(CO_ISDONE);
		return ;
	}
	HttpRequest& request = connection._request_pipeline[0];
	print_request(request);
	serverClass& server = *(connection.getServer(request.getHeaders().find("Host")->second));
	if (!request.isValid())//TODO check why is invalid and respond accordingly
		return send_error(request.getErrorCode(), server._default_error_pages, connection);
	LocationClass location = server.getLocation(request.getRequestLineInfos().target);
	if (connection.HasToWriteOnPipe() || connection.HasToReadOnPipe())
		return;
	else if (connection.HasDoneCgi())
	{
			if (!connection.isPersistent() || location.getKeepaliveTimeout() == 0)
				connection._currentResponse->setConnectionStatus(false);
			size_t body_beginning = 0;
			add_header_part((*connection._currentResponse), connection._cgiOutput, connection._cgiOutput_len, body_beginning);
			connection._currentResponse->setBody(&(connection._cgiOutput[body_beginning]), connection._cgiOutput_len - body_beginning);
			connection._currentResponse->setHeader(200);
			if (!connection.isPersistent() || location.getKeepaliveTimeout() == 0)
				connection._currentResponse->setConnectionStatus(false);
			connection.sendResponse(connection._currentResponse->toString());// Handles all of the response sending and adjust the connection accordingly (cf: pop request list close connection etc...)
			delete connection._currentResponse;
			delete [] connection._cgiOutput;
			connection._cgiOutput = 0;
			connection._cgiOutput_len = 0;
			connection.setHasDoneCgi(0);
			return;
	}
	connection._currentResponse = new HttpResponse();
	if (!location.methodIsAllowed(request.getMethod()))
	{
		std::cerr << "forbiden Http request method on location " << location.getUri() << std::endl;
		delete connection._currentResponse;
		return send_error(405, location.getErrorMap(), connection);
	}
	if (location.isRedirect())
	{
		connection.sendResponse(answer_redirection(request, location).toString());// Send redirect response
		return ;
	}
	switch (request.getMethod())// Generate the HttpResponse depending on HttpMethod
	{
		case GET_METHOD :
			/* leak probable: */
			answer_get(connection._request_pipeline[0], location, connection);
			if (location.isCGI() && !connection._currentResponse->isError())
				return;
			break;
		case POST_METHOD :
			answer_post(request, location, connection);
			if (connection.HasToWriteOnPipe())
				return; // a modif
			else
				break;
		case DELETE_METHOD :
			*connection._currentResponse = answer_delete(request, location);
			break; //uniformiser avec autres reponses pour ne plus retourner
		default :
			return send_error(501, location.getErrorMap(), connection);
	}
	if (!connection.isPersistent() || location.getKeepaliveTimeout() == 0)
	           connection._currentResponse->setConnectionStatus(false);
        connection.sendResponse(connection._currentResponse->toString());// Handles all of the response sending and adjust the connection accordingly (cf: pop request list close connection etc...)
	delete connection._currentResponse;
//	std::cout << "response :\n" << response.toString() << std::endl;
}
