/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 21:54:40 by asablayr          #+#    #+#             */
/*   Updated: 2021/07/18 18:03:50 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utility>
#include <iostream>
#include <vector>
#include <cstdio>
#include <vector>

#include "webserv.hpp"
#include "ConnectionUtils.hpp"
#include "ConnectionClass.hpp"

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

void	answer_connection(ConnectionClass& connection)
{
	std::cout << "answering on fd " << connection._socketNbr << std::endl;
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
//	print_pipeline(RequestPipeline, connection);
	send_ret = connection.sendResponse("HTTP/1.1 200 OK\r\nContent-length: 52\r\n\r\n<html><body><h1>Welcome to Webser</h1></body></html>");
	if (send_ret == -1)
		std::perror("send");
	if (!connection.isPersistent())
		connection.closeConnection();

}
