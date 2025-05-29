/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clear.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/09 14:33:36 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Cleaning the listed chain that contains environment variables.
*/
void	clear_lst_env(int type)
{
	t_list	*env_lst;

	if (type == 1)
		loop_clear();
	env_lst = g_minishell->lst_env;
	while (env_lst)
	{
		if (env_lst->content)
		{
			free(((t_node_env *)env_lst->content)->key);
			free(((t_node_env *)env_lst->content)->value);
		}
		env_lst = env_lst->next;
	}
	ft_lstclear(&g_minishell->lst_env, free);
}

/*
	Cleaning the array that contains environment variables.
*/
void	clear_arr_env(void)
{
	int	i;

	i = 0;
	while (g_minishell->env[i])
	{
		free(g_minishell->env[i]);
		i++;
	}
	free(g_minishell->env);
}

/*
	General cleaning of input between two prompts.
*/
static void	clear_parsed_input(void)
{
	t_list	*lst;

	lst = g_minishell->lst_parsed_input;
	if (g_minishell->lst_parsed_input)
	{
		while (lst)
		{
			free(((t_parsed_input *)lst->content)->input);
			lst = lst->next;
		}
		ft_lstclear(&g_minishell->lst_parsed_input, &free);
	}
}

/*
	General cleaning of node in between two prompts.
*/
static void	clear_node(void)
{
	t_list	*lst;

	dup2(g_minishell->d_fd_in, STDIN_FILENO);
	dup2(g_minishell->d_fd_out, STDOUT_FILENO);
	lst = g_minishell->nodes;
	while (lst)
	{
		if (((t_node *)lst->content)->path)
			free(((t_node *)lst->content)->path);
		if (((t_node *)lst->content)->parsed_cmd)
			ft_arr2dfree((void **)((t_node *)lst->content)->parsed_cmd);
		if (((t_node *)lst->content)->fd_in > 0)
			close(((t_node *)lst->content)->fd_in);
		if (((t_node *)lst->content)->fd_out > 0)
			close(((t_node *)lst->content)->fd_out);
		lst = lst->next;
	}
	if (g_minishell->nodes)
		ft_lstclear(&g_minishell->nodes, &free);
}

/*
	General cleaning between two prompts.
*/
void	loop_clear(void)
{
	clear_node();
	clear_parsed_input();
	if (g_minishell->raw_input)
		free(g_minishell->raw_input);
	g_minishell->raw_input = NULL;
}
