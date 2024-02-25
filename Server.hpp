/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:42 by slippert          #+#    #+#             */
/*   Updated: 2024/02/25 21:15:02 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#include <unistd.h>
#include <vector>
#include <stack>
#include <map>
#include <sstream>
#include <algorithm>
#include <fcntl.h>

#define ANSIESCAPE "\033[F\033[J"

#define green "\033[1m\033[32m"
#define blue "\033[1m\033[34m"
#define white "\033[1m\033[37m"
#define res "\033[0m"

#define red "\033[1m\033[31m"
#define yellow "\033[1m\033[33m"
#define cyan "\033[1m\033[36m"
#define magenta "\033[1m\033[35m"
#define black "\033[1m\033[30m"
#define bright_red "\033[1m\033[91m"
#define bright_yellow "\033[1m\033[93m"
#define bright_cyan "\033[1m\033[96m"
#define bright_magenta "\033[1m\033[95m"
#define bright_black "\033[1m\033[90m"

struct UserInfo
{
	int socket;
	bool escapen;
	bool isCmd;
	bool isLeaving;
	std::string textColor;
	std::string username;
	std::string message;
};

class Server
{
private:
	typedef std::map<int, UserInfo>::iterator iter;
	const int _port;
	static const int _maxConnections = 128;

	int _socket;

	std::map<int, UserInfo> Clients;
	std::map<int, UserInfo> Messages;
	std::multimap<int, UserInfo> MultiMessages;
	std::stack<std::string> funnyUsernames;
	std::stack<std::string> textColors;

	iter _itClient;
	iter _itReceiver;
	iter _itMessages;

	struct sockaddr_in _ServerAddress;
	struct sockaddr_in _ClientAddress;
	socklen_t _ClientAddrLen;

	std::string itoa(int i);
	static std::vector<std::string> split_buffer(const char *buffer);
	static std::string trim_whitespace(std::string str);

	void srvInit();
	void srvLstn();
	void userJoined(int clientSocket);
	void recvSignal();
	void sendSignal();
	void changeNickname(iter client, std::vector<std::string> commands);
	void listAllUsers(iter client, std::vector<std::string> commands);
	bool checkCommand(iter client, std::vector<std::string> commands);

public:
	Server(int port);
	~Server();
	void start();
};
