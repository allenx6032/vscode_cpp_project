/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/30 13:53:36 by ███████          #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	del_node_lst(t_list *lst)
{
	free(((t_node_env *)lst->content)->key);
	free(((t_node_env *)lst->content)->value);
	if (lst->previous != NULL)
	{
		lst->previous->next = lst->next;
		if (lst->next != NULL)
		{
			lst->next->previous = lst->previous;
			g_minishell->lst_env = \
			ft_lstfirst(g_minishell->lst_env->next);
		}
	}
	else
	{
		lst->next->previous = NULL;
		g_minishell->lst_env = g_minishell->lst_env->next;
	}
	ft_lstdelone(lst, free);
	return ;
}

static void	analyze_del_node_env(char *parsed_cmd, t_list *lst)
{
	while (lst)
	{
		if (((t_node_env *)lst->content)->key[0] == '_' && \
		((t_node_env *)lst->content)->key[1] == 0)
		{
			lst = lst->next;
			continue ;
		}
		if (!ft_strcmp(parsed_cmd, ((t_node_env *)lst->content)->key))
		{
			del_node_lst(lst);
			lst = g_minishell->lst_env;
			continue ;
		}
		lst = lst->next;
	}
}

int	del_env(char *parsed_cmd)
{
	t_list	*lst;
	int		i;

	i = 0;
	lst = g_minishell->lst_env;
	if (!is_valid_env_key(parsed_cmd))
		return (0);
	while (parsed_cmd[i++])
		if (parsed_cmd[i] == '=')
			return (0);
	analyze_del_node_env(parsed_cmd, lst);
	return (1);
}

void	builtin_unset(t_node *node)
{
	int		i;

	i = -1;
	if (node->parsed_cmd)
	{
		while (node->parsed_cmd[++i])
		{
			if (del_env(node->parsed_cmd[i]))
			{
				clear_arr_env();
				linked_list_to_arr();
				g_minishell->exit_status = 0;
			}
			else
			{
				g_minishell->exit_status = 1;
				msg_error(E_BADIDENTF);
			}
		}
	}
	else
		g_minishell->exit_status = 0;
}
