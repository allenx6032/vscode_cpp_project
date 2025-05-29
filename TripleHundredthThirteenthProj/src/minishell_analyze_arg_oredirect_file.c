/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   analyze_arg_oredirect_file.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Redirects program output to a file in trunc mode.
*/
static int	o_trunc_redirect(t_list *lst, int *t, t_node *node)
{
	if (t[4] == 1)
	{
		t[4] = 0;
		if (node->fd_out)
			close(node->fd_out);
		node->fd_out = open(((t_parsed_input *)lst->content)->input, \
			O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (node->fd_out < 0)
		{
			perror(E_);
			node->error = 2;
		}
		return (1);
	}
	if (((t_parsed_input *)lst->content)->interpret == 1 && \
	ft_strcmp(">", ((t_parsed_input *)lst->content)->input) == 0)
	{
		t[4] = 1;
		return (1);
	}
	return (0);
}

/*
	Redirects program output to a file in append mode.
*/
static int	o_append_redirect(t_list *lst, int *t, t_node *node)
{
	if (t[5] == 1)
	{
		t[5] = 0;
		if (node->fd_out)
			close(node->fd_out);
		node->fd_out = open(((t_parsed_input *)lst->content)->input, \
			O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (node->fd_out < 0)
		{
			perror(E_);
			node->error = 2;
		}
		return (1);
	}
	if (((t_parsed_input *)lst->content)->interpret == 1 && \
	ft_strcmp(">>", ((t_parsed_input *)lst->content)->input) == 0)
	{
		t[5] = 1;
		return (1);
	}
	return (0);
}

/*
	Identifies if the string being analyzed by lexing is an argument, \
	a redirection sign or an output file and will save it in the node.

	- Return 0 when the analyzed string is an argument.
	- Return 1 when the analyzed string is output (>) sign or a writeable file.

	*- reminder -
	t[4] to 1 identify that the previous word was output (>) redirect.
	t[5] to 1 identify that the previous word was an output (>>) redirect.
*/
int	analyze_arg_oredirect_file(t_list *lst, int *t)
{
	t_node	*node;
	char	**arr1;

	node = ft_lstlast(g_minishell->nodes)->content;
	if (ft_strlen(((t_parsed_input *)lst->content)->input) == 0)
		return (0);
	if (o_trunc_redirect(lst, t, node))
		return (1);
	if (o_append_redirect(lst, t, node))
		return (1);
	arr1 = node->parsed_cmd;
	node->parsed_cmd = ft_arradd_str(arr1, \
	((t_parsed_input *)lst->content)->input);
	ft_arr2dfree((void **)arr1);
	return (0);
}
