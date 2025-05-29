/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   node.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Creates a node, set all values and zero and return it.
*/
static t_node	*create_node(void)
{
	t_node	*node;

	node = ft_calloc(1, sizeof(t_node));
	if (!node)
		fatal_error(E_MALLOC);
	node->builtin_id = 0;
	node->path = NULL;
	node->parsed_cmd = NULL;
	node->error = 0;
	node->fd_in = 0;
	node->fd_out = 0;
	return (node);
}

/*
	Adds a new node to the linked list of nodes.
*/
static void	add_node(void)
{
	t_node	*node;
	t_list	*new_lst;
	t_list	*last_lst;

	if (g_minishell->nodes == NULL)
	{
		node = create_node();
		g_minishell->nodes = ft_lstnew(node);
	}
	else
	{
		node = create_node();
		new_lst = ft_lstnew(node);
		last_lst = ft_lstlast(g_minishell->nodes);
		((t_node *)new_lst->content)->id = \
		((t_node *)last_lst->content)->id + 1;
		ft_lstadd_back(&g_minishell->nodes, new_lst);
	}
}

/*
	Creates a new node in case no node exists or the previous \
	string analyzed by the lexing was a pipe.
*/
void	node_control(int *t)
{
	if (t[0] == 0)
	{
		t[0] = 1;
		add_node();
	}
}
