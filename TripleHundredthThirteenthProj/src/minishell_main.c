/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/*
	*1. Initialization of the structure that will be used throughout the project.
	*2. Infinite loop which will be broken by "exit" bultin.
	*3. "readline()" is directly in the parsing, if data are valids, they are \
		sent to the lexing.
	*4. Lexing makes qualify the behavior of the commands which are \
		between the pipes.
	*5. Execution of commands that have been qualified by the lexing.
	!6. Cleaning of part of the structure and we start the loop again.
*/
int	main(int argc, char **argv, char **env)
{
	(void)argc;
	init_s_minishell(argv, env);
	while (g_minishell->exit != 1)
	{
		if (prompting())
		{
			if (parsing())
			{
				lexing();
				if (g_minishell->sigint_hd == 0)
					execution();
				g_minishell->sigint_hd = 0;
			}
			if (g_minishell->dbg == 1)
				dbg_showlexing();
			else if (g_minishell->dbg == 2)
				dbg_showparsing();
			else if (g_minishell->dbg == 3)
				dbg_show_lstenv();
			loop_clear();
		}
	}
	rl_clear_history();
	clear_lst_env(0);
	exit(EXIT_SUCCESS);
}
