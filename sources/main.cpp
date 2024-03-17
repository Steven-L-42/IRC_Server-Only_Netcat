/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: slippert <slippert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/23 14:57:09 by slippert          #+#    #+#             */
/*   Updated: 2024/03/17 16:00:56 by slippert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

bool checkPort(char *_port, int *port)
{
	*port = static_cast<int>(std::atoi(_port));
	if (*port >= 1024 && *port <= 49151)
		return (true);
	else
		return (false);
}

int main(int argc, char **argv)
{
	int port = 0;
	if (argc != 2 || (argc == 2 && !checkPort(argv[1], &port)))
		return (1);

	Server server(port);
	try
	{
		server.start();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}
