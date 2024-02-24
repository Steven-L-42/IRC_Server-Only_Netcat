/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:42 by slippert          #+#    #+#             */
/*   Updated: 2024/02/24 12:31:05 by slippert         ###   ########.fr       */
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

#define green "\033[1m\033[32m"
#define blue "\033[1m\033[34m"
#define white "\033[1m\033[37m"
#define res "\033[0m"

struct UserInfo
{
	int socket;
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
	std::stack<std::string> funnyUsernames;

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

public:
	Server(int port);
	~Server();
	void start();
};
