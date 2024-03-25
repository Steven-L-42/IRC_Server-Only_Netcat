/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:38 by slippert          #+#    #+#             */
/*   Updated: 2024/03/25 08:29:49 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(int port) : _port(port)
{
}

Server::~Server()
{
	close(_socket);
}

bool Server::Signal = false;
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << "killed Signal" << std::endl;
	Server::Signal = true;
}

void Server::start()
{
	signal(SIGINT, Server::SignalHandler);
	signal(SIGQUIT, Server::SignalHandler);
	srvInit();
	srvLstn();
}

void Server::srvInit()
{

	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw(std::runtime_error("Error: socket"));

	_ServerAddress.sin_family = AF_INET;
	_ServerAddress.sin_addr.s_addr = INADDR_ANY;
	_ServerAddress.sin_port = htons(_port);

	int en = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("Error: setsockopt"));

	if ((bind(_socket, (struct sockaddr *)&_ServerAddress, sizeof(_ServerAddress))) == -1)
		throw(std::runtime_error("Error: bind"));

	if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("Error: fcntl"));

	if ((listen(_socket, _maxConnections)) == -1)
		throw(std::runtime_error("Error: listen"));
}

std::string _serverName = "MyIRCServer";

void Server::srvLstn()
{
	std::cout << blue << "~ Server is running | Port: " << _port << " ~ " << res << std::endl;
	std::cout << std::endl;
	fd_set read_set;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;

	// msgSystem.userJoined(_socket, 0);
	std::cout << _socket << std::endl;
	while (Server::Signal == false)
	{

		FD_ZERO(&read_set);
		FD_SET(_socket, &read_set);

		int ret;
		if ((ret = select(_socket + 1, &read_set, NULL, NULL, &timeout)) == -1 && Server::Signal == false)
			throw(std::runtime_error("Error: select"));
		else if (Server::Signal == false)
		{
			if (ret == 0)
			{
			}
			else
			{
				if (FD_ISSET(_socket, &read_set))
				{
					_ClientAddrLen = sizeof(_ClientAddress);
					int _clientSocket;
					if ((_clientSocket = accept(_socket, (struct sockaddr *)&_ClientAddress, &_ClientAddrLen)) == -1)
						throw(std::runtime_error("Error: accept"));
					msgSystem.userJoined(_clientSocket, 0);
				}
			}
			msgSystem.recvSignal();
			msgSystem.sendSignal();
			msgSystem.removeUsers();
		}
	}
}
