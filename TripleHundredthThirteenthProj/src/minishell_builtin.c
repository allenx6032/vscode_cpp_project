/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:36:23 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Builtin execution (based on builtin ID).
	!If the builtin is in a pipe, it will exit the fork \
	otherwise it will not stop the program.
*/
void	exec_builtin(t_node	*node, int last_node_id)
{
	if (node->fd_in > 0)
		dup2(node->fd_in, STDIN_FILENO);
	if (node->fd_out > 0)
		dup2(node->fd_out, STDOUT_FILENO);
	if (node->builtin_id == 1)
		builtin_exit(node);
	if (node->builtin_id == 2)
		builtin_echo(node);
	if (node->builtin_id == 3)
		builtin_pwd(node);
	if (node->builtin_id == 4)
		builtin_cd(node);
	if (node->builtin_id == 5)
		builtin_env(node);
	else if (node->builtin_id == 6)
		builtin_export(node);
	else if (node->builtin_id == 7)
		builtin_unset(node);
	if (node->id > 0 || (node->id == 0 && last_node_id > 0))
	{
		g_minishell->exit = 1;
		loop_clear();
		clear_lst_env(0);
		exit(g_minishell->exit_status);
	}
}

/*
	Returns the builtin ID, if the command is not a builtin, return zero.
*/
int	get_builtin_id(char *str)
{
	if (ft_strcmp("exit", str) == 0)
		return (1);
	else if (ft_strcmp("echo", str) == 0)
		return (2);
	else if (ft_strcmp("pwd", str) == 0)
		return (3);
	else if (ft_strcmp("cd", str) == 0)
		return (4);
	else if (ft_strcmp("env", str) == 0)
		return (5);
	else if (ft_strcmp("export", str) == 0)
		return (6);
	else if (ft_strcmp("unset", str) == 0)
		return (7);
	return (0);
}
