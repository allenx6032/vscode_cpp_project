/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   analyze_cmd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/27 13:49:58 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Returns the absolute path of a command and saves the exit_status.
*/
static char	*absolute_path_exit_status(int exit_status, char *absolute_path1)
{
	char	*absolute_path2;

	absolute_path2 = convert_cmd_to_path(absolute_path1);
	free(absolute_path1);
	if (absolute_path2)
		return (absolute_path2);
	else
	{
		g_minishell->exit_status = exit_status;
		free(absolute_path2);
		return (NULL);
	}
}

/*
	1. Tests if a path is executable and returned it.
	! otherwise ->
	2. Searches the path of a command and return it (if exist).
	! otherwise ->
	3. Returns NULL.
*/
static char	*get_absolute_path(char *cmd)
{
	char	*absolute_path1;

	absolute_path1 = ft_strdup(cmd);
	if (!absolute_path1)
		fatal_error(E_MALLOC);
	if (access(absolute_path1, F_OK) != 0)
		return (absolute_path_exit_status(127, absolute_path1));
	else if (access(absolute_path1, X_OK) != 0)
		return (absolute_path_exit_status(126, absolute_path1));
	return (absolute_path1);
}

/*
	Determines if the string being analyzed by the lexing is a builtin or a command.
*/
int	analyze_cmd(t_list *lst)
{
	t_node	*node;
	char	*absolute_path1;

	node = ft_lstlast(g_minishell->nodes)->content;
	node->builtin_id = get_builtin_id(((t_parsed_input *)lst->content)->input);
	if (node->builtin_id != 0)
		return (0);
		absolute_path1 = get_absolute_path(((t_parsed_input *)lst->\
		content)->input);
	if (absolute_path1 != NULL)
	{
		node->path = ft_strdup(absolute_path1);
		node->parsed_cmd = ft_arradd_str(node->parsed_cmd, \
		absolute_path1);
		free(absolute_path1);
	}
	else
		node->error = 1;
	return (1);
}
