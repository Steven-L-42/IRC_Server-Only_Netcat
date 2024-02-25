/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:38 by slippert          #+#    #+#             */
/*   Updated: 2024/02/25 21:33:18 by slippert         ###   ########.fr       */
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

	textColors.push(red);
	textColors.push(yellow);
	textColors.push(cyan);
	textColors.push(magenta);
	textColors.push(black);
	textColors.push(bright_red);
	textColors.push(bright_yellow);
	textColors.push(bright_black);
	textColors.push(bright_cyan);
	textColors.push(bright_magenta);

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

		ssize_t bytes_received = recv(_itClient->first, buffer, sizeof(buffer), 0);
		if (bytes_received == -1)
			continue;
		else if (bytes_received == 0)
		{
			std::string userPrefix = blue + std::string("Server") + " ➤ " + res;
			std::string msg = white + std::string("User: ") + _itClient->second.textColor + _itClient->second.username + res + " disconnected." + res;
			std::cout << msg << std::endl;
			UserInfo info;
			info.socket = _itClient->first;
			info.username = _itClient->second.username;
			info.message = userPrefix + msg;
			info.isLeaving = true;
			MultiMessages.insert(std::make_pair(_itClient->first, info));
			close(_itClient->first);
			_itClient->second.socket = -1;
			continue;
		}

		buffer[bytes_received] = '\0';
		if (bytes_received == 1 && buffer[0] == '\n')
			continue;

		std::vector<std::string> commands = split_buffer(buffer);

		if (checkCommand(_itClient, commands))
			continue;

		std::string userPrefix = _itClient->second.textColor + _itClient->second.username + " ➤ " + res;
		UserInfo info;
		info.socket = _itClient->first;
		info.username = _itClient->second.username;
		info.message = userPrefix + std::string(buffer);
		info.escapen = true;
		MultiMessages.insert(std::make_pair(_itClient->first, info));
	}
}

bool Server::checkCommand(iter client, std::vector<std::string> commands)
{
	typedef void (Server::*CommandFunction)(iter client, std::vector<std::string>);

	std::map<std::string, CommandFunction> commandDispatch;
	commandDispatch["NICK"] = &Server::changeNickname;
	commandDispatch["WHO"] = &Server::listAllUsers;

	std::transform(commands[0].begin(), commands[0].end(), commands[0].begin(), ::toupper);

	for (std::map<std::string, CommandFunction>::iterator it = commandDispatch.begin(); it != commandDispatch.end(); it++)
	{
		if (it->first == commands[0])
		{
			(this->*(it->second))(client, commands);
			return true;
		}
	}
	return (false);
}

void Server::changeNickname(iter client, std::vector<std::string> commands)
{
	std::string userPrefix = blue + std::string("Server") + " ➤ " + res;
	std::string msg = white + std::string("User: ") + client->second.textColor + client->second.username + res + " has changed his name to " + client->second.textColor + commands[1] + res;
	client->second.username = commands[1];
	std::cout << userPrefix << msg << std::endl;
	UserInfo info;
	info.socket = client->first;
	info.username = client->second.username;
	info.message = userPrefix + msg;
	info.escapen = true;
	info.isCmd = false;
	MultiMessages.insert(std::make_pair(client->first, info));
}

void Server::listAllUsers(iter client, std::vector<std::string> commands)
{
	iter it;
	UserInfo info;
	commands[1] = commands[1];
	std::string activeUsers = "~ " + itoa(Clients.size()) + (Clients.size() == 1 ? " active User ~" : " active Users ~");
	info.message = blue + activeUsers + res;
	info.isCmd = true;
	MultiMessages.insert(std::make_pair(client->first, info));
	for (it = Clients.begin(); it != Clients.end(); it++)
	{
		UserInfo info;
		info.message = it->second.textColor + it->second.username + res;
		info.isCmd = true;
		MultiMessages.insert(std::make_pair(client->first, info));
	}
}

void Server::sendSignal()
{

	for (_itMessages = MultiMessages.begin(); _itMessages != MultiMessages.end(); _itMessages++)
	{
		for (_itReceiver = Clients.begin(); _itReceiver != Clients.end(); _itReceiver++)
		{
			if (_itReceiver->second.socket == -1)
				continue;
			if ((_itMessages->second.isCmd) && _itMessages->first != _itReceiver->first)
				continue;

			std::string message;

			if (_itMessages->first == _itReceiver->first && _itMessages->second.escapen)
				message = ANSIESCAPE + _itMessages->second.message;
			else
				message = _itMessages->second.message;

			if (message.back() != '\n')
				message += "\n";

			ssize_t bytes_send = send(_itReceiver->second.socket, message.c_str(), message.size(), 0);
			if (bytes_send == -1)
				std::cerr << "Error: send msg: " << message;
		}
	}
	MultiMessages.clear();
}

void Server::userJoined(int clientSocket)
{
	UserInfo newUser;
	newUser.socket = clientSocket;
	newUser.username = funnyUsernames.top();
	newUser.message = blue + newUser.username + green + " joined the channel" + res;
	newUser.textColor = textColors.top();

	Clients[clientSocket] = newUser;
	MultiMessages.insert(std::make_pair(clientSocket, newUser));
	funnyUsernames.pop();
	textColors.pop();
	std::cout << newUser.message << std::endl;
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
