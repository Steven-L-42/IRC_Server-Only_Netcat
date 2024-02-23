/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:38 by slippert          #+#    #+#             */
/*   Updated: 2024/02/23 18:05:45 by slippert         ###   ########.fr       */
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
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw(std::runtime_error("Error: socket"));

	_ServerAddress.sin_family = AF_INET;
	_ServerAddress.sin_addr.s_addr = INADDR_ANY;
	_ServerAddress.sin_port = htons(_port);

	if ((_bind = bind(_socket, (struct sockaddr *)&_ServerAddress, sizeof(_ServerAddress))) == -1)
		throw(std::runtime_error("Error: bind"));

	if ((_listen = listen(_socket, _maxConnections)) == -1)
		throw(std::runtime_error("Error: listen"));

	std::cout << "~ Server is running... ~\n" << std::endl;

	int clients = 0;

	while (1)
	{
		_ClientAddrLen = sizeof(_ClientAddress);
		if ((_accept = accept(_socket, (struct sockaddr *)&_ClientAddress, &_ClientAddrLen)) == -1)
			throw(std::runtime_error("Error: accept"));
		clients++;
		User newUser;
		newUser.id = _accept;
		newUser.username = itoa(clients);
		Clients[_accept] = newUser;
		std::cout << "Client " << clients << " joined the channel" << std::endl;

		pthread_t client_thread;
		pthread_create(&client_thread, NULL, handle_client, (void *)&Clients[_accept]);
		pthread_detach(client_thread);
	}
}

void *Server::handle_client(void *arg)
{
	User user = *((User *)arg);
	while (1)
	{
		char buffer[1024];
		ssize_t bytes_received = recv(user.id, buffer, sizeof(buffer), 0);
		if (bytes_received == -1)
			throw(std::runtime_error("Error: recv"));
		else if (bytes_received == 0)
		{
			std::cout << user.username << " disconnected." << std::endl;

			break;
		}

		buffer[bytes_received] = '\0';
		if (bytes_received == 1 && buffer[0] == '\n')
			continue;

		std::vector<std::string> commands = split_buffer(buffer);

		if (commands.size() == 2)
		{
			if (commands[0] == "NICK")
				user.username = commands[1];
		}

		std::vector<std::string>::iterator it;
		std::cout << user.username << ": ";
		for (it = commands.begin(); it != commands.end(); it++)
			std::cout << *it << " ";
		std::cout << std::endl;
	}
	close(user.id);
	return NULL;
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