/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 17:46:45 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	if_args(t_node *node)
{
	if (node->parsed_cmd[1])
	{
		g_minishell->exit_status = 255;
		msg_error(E_TOMANY_ARG);
	}
	else if (!ft_isnbr(node->parsed_cmd[0]))
	{
		g_minishell->exit_status = 255;
		g_minishell->exit = 1;
		ft_printf("exit\n");
		fatal_error(E_NUMERIC);
	}
	else
	{
		ft_printf("exit\n");
		g_minishell->exit_status = ft_atoi(node->parsed_cmd[0]);
		g_minishell->exit = 1;
	}
}

void	builtin_exit(t_node *node)
{
	if (node->parsed_cmd)
		if_args(node);
	else
	{
		ft_printf("exit\n");
		g_minishell->exit_status = g_minishell->exit_status % 256;
		g_minishell->exit = 1;
	}
	if (!(node->parsed_cmd && node->parsed_cmd[1]))
	{
		loop_clear();
		clear_lst_env(0);
		exit(g_minishell->exit_status);
	}
}
