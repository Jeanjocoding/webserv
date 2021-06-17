/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asablayr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/06 21:54:40 by asablayr          #+#    #+#             */
/*   Updated: 2021/06/08 12:56:51 by asablayr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utility>
#include <iostream>
#include <vector>

#include "webserv.hpp"
#include "ConnectionClass.hpp"

void	handle_connection(ConnectionClass& connection)
{
	int send_ret;
	int retVal;
	std::vector<HttpRequest>	RequestPipeline;

	std::cout << "connection server on port : " << connection._server->_port << std::endl;
	retVal = connection.receiveRequest(RequestPipeline);
	if (retVal == -1)
		return;
	else if (retVal == 0)
	{
		std::cout << "connection closed by client" << std::endl;
		if (connection.closeConnection() == -1)
			perror("close");
		return;
	}
//	HttpRequest request(request_infos.second);
//	std::cout << "message received by server: " << request_infos.second << std::endl;
	send_ret = connection.sendResponse("HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Welcome to Webser</h1></body></html>\r\n");
	if (send_ret == -1)
		perror("send");
}
