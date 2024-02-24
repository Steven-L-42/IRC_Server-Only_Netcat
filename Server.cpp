/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:38 by slippert          #+#    #+#             */
/*   Updated: 2024/02/24 12:40:29 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port) : _port(port)
{
}

Server::~Server()
{
	close(_socket);
}

void Server::start()
{
	srvInit();
	srvLstn();
}

void Server::srvInit()
{
	funnyUsernames.push("Fröhlicher Frosch");
	funnyUsernames.push("Tanzende Tomate");
	funnyUsernames.push("Kichernder Kranich");
	funnyUsernames.push("Lustige Libelle");
	funnyUsernames.push("Strahlende Schildkröte");
	funnyUsernames.push("Witzige Wurzel");
	funnyUsernames.push("Kichernde Kaktus");
	funnyUsernames.push("Tanzende Gurke");
	funnyUsernames.push("Lachender Wal");
	funnyUsernames.push("Trockener Humor");
	funnyUsernames.push("Gewürzgurke97");

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

void Server::srvLstn()
{
	std::cout << blue << "~ Server is running ~ " << res << std::endl;
	std::cout << std::endl;
	fd_set read_set;
	struct timeval timeout;

	while (1)
	{
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;
		FD_ZERO(&read_set);
		FD_SET(_socket, &read_set);

		int ret;
		if ((ret = select(_socket + 1, &read_set, NULL, NULL, &timeout)) == -1)
			throw(std::runtime_error("Error: select"));
		else if (ret == 0)
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
				userJoined(_clientSocket);
			}
		}
		recvSignal();
		sendSignal();
	}
}

void Server::recvSignal()
{
	for (_itClient = Clients.begin(); _itClient != Clients.end(); _itClient++)
	{
		char buffer[1024];
		ssize_t bytes_received = recv(_itClient->second.socket, buffer, sizeof(buffer), 0);
		if (bytes_received == -1)
			continue;
		else if (bytes_received == 0)
		{
			std::cout << blue + _itClient->second.username + white + " disconnected." << std::endl;
			close(_itClient->second.socket);
			_itClient->second.socket = -1;
		}

		buffer[bytes_received] = '\0';
		if (bytes_received == 1 && buffer[0] == '\n')
			continue;

		std::vector<std::string> commands = split_buffer(buffer);

		if (commands.size() == 2)
		{
			if (commands[0] == "NICK")
			{
				std::cout << blue << "Server ➤ " << white << "User: " << _itClient->second.username << res << " has changed his name to ";
				_itClient->second.username = commands[1];
				std::cout << white << _itClient->second.username << res << std::endl;
				continue;
			}
		}
		std::string userPrefix = green + _itClient->second.username + " ➤ " + res;
		UserInfo info;
		info.socket = _itClient->second.socket;
		info.username = _itClient->second.username;
		info.message = userPrefix + std::string(buffer);
		Messages[_itClient->second.socket] = info;
	}
}

void Server::sendSignal()
{
	for (_itMessages = Messages.begin(); _itMessages != Messages.end(); _itMessages++)
	{
		for (_itReceiver = Clients.begin(); _itReceiver != Clients.end(); _itReceiver++)
		{
			if (_itMessages->second.socket == _itReceiver->second.socket)
				continue;
			ssize_t bytes_send = send(_itReceiver->second.socket, _itMessages->second.message.c_str(), _itMessages->second.message.size(), 0);
			if (bytes_send == -1)
				throw(std::runtime_error("Error: send"));
		}
	}
	Messages.clear();
}

void Server::userJoined(int clientSocket)
{
	UserInfo newUser;
	newUser.socket = clientSocket;
	newUser.username = funnyUsernames.top();
	newUser.message = blue + newUser.username + green + " joined the channel\n" + res;
	Clients[clientSocket] = newUser;
	Messages[clientSocket] = newUser;
	funnyUsernames.pop();
	std::cout << newUser.message;
}

std::string Server::itoa(int i)
{
	std::ostringstream oss;
	oss << i;
	if ((oss.fail()))
		throw(std::runtime_error("Error: oss"));
	return (oss.str());
}

std::string Server::trim_whitespace(std::string str)
{
	size_t start = str.find_first_not_of(" \n");
	if (start == std::string::npos)
		return str;
	size_t end = str.find_last_not_of(" \n");
	return str.substr(start, end - start + 1);
}

std::vector<std::string> Server::split_buffer(const char *buffer)
{
	std::vector<std::string> commands;
	std::stringstream strStream(buffer);
	std::string command;
	while (std::getline(strStream, command, ' '))
		commands.push_back(trim_whitespace(command));
	return (commands);
}
