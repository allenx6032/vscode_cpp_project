/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/09 14:18:49 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Create the "PWD" and "_" environment variables when no environment \
	variable exists.
*/
void	rebuild_necessary_env(void)
{
	char	*cwd;
	char	*pwd_env;

	if (g_minishell->env[0] == NULL)
	{
		push_env("_=/usr/bin/env");
		cwd = getcwd(NULL, 0);
		if (!cwd)
			fatal_error(E_MALLOC);
		pwd_env = ft_strjoin("PWD=", cwd);
		if (!pwd_env)
			fatal_error(E_MALLOC);
		free(cwd);
		push_env(pwd_env);
		free(pwd_env);
	}
}

/*
	Transform the array of environment variables into a linked list \
	of environment variables.
*/
void	arr_to_linked_list(void)
{
	int			i;
	t_node_env	*content;
	int			pos;

	i = 0;
	while (g_minishell->env[i] != NULL)
	{
		pos = ft_strchr_i(g_minishell->env[i], '=');
		content = ft_calloc(1, sizeof(t_node_env));
		if (!content)
			fatal_error(E_MALLOC);
		content->displayable = 1;
		content->key = ft_strndup(g_minishell->env[i], pos);
		if (!content->key)
			fatal_error(E_MALLOC);
		content->value = ft_strdup(&ft_strchr(g_minishell->env[i], '=')[1]);
		if (!content->value)
			fatal_error(E_MALLOC);
		g_minishell->lst_env = ft_lst_push(g_minishell->lst_env, content);
		i++;
	}
}

/*
	Copy the content of the linked list to string of environment variables.
*/
static char	*copy_lst_env_var(t_node_env *node)
{
	char	*item1;
	char	*item2;

	item1 = ft_strjoin(node->key, "=");
	if (!item1)
		fatal_error(E_MALLOC);
	item2 = ft_strjoin(item1, (node->value));
	if (!item2)
		fatal_error(E_MALLOC);
	free(item1);
	return (item2);
}

/*
	Transform the linked list of environment variables into an array of \
	environment variables
*/
void	linked_list_to_arr(void)
{
	int		i;
	char	**env_arr;
	t_list	*lst;

	lst = g_minishell->lst_env;
	env_arr = ft_calloc(sizeof(char *), ft_lstsize(lst) + 1);
	if (!env_arr)
		fatal_error(E_MALLOC);
	i = 0;
	while (lst)
	{
		if (((t_node_env *)lst->content)->displayable == 1)
		{
			env_arr[i++] = copy_lst_env_var((t_node_env *)lst->content);
			if (env_arr[i])
				fatal_error(E_MALLOC);
		}
		lst = lst->next;
	}
	env_arr[i] = 0;
	g_minishell->env = env_arr;
}
