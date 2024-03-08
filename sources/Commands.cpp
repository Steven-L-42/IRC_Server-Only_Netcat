/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:07:03 by slippert          #+#    #+#             */
/*   Updated: 2024/03/08 13:45:47 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Commands.hpp"

Commands::Commands()
{
}

Commands::~Commands()
{
}

bool Commands::showCommands(iter client, std::vector<std::string> commands, MsgSystem &msgSystem)
{
	if (commands.size() != 1)
		return (false);

	std::vector<std::string>::iterator it;
	UserInfo info;
	commands[1] = commands[1];

	std::vector<std::string> cmdList;
	cmdList.push_back("HELP" + std::string(res) + "\t- List all available commands. [HELP]");
	cmdList.push_back("INFO" + std::string(res) + "\t- List all personal informations. [INFO]");
	cmdList.push_back("KICK" + std::string(res) + "\t- kick called user. [KICK <username>]");
	cmdList.push_back("NICK" + std::string(res) + "\t- change your username. [NICK <new_username>]");
	cmdList.push_back("WHO" + std::string(res) + "\t- List all active users. [WHO]");
	cmdList.push_back("ALL" + std::string(res) + "\t- Send server announcement. [ALL <announcement>]");
	cmdList.push_back("COLORS" + std::string(res) + "\t- List all available colors. [COLORS]");
	cmdList.push_back("COL" + std::string(res) + "\t- set text color from 1 to 255. [COL <nbr>]");

	std::string header = "~ Commandlist ~";
	info.message = blue + header + res;
	info.escapen = false;
	info.isCmd = true;
	msgSystem.MultiMessages.insert(std::make_pair(client->first, info));
	for (it = cmdList.begin(); it != cmdList.end(); it++)
	{
		UserInfo info;
		info.message = orange + *it + res;
		info.escapen = false;
		info.isCmd = true;
		msgSystem.MultiMessages.insert(std::make_pair(client->first, info));
	}
	return (true);
}

bool Commands::kickUser(iter client, std::vector<std::string> commands, MsgSystem &msgSystem)
{
	if (commands.size() < 2)
		return (false);
	iter _itClient;
	std::string newName = "";
	std::vector<std::string>::iterator iterCmd = commands.begin();
	iterCmd++;

	for (; iterCmd != commands.end() && iterCmd != commands.begin() + 3; iterCmd++)
		newName += *iterCmd + (iterCmd + 1 != commands.end() ? " " : "");
	newName = Helper::trim_whitespace(newName);

	iter itSearch;
	for (itSearch = msgSystem.Clients.begin(); itSearch != msgSystem.Clients.end(); itSearch++)
		if (itSearch->second.username == newName)
			break;
	if (itSearch == msgSystem.Clients.end())
		return (false);
	std::string msg = res + std::string("User: ") + itSearch->second.textColor + itSearch->second.username + res + " was kicked by " + client->second.textColor + client->second.username + res;
	std::cout << SERVER_PREFIX << msg << std::endl;
	UserInfo info;
	info.socket = _itClient->first;
	info.username = _itClient->second.username;
	info.message = SERVER_PREFIX + msg;
	info.isLeaving = true;
	info.isCmd = false;
	msgSystem.MultiMessages.insert(std::make_pair(_itClient->first, info));
	close(itSearch->first);
	itSearch->second.socket = -1;
	msgSystem.chatHistory.push_back(info.message);
	return (true);
}

bool Commands::changeColor(iter client, std::vector<std::string> commands, MsgSystem &msgSystem)
{
	int colNbr = std::atoi(commands[1].c_str());
	if (commands.size() != 2 || colNbr < 1 || colNbr > 255)
		return (false);
	std::vector<std::string>::iterator iterCmd = commands.begin();
	iterCmd++;

	std::string newColor(b_empty + commands[1] + "m");
	std::string msg = std::string("User: ") + client->second.textColor + client->second.username + res + " has changed his color to " + newColor + commands[1] + res;
	client->second.colNbr = colNbr;
	client->second.textColor = newColor;
	std::cout << SERVER_PREFIX << msg << std::endl;
	UserInfo info;
	info.socket = client->first;
	info.username = client->second.username;
	info.message = SERVER_PREFIX + msg;
	info.escapen = true;
	info.isCmd = false;
	msgSystem.MultiMessages.insert(std::make_pair(client->first, info));
	msgSystem.chatHistory.push_back(info.message);
	return (true);
}

bool Commands::changeNickname(iter client, std::vector<std::string> commands, MsgSystem &msgSystem)
{
	if (commands.size() < 2)
		return (false);
	iter itNames;
	std::string msgTaken = "Username already taken!\n";
	UserInfo alreadyTaken;
	alreadyTaken.message = orange + std::string("Privat ➤ ") + res + msgTaken + res;
	alreadyTaken.isCmd = true;
	alreadyTaken.escapen = true;
	for (itNames = msgSystem.Clients.begin(); itNames != msgSystem.Clients.end(); itNames++)
	{
		if (commands.size() == 2 && itNames->second.username == commands[1])
		{
			msgSystem.MultiMessages.insert(std::make_pair(client->first, alreadyTaken));
			return (false);
		}
		else if (commands.size() == 3 && itNames->second.username == commands[1] + " " + commands[2])
		{
			msgSystem.MultiMessages.insert(std::make_pair(client->first, alreadyTaken));
			return (false);
		}
	}

	std::vector<std::string>::iterator iterCmd = commands.begin();
	iterCmd++;
	std::string newName = "";

	for (; iterCmd != commands.end() && iterCmd != commands.begin() + 3; iterCmd++)
		newName += *iterCmd + (iterCmd + 1 != commands.end() ? " " : "");
	newName = Helper::trim_whitespace(newName);

	std::string msg = std::string("User: ") + client->second.textColor + client->second.username + res + " has changed his name to " + client->second.textColor + newName + res;

	client->second.username = newName;
	std::cout << SERVER_PREFIX << msg << std::endl;
	UserInfo info;
	info.socket = client->first;
	info.username = client->second.username;
	info.message = SERVER_PREFIX + msg;
	info.escapen = true;
	info.isCmd = false;
	msgSystem.MultiMessages.insert(std::make_pair(client->first, info));
	msgSystem.chatHistory.push_back(info.message);
	return (true);
}

bool Commands::serverAnnounce(iter client, std::vector<std::string> commands, MsgSystem &msgSystem)
{
	if (commands.size() < 2)
		return (false);
	std::string newAnnounce = "";
	std::vector<std::string>::iterator iterCmd = commands.begin();
	iterCmd++;

	for (; iterCmd != commands.end(); iterCmd++)
		newAnnounce += *iterCmd + (iterCmd + 1 != commands.end() ? " " : "");
	newAnnounce = Helper::trim_whitespace(newAnnounce);

	UserInfo info;
	info.socket = client->first;
	info.message = SERVER_PREFIX + newAnnounce;
	info.isCmd = false;
	msgSystem.MultiMessages.insert(std::make_pair(client->first, info));
	msgSystem.chatHistory.push_back(info.message);
	std::cout << info.message << std::endl;
	return (true);
}

bool Commands::checkCommand(iter client, std::vector<std::string> commands, MsgSystem &msgSystem)
{
	typedef bool (Commands::*CommandFunction)(iter client, std::vector<std::string>, MsgSystem &msgSystem);
	std::map<std::string, CommandFunction> commandDispatch;
	commandDispatch["HELP"] = &Commands::showCommands;
	commandDispatch["INFO"] = &Commands::listAllInfo;
	commandDispatch["KICK"] = &Commands::kickUser;
	commandDispatch["NICK"] = &Commands::changeNickname;
	commandDispatch["WHO"] = &Commands::listAllUsers;
	commandDispatch["ALL"] = &Commands::serverAnnounce;
	commandDispatch["COLORS"] = &Commands::listAllColors;
	commandDispatch["COL"] = &Commands::changeColor;

	std::transform(commands[0].begin(), commands[0].end(), commands[0].begin(), ::toupper);

	std::map<std::string, CommandFunction>::iterator it = commandDispatch.find(commands[0]);
	if (it != commandDispatch.end())
	{
		if ((this->*(it->second))(client, commands, msgSystem))
			return true;
		else
			return false;
	}

	return (false);
}

bool Commands::listAllColors(iter client, std::vector<std::string> commands, MsgSystem &msgSystem)
{
	if (commands.size() != 1)
		return (false);
	iter it;
	UserInfo info;
	commands[1] = commands[1];
	std::string activeUsers = "~ Colors from 1 - 255 ~";
	info.message = blue + activeUsers + res;
	info.isCmd = true;
	info.escapen = true;
	msgSystem.MultiMessages.insert(std::make_pair(client->first, info));

	std::string msg = "";
	for (int i = 1; i <= 255; i++)
	{
		msg += std::string(b_empty + Helper::itoa(i)) + "m" + std::string(Helper::itoa(i)) + res + "\t";
		if (i % 5 == 0)
		{
			UserInfo info;
			info.isCmd = true;
			info.escapen = false;
			info.message = msg;
			msgSystem.MultiMessages.insert(std::make_pair(client->first, info));
			msg = "";
		}
	}
	return (true);
}

bool Commands::listAllInfo(iter client, std::vector<std::string> commands, MsgSystem &msgSystem)
{
	if (commands.size() != 1)
		return (false);
	iter it;
	UserInfo info;
	commands[1] = commands[1];
	std::string activeUsers = "~ Your Credentials ~";
	info.message = blue + activeUsers + res;
	info.isCmd = true;
	info.escapen = true;
	msgSystem.MultiMessages.insert(std::make_pair(client->first, info));

	std::string msg = "";

	msg += "Socket ID: " + Helper::itoa(client->first) + "\n";
	msg += "Username: " + client->second.username + "\n";
	msg += "Color: " + client->second.textColor + Helper::itoa(client->second.colNbr) + "\n" + res;

	UserInfo info2;
	info2.isCmd = true;
	info2.escapen = false;
	info2.message = msg;
	msgSystem.MultiMessages.insert(std::make_pair(client->first, info2));

	return (true);
}

bool Commands::listAllUsers(iter client, std::vector<std::string> commands, MsgSystem &msgSystem)
{
	if (commands.size() != 1)
		return (false);
	iter it;
	UserInfo info;
	commands[1] = commands[1];
	std::string activeUsers = "~ " + Helper::itoa(msgSystem.Clients.size()) + (msgSystem.Clients.size() == 1 ? " active User ~" : " active Users ~");
	info.message = blue + activeUsers + res;
	info.escapen = true;
	info.isCmd = true;
	msgSystem.MultiMessages.insert(std::make_pair(client->first, info));
	for (it = msgSystem.Clients.begin(); it != msgSystem.Clients.end(); it++)
	{
		UserInfo info;
		info.message = it->second.textColor + it->second.username + res;
		info.isCmd = true;
		info.escapen = false;
		msgSystem.MultiMessages.insert(std::make_pair(client->first, info));
	}
	return (true);
}
