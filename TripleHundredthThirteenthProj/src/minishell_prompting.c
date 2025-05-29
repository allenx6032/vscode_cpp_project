/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompting.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/09 14:35:59 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	!Readline launch a prompt to wait for user commands.
	User input is copied to a variable as a string.

	The two signal reception functions are for
		SIGINT (ctrl + c)
		SIGQUIT (ctrl + \)

	ctrl + d is not a signal but an EOF
*/

void	asking_input(void)
{
	g_minishell->raw_input = NULL;
	g_minishell->raw_input = readline("minishell$ ");
}

/*
	Combines signaling and readline management.
*/
int	prompting(void)
{
	signal_recept_not_blocking_cmd();
	asking_input();
	signal_recept_blocking_cmd();
	if (g_minishell->raw_input == NULL)
	{
		ft_printf("\033[1A");
		ft_printf("\033[K\rminishell$ exit\n");
		g_minishell->exit = 1;
		return (0);
	}
	return (1);
}
