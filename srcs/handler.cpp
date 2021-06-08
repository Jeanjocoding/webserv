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

#include "ConnectionClass.hpp"

void	handle_connection(ConnectionClass& connection)
{
	int send_ret;
	std::pair<int, std::string>	request_infos;

	std::cout << "connection server on port : " << connection._server->_port << std::endl;
	request_infos = connection.receiveRequest();
	if (request_infos.first == -1)
		return;
	else if (request_infos.first == 0)
	{
		std::cout << "connection closed by client" << std::endl;
		if (connection.closeConnection() == -1)
			perror("close");
		return;
	}
	std::cout << "message received by server: " << request_infos.second << std::endl;
	send_ret = connection.sendResponse("HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Welcome to Webser</h1></body></html>\r\n");
	if (send_ret == -1)
		perror("send");
}
