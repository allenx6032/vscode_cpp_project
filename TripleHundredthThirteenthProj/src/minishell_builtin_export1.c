/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/30 13:53:36 by ███████          #+#    #+#             */
/*   Updated: 2022/10/09 15:39:00 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	fill_node_env(char *parsed_cmd, t_node_env	*node_env)
{
	int			pos;

	pos = ft_strchr_i(parsed_cmd, '=');
	if (pos > -1)
	{
		node_env->key = ft_strndup(parsed_cmd, pos);
		if (!node_env->key)
			fatal_error(E_MALLOC);
		node_env->value = ft_strdup(&ft_strchr(parsed_cmd, '=')[1]);
		if (!node_env->value)
			fatal_error(E_MALLOC);
		node_env->displayable = 1;
	}
	else
	{
		node_env->key = ft_strdup(parsed_cmd);
		if (!node_env->key)
			fatal_error(E_MALLOC);
		node_env->value = ft_strdup("");
		if (!node_env->value)
			fatal_error(E_MALLOC);
		node_env->displayable = 0;
	}
}

static int	edit_exist_env(t_node_env *node_env, char *parsed_cmd)
{
	t_list		*lst_env;

	lst_env = g_minishell->lst_env;
	while (lst_env)
	{
		if (ft_strcmp(node_env->key, \
		((t_node_env *)lst_env->content)->key) == 0)
		{
			if (ft_strlen(node_env->value) != 0 || \
			ft_strlen(node_env->key) == (size_t)ft_strchr_i(parsed_cmd, '='))
			{
				free(((t_node_env *)lst_env->content)->value);
				((t_node_env *)lst_env->content)->value = \
				ft_strdup(node_env->value);
			}
			if (!((t_node_env *)lst_env->content)->value)
				fatal_error(E_MALLOC);
			if (((t_node_env *)lst_env->content)->displayable == 0)
				((t_node_env *)lst_env->content)->displayable = \
				node_env->displayable;
			return (1);
		}
		lst_env = lst_env->next;
	}
	return (0);
}

/*
	1. Create a new environment variable. If it already exists, it will be edited.
	2. If an environment variable has a variable (or an equal), it will be marked \
	 displayable, otherwise it will be marked not displayable
*/
int	push_env(char *parsed_cmd)
{
	t_node_env	*node_env;
	int			valid_env_key;

	node_env = ft_calloc(1, sizeof(t_node_env));
	if (!node_env)
		fatal_error(E_MALLOC);
	fill_node_env(parsed_cmd, node_env);
	valid_env_key = is_valid_env_key(node_env->key);
	if (edit_exist_env(node_env, parsed_cmd) == 1 || valid_env_key == 0)
	{
		free(node_env->key);
		free(node_env->value);
		free(node_env);
	}
	else
		g_minishell->lst_env = ft_lst_push(g_minishell->lst_env, node_env);
	if (valid_env_key == 0)
		return (0);
	return (1);
}

void	builtin_export(t_node *node)
{
	int		i;

	i = -1;
	if (!node->parsed_cmd)
		print_env_alphabetically();
	else
	{
		while (node->parsed_cmd[++i])
		{
			if (push_env(node->parsed_cmd[i]))
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
}
