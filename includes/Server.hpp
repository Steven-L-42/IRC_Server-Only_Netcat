/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 18:04:42 by slippert          #+#    #+#             */
/*   Updated: 2024/03/17 15:40:26 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../includes/Includes.hpp"
#include "../includes/Helper.hpp"
#include "../includes/Commands.hpp"
#include "../includes/MsgSystem.hpp"

class Server
{
private:
	MsgSystem msgSystem;

	int _socket;
	const int _port;
	static const int _maxConnections = 128;

	struct sockaddr_in _ServerAddress;
	struct sockaddr_in _ClientAddress;
	socklen_t _ClientAddrLen;

	static bool Signal;
	static void SignalHandler(int signum);
	void srvInit();
	void srvLstn();

public:
	Server(int port);
	~Server();
	void start();
};
