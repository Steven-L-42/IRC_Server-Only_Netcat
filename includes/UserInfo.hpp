/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserInfo.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 11:21:22 by slippert          #+#    #+#             */
/*   Updated: 2024/03/08 14:45:51 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

struct UserInfo
{
	int socket;
	bool escapen;
	bool isCmd;
	bool isLeaving;
	int channel;
	std::string textColor;
	int colNbr;
	std::string username;
	std::string message;
};