/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MsgSystem.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:32:05 by slippert          #+#    #+#             */
/*   Updated: 2024/03/25 08:31:45 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/MsgSystem.hpp"

MsgSystem::MsgSystem()
{
}

MsgSystem::~MsgSystem()
{
}

void MsgSystem::recvSignal()
{
	for (_itClient = Clients.begin(); _itClient != Clients.end(); _itClient++)
	{
		char buffer[1024];

		ssize_t bytes_received = recv(_itClient->first, buffer, sizeof(buffer), 0);
		std::string currentTime = Helper::getCurTime();

		if (bytes_received == -1)
			continue;
		else if (bytes_received == 0)
		{
			std::string msg = res + std::string("User: ") + _itClient->second.textColor + _itClient->second.username + res + " disconnected." + res;
			std::cout << SERVER_PREFIX << msg << std::endl;
			UserInfo info;
			info.socket = _itClient->first;
			info.username = _itClient->second.username;
			info.message = SERVER_PREFIX + msg;
			info.channel = _itClient->second.channel;
			info.isLeaving = true;
			info.isCmd = false;
			MultiMessages.insert(std::make_pair(_itClient->first, info));
			close(_itClient->first);
			_itClient->second.socket = -1;
			chatHistory.push_back(info);
			continue;
		}
		buffer[bytes_received] = '\0';
		// std::cout << buffer;

		if (bytes_received == 1 && buffer[0] == '\n')
			continue;
		std::vector<std::string> commands = Helper::split_buffer(buffer);

		if (cmds->checkCommand(_itClient, commands, *this))
			continue;
		if (_itClient->second.channel == 0)
			continue;

		std::string userPrefix = _itClient->second.textColor + _itClient->second.username + " ➤ " + res;
		std::cout << "CH: " << _itClient->second.channel << " " << currentTime + userPrefix + std::string(buffer);
		UserInfo info;
		info.socket = _itClient->first;
		info.username = _itClient->second.username;
		info.message = currentTime + userPrefix + std::string(buffer);
		info.channel = _itClient->second.channel;
		info.escapen = true;
		info.isCmd = false;
		MultiMessages.insert(std::make_pair(_itClient->first, info));
		chatHistory.push_back(info);
	}
}

void MsgSystem::sendSignal()
{
	for (_itMessages = MultiMessages.begin(); _itMessages != MultiMessages.end(); _itMessages++)
	{
		for (_itReceiver = Clients.begin(); _itReceiver != Clients.end(); _itReceiver++)
		{
			if (_itReceiver->second.socket == -1 || _itReceiver->second.channel != _itMessages->second.channel || _itReceiver->second.socket == 3)
				continue;
			if ((_itMessages->second.isCmd) && _itMessages->first != _itReceiver->first)
				continue;

			std::string message;

			if (_itMessages->first == _itReceiver->first && _itMessages->second.escapen)
				message = ANSIESCAPE + _itMessages->second.message;
			else
				message = _itMessages->second.message;

			if (message.back() != '\n')
				message.append("\n");
			ssize_t bytes_send = send(_itReceiver->first, message.c_str(), message.size(), 0);
			if (bytes_send == -1)
				std::cerr << "Error: send msg: " << message;
		}
	}
	MultiMessages.clear();
}

void MsgSystem::sendHistory(int clientSocket, int channel)
{
	iter client = Clients.find(clientSocket);
	if (client != Clients.end())
		for (_itHistory = chatHistory.begin(); _itHistory != chatHistory.end(); _itHistory++)
		{
			if (_itHistory->channel != channel)
				continue;
			std::string message = _itHistory->message;
			if (message.back() != '\n')
				message.append("\n");

			ssize_t bytes_send = send(client->first, message.c_str(), message.size(), 0);

			if (bytes_send == -1)
				std::cerr << "Error: send msg: " << message;
		}
}

void MsgSystem::removeUsers()
{
	iter itRemove;
	for (itRemove = Clients.begin(); itRemove != Clients.end();)
	{
		if (itRemove->second.socket == -1)
			itRemove = Clients.erase(itRemove);
		else
			++itRemove;
	}
}

void MsgSystem::userJoined(int clientSocket, int channel)
{
	std::srand(time(0));
	int rand = std::rand() % 256;

	std::string userPrefix = blue + std::string("Server") + " ➤ " + res;
	UserInfo newUser;
	if (Clients.find(clientSocket) != Clients.end())
	{
		newUser.username = Clients[clientSocket].username;
		newUser.colNbr = Clients[clientSocket].colNbr;
		newUser.textColor = Clients[clientSocket].textColor;
	}
	else
	{
		newUser.username = "Nick#" + Helper::itoa(clientSocket - 3);
		newUser.colNbr = rand;
		newUser.textColor = b_empty + Helper::itoa(rand) + std::string("m");
	}
	newUser.socket = clientSocket;
	newUser.isCmd = false;
	newUser.escapen = false;
	newUser.channel = channel;
	newUser.message = userPrefix + std::string("User: ") + newUser.textColor + newUser.username + res + " joined the " + (channel == 0 ? "server." : "channel.");
	Clients[clientSocket] = newUser;
	MultiMessages.insert(std::make_pair(clientSocket, newUser));

	std::cout << "CH: " << Helper::itoa(channel) << " " << newUser.message << std::endl;
	if (channel != 0)
	{
		sendHistory(clientSocket, channel);
		chatHistory.push_back(newUser);
	}
}
