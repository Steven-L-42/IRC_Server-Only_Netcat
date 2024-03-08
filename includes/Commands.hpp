/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:06:52 by slippert          #+#    #+#             */
/*   Updated: 2024/03/08 15:06:45 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Includes.hpp"
#include "Helper.hpp"
#include "MsgSystem.hpp"

class MsgSystem;

class Commands
{
private:
	typedef std::map<int, UserInfo>::iterator iter;
	typedef bool (Commands::*CommandFunction)(iter client, std::vector<std::string>, MsgSystem &msgSystem);
	bool joinChannel(iter client, std::vector<std::string> commands, MsgSystem &msgSystem);
	bool showCommands(iter client, std::vector<std::string> commands, MsgSystem &msgSystem);
	bool kickUser(iter client, std::vector<std::string> commands, MsgSystem &msgSystem);
	bool changeColor(iter client, std::vector<std::string> commands, MsgSystem &msgSystem);
	bool changeNickname(iter client, std::vector<std::string> commands, MsgSystem &msgSystem);
	bool serverAnnounce(iter client, std::vector<std::string> commands, MsgSystem &msgSystem);
	bool listAllUsers(iter client, std::vector<std::string> commands, MsgSystem &msgSystem);
	bool listAllInfo(iter client, std::vector<std::string> commands, MsgSystem &msgSystem);
	bool listAllColors(iter client, std::vector<std::string> commands, MsgSystem &msgSystem);

public:
	Commands();
	~Commands();
	bool checkCommand(iter client, std::vector<std::string> commands, MsgSystem &msgSystem);
};