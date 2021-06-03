#include <sys/socket.h>
#include <cstring>
#include <stdio.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "ConnectionClass.hpp"

void	launch_client()
{
	int		cli_sock;
	struct sockaddr_in	serv_addr;
//	struct sockaddr_in	client_addr;
	char  buf_response[50];
	char  buf_to_input[1001];
	int call_ret;
	int read_ret;
	std::pair<int, std::string> 	request_infos;

	int i;

	i = 0;
	if ((cli_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket");
	if ((call_ret = inet_pton(AF_INET, "127.0.0.1", &(serv_addr.sin_addr))) == 0)
		perror("inet_pton");
	serv_addr.sin_port = htons(80);
	if ((call_ret = connect(cli_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
		perror("connect");
	ConnectionClass connection(cli_sock);
	while (1)
	{
		std::memset(buf_to_input, '\0', 1000);
		std::cout << "write the message you want to send" << std::endl;
		read_ret = read(0, buf_to_input, 1000);
		buf_to_input[read_ret] = '\0';
		std::cout << "message sent: " << buf_to_input << std::endl;
		if ((call_ret = write(cli_sock, buf_to_input, read_ret)) < 0)
			perror("write");
		std::memset(buf_response, '\0', 50);
		request_infos = connection.receiveRequest();
		if (request_infos.first == -1)
			return;
		else if (request_infos.first == 0)
		{
			std::cout << "connection closed by server" << std::endl;
			if (connection.closeConnection() == -1)
				perror("close");
			return;
		}
		std::cout << "message received by client: " << request_infos.second << std::endl;
	}
	return;

}

int main(void)
{
	launch_client();
}