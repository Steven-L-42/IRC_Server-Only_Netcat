/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:38 by slippert          #+#    #+#             */
/*   Updated: 2024/02/26 13:27:07 by slippert         ###   ########.fr       */
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

	forever
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
		removeUsers();
	}
}

void Server::recvSignal()
{
	for (_itClient = Clients.begin(); _itClient != Clients.end(); _itClient++)
	{
		char buffer[1024];

		ssize_t bytes_received = recv(_itClient->first, buffer, sizeof(buffer), 0);
		std::string currentTime = getCurTime();
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
			info.isLeaving = true;
			info.isCmd = false;
			MultiMessages.insert(std::make_pair(_itClient->first, info));
			close(_itClient->first);
			_itClient->second.socket = -1;
			chatHistory.push_back(info.message);
			//_itClient = Clients.erase(_itClient);
			continue;
		}

		buffer[bytes_received] = '\0';
		if (bytes_received == 1 && buffer[0] == '\n')
			continue;

		std::vector<std::string> commands = split_buffer(buffer);

		if (checkCommand(_itClient, commands))
			continue;

		std::string userPrefix = _itClient->second.textColor + _itClient->second.username + " ➤ " + res;
		std::cout << currentTime + userPrefix + std::string(buffer);
		UserInfo info;
		info.socket = _itClient->first;
		info.username = _itClient->second.username;
		info.message = currentTime + userPrefix + std::string(buffer);
		info.escapen = true;
		info.isCmd = false;
		MultiMessages.insert(std::make_pair(_itClient->first, info));
		chatHistory.push_back(info.message);
	}
}

bool Server::checkCommand(iter client, std::vector<std::string> commands)
{
	typedef bool (Server::*CommandFunction)(iter client, std::vector<std::string>);

	std::map<std::string, CommandFunction> commandDispatch;
	commandDispatch["HELP"] = &Server::showCommands;
	commandDispatch["KICK"] = &Server::kickUser;
	commandDispatch["NICK"] = &Server::changeNickname;
	commandDispatch["WHO"] = &Server::listAllUsers;
	commandDispatch["ALL"] = &Server::serverAnnounce;

	std::transform(commands[0].begin(), commands[0].end(), commands[0].begin(), ::toupper);

	for (std::map<std::string, CommandFunction>::iterator it = commandDispatch.begin(); it != commandDispatch.end(); it++)
	{
		if (it->first == commands[0])
		{
			if ((this->*(it->second))(client, commands))
				return (true);
			else
				return (false);
		}
	}
	return (false);
}

bool Server::changeNickname(iter client, std::vector<std::string> commands)
{
	if (commands.size() < 2)
		return (false);
	std::vector<std::string>::iterator iterCmd = commands.begin();
	iterCmd++;
	std::string newName = "";

	for (; iterCmd != commands.end() && iterCmd != commands.begin() + 3; iterCmd++)
		newName += *iterCmd + (iterCmd + 1 != commands.end() ? " " : "");
	newName = trim_whitespace(newName);

	std::string msg = std::string("User: ") + client->second.textColor + client->second.username + res + " has changed his name to " + client->second.textColor + newName + res;

	client->second.username = newName;
	std::cout << SERVER_PREFIX << msg << std::endl;
	UserInfo info;
	info.socket = client->first;
	info.username = client->second.username;
	info.message = SERVER_PREFIX + msg;
	info.escapen = true;
	info.isCmd = false;
	MultiMessages.insert(std::make_pair(client->first, info));
	chatHistory.push_back(info.message);
	return (true);
}

bool Server::listAllUsers(iter client, std::vector<std::string> commands)
{
	if (commands.size() != 1)
		return (false);
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
	return (true);
}

bool Server::showCommands(iter client, std::vector<std::string> commands)
{
	if (commands.size() != 1)
		return (false);
	std::vector<std::string>::iterator it;
	UserInfo info;
	commands[1] = commands[1];
	std::vector<std::string> cmdList;
	cmdList.push_back("HELP" + std::string(res) + "\t- List all available commands. [HELP]");
	cmdList.push_back("KICK" + std::string(res) + "\t- kick called user. [KICK <username>]");
	cmdList.push_back("NICK" + std::string(res) + "\t- change your username. [NICK <new_username>]");
	cmdList.push_back("WHO" + std::string(res) + "\t- List all active users. [WHO]");
	cmdList.push_back("ALL" + std::string(res) + "\t- Send server announcement. [ALL <announcement>]");

	std::string header = "~ Commandlist ~";
	info.message = blue + header + res;
	info.escapen = false;
	info.isCmd = true;
	MultiMessages.insert(std::make_pair(client->first, info));
	for (it = cmdList.begin(); it != cmdList.end(); it++)
	{
		UserInfo info;
		info.message = orange + *it + res;
		info.escapen = false;
		info.isCmd = true;
		MultiMessages.insert(std::make_pair(client->first, info));
	}
	return (true);
}

bool Server::kickUser(iter client, std::vector<std::string> commands)
{
	if (commands.size() < 2)
		return (false);
	std::string newName = "";
	std::vector<std::string>::iterator iterCmd = commands.begin();
	iterCmd++;

	for (; iterCmd != commands.end() && iterCmd != commands.begin() + 3; iterCmd++)
		newName += *iterCmd + (iterCmd + 1 != commands.end() ? " " : "");
	newName = trim_whitespace(newName);

	iter itSearch;
	for (itSearch = Clients.begin(); itSearch != Clients.end(); itSearch++)
		if (itSearch->second.username == newName)
			break;
	if (itSearch == Clients.end())
		return (false);
	std::string msg = res + std::string("User: ") + itSearch->second.textColor + itSearch->second.username + res + " was kicked by " + client->second.textColor + client->second.username + res;
	std::cout << SERVER_PREFIX << msg << std::endl;
	UserInfo info;
	info.socket = _itClient->first;
	info.username = _itClient->second.username;
	info.message = SERVER_PREFIX + msg;
	info.isLeaving = true;
	info.isCmd = false;
	MultiMessages.insert(std::make_pair(_itClient->first, info));
	close(itSearch->first);
	itSearch->second.socket = -1;
	chatHistory.push_back(info.message);
	return (true);
}

void Server::removeUsers()
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

bool Server::serverAnnounce(iter client, std::vector<std::string> commands)
{
	if (commands.size() < 2)
		return (false);
	std::string newAnnounce = "";
	std::vector<std::string>::iterator iterCmd = commands.begin();
	iterCmd++;

	for (; iterCmd != commands.end(); iterCmd++)
		newAnnounce += *iterCmd + (iterCmd + 1 != commands.end() ? " " : "");
	newAnnounce = trim_whitespace(newAnnounce);

	UserInfo info;
	info.socket = client->first;
	info.message = SERVER_PREFIX + newAnnounce;
	info.isCmd = false;
	MultiMessages.insert(std::make_pair(client->first, info));
	chatHistory.push_back(info.message);
	std::cout << info.message << std::endl;
	return (true);
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

			ssize_t bytes_send = send(_itReceiver->first, message.c_str(), message.size(), 0);
			if (bytes_send == -1)
				std::cerr << "Error: send msg: " << message;
		}
	}
	MultiMessages.clear();
}

void Server::sendHistory(int clientSocket)
{
	iter client = Clients.find(clientSocket);
	for (_itHistory = chatHistory.begin(); _itHistory != chatHistory.end(); _itHistory++)
	{
		std::string message;
		message = *_itHistory;

		if (message.back() != '\n')
			message += "\n";

		ssize_t bytes_send = send(client->first, message.c_str(), message.size(), 0);
		if (bytes_send == -1)
			std::cerr << "Error: send msg: " << message;
	}
}

void Server::userJoined(int clientSocket)
{
	std::string userPrefix = blue + std::string("Server") + " ➤ " + res;
	UserInfo newUser;
	newUser.socket = clientSocket;
	newUser.isCmd = false;
	newUser.username = funnyUsernames.top();
	newUser.textColor = textColors.top();
	newUser.message = userPrefix + std::string("User: ") + newUser.textColor + newUser.username + res + " joined the channel.";
	Clients[clientSocket] = newUser;
	MultiMessages.insert(std::make_pair(clientSocket, newUser));
	funnyUsernames.pop();
	textColors.pop();
	std::cout << newUser.message << std::endl;
	sendHistory(clientSocket);
	chatHistory.push_back(newUser.message);
}

std::string Server::getCurTime()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);

	std::ostringstream ssHour;
	ssHour << (ltm->tm_hour);
	std::ostringstream ssMin;
	ssMin << (ltm->tm_min);
	std::ostringstream ssSec;
	ssSec << (ltm->tm_sec);
	std::string hour((ltm->tm_hour < 10 ? "0" + ssHour.str() : ssHour.str()));
	std::string min((ltm->tm_min < 10 ? "0" + ssMin.str() : ssMin.str()));
	std::string sec((ltm->tm_sec < 10 ? "0" + ssSec.str() : ssSec.str()));
	std::string localTime = "[" + hour + ":" + min + ":" + sec + "] ";
	return (localTime);
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
