/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/17 01:41:26 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Executes a command located in a node.
*/
static void	exec_cmd(t_list *lst)
{
	if (execve(((t_node *)lst->content)->path, \
	((t_node *)lst->content)->parsed_cmd, g_minishell->env) == -1)
	{
		g_minishell->exit_status = 127;
		fatal_error(E_CMD_NFND);
	}
}

/*
	Wait the commands are ends to close the output file descriptor
*/
void	wait_parent(void)
{
	int		i;
	t_list	*lst_node;
	int		node_size;
	int		status;

	i = 0;
	lst_node = g_minishell->nodes;
	node_size = ft_lstsize(g_minishell->nodes);
	while (i < node_size)
	{
		close(((t_node *)lst_node->content)->pipe[0]);
		waitpid(((t_node *)lst_node->content)->pid, &status, 0);
		close(((t_node *)lst_node->content)->pipe[1]);
		if (((t_node *)lst_node->content)->builtin_id == 0)
			if (WIFEXITED(status))
				g_minishell->exit_status = WEXITSTATUS(status);
		if (lst_node->next)
			lst_node = lst_node->next;
		i++;
	}
}

/*
	*1. Create a tube for each of the nodes (commands or builtin)
	*2. Creates a fork (cellular mitosis), the child will launch the command or \
		builtin, the parent will wait for the child to finish.
*/
static void	pipe_redirect_dup(t_list *lst_node, int last_node_id)
{
	int	build_id;

	build_id = ((t_node *)lst_node->content)->builtin_id;
	if (pipe(((t_node *)lst_node->content)->pipe) == -1)
		fatal_error(E_PIPE);
	if (build_id == 0 || (build_id > 0 && \
	((t_node *)lst_node->content)->id > 0) | (build_id > 0 && lst_node->next))
	{
		((t_node *)lst_node->content)->pid = fork();
		if (((t_node *)lst_node->content)->pid == -1)
			fatal_error(E_FORK);
	}
	if (((t_node *)lst_node->content)->pid == 0)
	{
		redirect_dup(lst_node);
		if (((t_node *)lst_node->content)->builtin_id != 0)
			exec_builtin((t_node *)lst_node->content, last_node_id);
		else
			exec_cmd(lst_node);
	}
	else
		if (close(((t_node *)lst_node->content)->pipe[1]) == -1)
			fatal_error(E_CLOSE);
}

/*
	Loops on all commands/builtin nodes.

	The child executes the program while the parent waits until the child is done \
	to return control to the main loop.
*/
void	execution(void)
{
	int		i;
	t_list	*lst_node;
	int		node_size;
	int		last_node_id;

	i = 0;
	lst_node = g_minishell->nodes;
	node_size = ft_lstsize(g_minishell->nodes);
	last_node_id = ((t_node *)ft_lstlast(lst_node)->content)->id;
	while (i < node_size)
	{
		if (((t_node *)lst_node->content)->error == 0)
			pipe_redirect_dup(lst_node, last_node_id);
		else if (((t_node *)lst_node->content)->error == 1)
			msg_error(E_CMD_NFND);
		if (lst_node->next)
			lst_node = lst_node->next;
		i++;
	}
	wait_parent();
}
