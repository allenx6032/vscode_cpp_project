/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:29:16 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Initializes the default variables of the minishell structure.
*/
void	init_s_minishell(char **argv, char **env)
{
	g_minishell = ft_calloc(1, sizeof(t_minishell));
	g_minishell->dbg = 0;
	g_minishell->exit_status = 0;
	if (argv[1] && ft_strcmp(argv[1], "DEBUG=1") == 0)
		g_minishell->dbg = 1;
	if (argv[1] && ft_strcmp(argv[1], "DEBUG=2") == 0)
		g_minishell->dbg = 2;
	if (argv[1] && ft_strcmp(argv[1], "DEBUG=3") == 0)
		g_minishell->dbg = 3;
	g_minishell->raw_input = NULL;
	g_minishell->lst_parsed_input = NULL;
	g_minishell->nodes = NULL;
	g_minishell->d_fd_in = dup(STDIN_FILENO);
	g_minishell->d_fd_out = dup(STDOUT_FILENO);
	g_minishell->sigint_hd = 0;
	g_minishell->env = env;
	rebuild_necessary_env();
	arr_to_linked_list();
	linked_list_to_arr();
}
