/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MsgSystem.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:31:45 by slippert          #+#    #+#             */
/*   Updated: 2024/03/17 17:34:56 by slippert         ###   ########.fr       */
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
	std::vector<UserInfo> chatHistory;
	std::vector<UserInfo>::iterator _itHistory;

	void recvSignal();
	void sendSignal();
	void sendHistory(int clientSocket, int channel);
	void removeUsers();
	void userJoined(int clientSocket, int channel);
	void userAnswer1(int clientSocket, int channel);
	void userAnswer2(int clientSocket, int channel);
};