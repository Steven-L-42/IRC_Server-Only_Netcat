/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MsgSystem.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:31:45 by slippert          #+#    #+#             */
/*   Updated: 2024/03/08 13:35:49 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Includes.hpp"
#include "Helper.hpp"
#include "Commands.hpp"

class Commands;

class MsgSystem
{
public:
	Commands *cmds;
	MsgSystem();
	~MsgSystem();

	typedef std::map<int, UserInfo>::iterator iter;
	iter _itClient;
	iter _itReceiver;
	iter _itMessages;
	std::map<int, UserInfo> Clients;
	std::map<int, UserInfo> Messages;
	std::multimap<int, UserInfo> MultiMessages;
	std::vector<std::string> chatHistory;
	std::vector<std::string>::iterator _itHistory;

	void recvSignal();
	void sendSignal();
	void sendHistory(int clientSocket);
	void removeUsers();
	void userJoined(int clientSocket);
};