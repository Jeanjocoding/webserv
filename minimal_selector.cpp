#include <sys/socket.h>
#include <cstring>
#include <stdio.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <iostream>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <map>
#include <utility>
#include "ConnectionClass.hpp"

void	handle_connection(ConnectionClass& connection)
{
	int send_ret;
	std::pair<int, std::string>	request_infos;

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
	send_ret = connection.sendResponse("HTTP/1.1 200 OK\r\n\r\n<html><body><h1>Welcome to Webser</h1></body></html>");
	if (send_ret == -1)
		perror("send");
}

void	launch_server()
{
	int					s;
	int					s2;
	struct sockaddr_in	serv_address;
	int					call_ret;
	int					opt_val;
	int 				pollindex;
	int					pollmax;
	int					poll_ret;
	struct pollfd		poll_tab[1024];
	std::map<int, ConnectionClass>	connection_map; 



	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket");
	if (fcntl(s, F_SETFL, O_NONBLOCK) < 0)
		perror("fcntl");
	opt_val = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val)) < 0)
		perror("setsockopt");
	std::memset((char*)&serv_address, '\0', sizeof(serv_address));

    serv_address.sin_family = AF_INET;
    serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
   	serv_address.sin_port = htons(80);
	if ((call_ret = bind(s, (struct sockaddr*)&serv_address, sizeof(serv_address))) < 0)
		perror("bind");
	if ((call_ret = listen(s, 1024)) < 0)
		perror("listen");
	pollindex = 0;
	pollmax = 0;
	while (1)
	{
		if ((s2 = accept(s, NULL, NULL)) < 0)
//			perror("accept");
			;
		else
		{
			if (fcntl(s2, F_SETFL, O_NONBLOCK) < 0)
				perror("fcntl");
			poll_tab[pollmax].fd = s2;
			poll_tab[pollmax].events = POLLIN;
			connection_map.insert(std::pair<int, ConnectionClass>(s2, ConnectionClass(s2)));
			pollmax++;
		}
		poll_ret = poll(poll_tab, pollmax, 0);
		if (poll_ret < 0)
			perror("poll_ret");
		else if (poll_ret)
		{
			while (pollindex < pollmax)
			{
				if	(poll_tab[pollindex].fd > 0)
				{
					if (poll_tab[pollindex].revents & POLLIN)
						handle_connection(connection_map[poll_tab[pollindex].fd]);
					if (poll_tab[pollindex].revents & POLLHUP)
					{
						std::cout << "closing fd " << poll_tab[pollindex].fd << std::endl;
						if (close(poll_tab[pollindex].fd) < -1)
							perror("close");
						poll_tab[pollindex].fd = -1;
					}
					poll_tab[pollindex].revents = 0;
				}
				pollindex++;
			}
			pollindex = 0;
		}
	}
}

int main(void)
{
	launch_server();
}