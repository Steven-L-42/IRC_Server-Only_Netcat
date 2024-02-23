/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:42 by slippert          #+#    #+#             */
/*   Updated: 2024/02/23 18:04:43 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>

class Server
{
private:
	const int _port;
	static const int _maxConnections = 1;

	int _socket;
	int _bind;
	int _listen;
	int _accept;

	struct User
	{
		int id;
		std::string username;
	};
	std::map<int, User> Clients;

	struct sockaddr_in _ServerAddress;
	struct sockaddr_in _ClientAddress;
	socklen_t _ClientAddrLen;
	std::string itoa(int i);
	static void *handle_client(void *arg);
	static std::vector<std::string> split_buffer(const char *buffer);
	static std::string trim_whitespace(std::string str);

public:
	Server(int port);
	~Server();
	void start();
};
