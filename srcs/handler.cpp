/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 21:54:40 by asablayr          #+#    #+#             */
/*   Updated: 2021/08/16 16:37:58 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utility>
#include <iostream>
#include <fstream>
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
		std::cout << "BODY: " << requestPipeline[i].getContent() << std::endl;
		std::cout << std::endl;
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
	std::cout << "BODY: " << request.getContent() << std::endl;
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
	HttpResponse response = HttpResponse(error_nb, error_map.find(error_nb)->second);//TODO
	if (connection.sendResponse(response.toString()) == -1)
	{
		std::perror("send");
		connection.closeConnection();
	}
	connection._request_pipeline.erase(connection._request_pipeline.begin());//might put this part in a connection method
	if (connection._request_pipeline.empty())
	{
		if (connection.isPersistent())
			connection.setStatus(CO_ISDONE);
		else
			connection.closeConnection();
	}
	return ;
}

static HttpResponse	answer_get(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	std::string		tmp = location.getRoot();
	//TODO
	
	std::cout << "root : " << location.getRoot() << std::endl;
	tmp.append(request.getRequestLineInfos().target);
	std::cout << "answering get request\ntrying to get file : " << tmp << std::endl;

	if (*(--request.getRequestLineInfos().target.end()) != '/')
	{
		std::ifstream body;
		body.open(tmp.c_str());
		if (!body.is_open())
			response = HttpResponse(404, location.getErrorPage(404));
		else
			response = HttpResponse(200, tmp);
	}
	else
	{
		tmp.append(location.getIndex());
		if (tmp.empty() && location.autoIndexIsOn())
			response.setBody(location.getAutoIndex());// to code
		std::ifstream body;
		body.open(tmp.c_str());
		std::cout << "tmp : " << tmp << std::endl;
		if (!body.is_open())
			response = HttpResponse(404, location.getErrorPage(404));
		else
			response = HttpResponse(200, tmp);
	}
	return response;
}

/* static HttpResponse	answer_post(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	std::string		tmp = location.getRoot();

	//TODO

	tmp.append(request.getRequestLineInfos().target);
	std::cout << "answering post request\n";
	return response;
} */

static HttpResponse	answer_delete(HttpRequest const& request, LocationClass const& location)
{
	HttpResponse	response;
	std::string		tmp = location.getRoot();

	//TODO
	tmp.append(request.getRequestLineInfos().target);
	std::cout << "answering delete request\n";
	return response;
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
	if (!request.isValid())
		return send_error(400, server._default_error_pages, connection);
	print_request(request);
	std::cout << "target: " << request.getRequestLineInfos().target << std::endl;
	LocationClass location = server.getLocation(request.getRequestLineInfos().target);//TODO
	if (!location.methodIsAllowed(request.getMethod()))
	{
		std::cerr << "forbiden Http request method on location " << location.getUri() << std::endl;
		return send_error(405, location.getErrorMap(), connection);//TODO
	}
	switch (request.getMethod())
	{
		case GET_METHOD :
			response = answer_get(request, location);//TODO
			break;
		case POST_METHOD :
			response = answer_post(request, location);//TODO
			break;
		case DELETE_METHOD :
			response = answer_delete(request, location);//TODO
			break;
		default :
			return send_error(501, location.getErrorMap(), connection);
	}
	if (connection.sendResponse(response.toString()) == -1)
	{
		std::perror("send");
		connection.closeConnection();
		return ;
	}
	connection._request_pipeline.erase(connection._request_pipeline.begin());//might put this part in a connection method
	if (connection._request_pipeline.empty())
	{
		if (connection.isPersistent())
			connection.setStatus(CO_ISDONE);
		else
			connection.closeConnection();
	}
}

void	handle_connection(ConnectionClass& connection)
{
	int send_ret;
	int retVal = 0;
	std::vector<HttpRequest>	RequestPipeline;

//	std::cout << "connection server on port : " << connection._server->_port << std::endl;
//	retVal = connection.receiveRequest(RequestPipeline);
	if (retVal == -1)
	{
		connection.closeConnection();
		return;
	}
	else if (retVal == 0)
	{
		std::cout << "connection closed by client" << std::endl;
		connection.closeConnection();
		return;
	}
	print_pipeline(RequestPipeline, connection);
	send_ret = connection.sendResponse("HTTP/1.1 200 OK\r\nContent-length: 52\r\n\r\n<html><body><h1>Welcome to Webser</h1></body></html>");
	if (send_ret == -1)
		std::perror("send");
	if (!connection.isPersistent())
		connection.closeConnection();

}
