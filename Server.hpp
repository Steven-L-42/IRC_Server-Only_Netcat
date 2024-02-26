/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:42 by slippert          #+#    #+#             */
/*   Updated: 2024/02/26 19:05:37 by slippert         ###   ########.fr       */
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
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include <sstream>
#include <algorithm>
#include <fcntl.h>

#define ANSIESCAPE "\033[F\033[J"

#define green "\033[32;1m"
#define blue "\033[34;1m"
#define white "\033[37;1m"
#define res "\033[0m"

#define b_empty "\033[1m\033[38;5;"
#define bold "\033[1m"
#define red "\033[38;5;202m"
#define yellow "\033[33;1m"
#define cyan "\033[36;1m"
#define magenta "\033[35;1m"
#define black "\033[30;1m"
#define orange "\033[1m\033[38;5;202m"

#define SERVER_PREFIX "\033[1m\033[34mServer ➤ \033[0m"

#define forever for (; /*ever*/;)
struct UserInfo
{
	int socket;
	bool escapen;
	bool isCmd;
	bool isLeaving;
	std::string textColor;
	int colNbr;
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
	std::vector<std::string> chatHistory;

	iter _itClient;
	iter _itReceiver;
	iter _itMessages;
	std::vector<std::string>::iterator _itHistory;

	struct sockaddr_in _ServerAddress;
	struct sockaddr_in _ClientAddress;
	socklen_t _ClientAddrLen;

	std::string itoa(int i);
	std::string getCurTime();
	static std::vector<std::string> split_buffer(const char *buffer);
	static std::string trim_whitespace(std::string str);

	void srvInit();
	void srvLstn();
	void userJoined(int clientSocket);
	void recvSignal();
	void sendSignal();
	void sendHistory(int clientSocket);
	void removeUsers();
	bool showCommands(iter client, std::vector<std::string> commands);
	bool kickUser(iter client, std::vector<std::string> commands);
	bool changeColor(iter client, std::vector<std::string> commands);
	bool changeNickname(iter client, std::vector<std::string> commands);
	bool serverAnnounce(iter client, std::vector<std::string> commands);
	bool listAllUsers(iter client, std::vector<std::string> commands);
	bool listAllInfo(iter client, std::vector<std::string> commands);
	bool listAllColors(iter client, std::vector<std::string> commands);
	bool checkCommand(iter client, std::vector<std::string> commands);

public:
	Server(int port);
	~Server();
	void start();
};
