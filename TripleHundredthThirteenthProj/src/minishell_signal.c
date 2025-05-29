/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████          #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	* rl_on_new_line(void)
		- Tell the update routines that we have moved onto a new (empty) line,
		usually after outputting a newline.
	* rl_replace_line(const char *text, int clear_undo)
		- Replace the contents of rl_line_buffer with text.
		- The point and mark are preserved, if possible.
		- If clear_undo is non-zero, the undo list associated with the current 
        line is cleared.
	* rl_redisplay(void)
		- Change what's displayed on the screen to reflect the current contents 
        of rl_line_buffer.

	-----

	A SIGINT (ctrl + c) was received in an empty or non empty prompt \
	but not after a blocking command was executed:
*/
void	signal_handler_not_blocking_cmd(int signum)
{
	(void)signum;
	ft_printf("\n");
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_minishell->exit_status = 1;
}

/*
	A blocking command (eg. cat) was executed 
	before these signals:
		ctrl + c (SIGINT)
		ctrl + \ (SIGQUIT)
*/
void	signal_handler_blocking_cmd(int signum)
{
	if (signum == SIGINT)
	{
		g_minishell->exit_status = 130;
		ft_printf("\n");
	}
	else if (signum == SIGQUIT)
	{
		g_minishell->exit_status = 131;
		ft_printf("Quit: 3\n");
	}
}

/*
    Execution of signal() function allows reception of specific \
	signals (first param) at any time during the running \
	of the loop it is placed in.

	When a signal is received, the function handler \
	(second param) is executed.

	Signal reception after empty or non empty prompt \
	but not after a blocking command was executed:
		ctrl + c (SIGINT)
		ctrl + \ (SIGQUIT)
*/
void	signal_recept_not_blocking_cmd(void)
{
	signal(SIGINT, signal_handler_not_blocking_cmd);
	signal(SIGQUIT, SIG_IGN);
}

/*
	Signal reception after a blocking command was executed:
		ctrl + c (SIGINT)
		ctrl + \ (SIGQUIT)
*/
void	signal_recept_blocking_cmd(void)
{
	signal(SIGINT, signal_handler_blocking_cmd);
	signal(SIGQUIT, signal_handler_blocking_cmd);
}
