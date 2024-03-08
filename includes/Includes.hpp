/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Includes.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:22:15 by slippert          #+#    #+#             */
/*   Updated: 2024/03/08 12:30:51 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "UserInfo.hpp"

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
