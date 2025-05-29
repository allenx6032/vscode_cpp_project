/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dbg_show.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Displays args of parsed command.
*/
static void	dbg_showargs(char **parsed_cmd)
{
	int	i;

	i = 0;
	if (parsed_cmd != NULL)
	{
		while (parsed_cmd[i])
		{
			ft_printf("  args: %s \n", parsed_cmd[i]);
			i++;
		}
	}
}

/*
	Displays a nodes debug menu.
*/
void	dbg_showlexing(void)
{
	t_list	*lst;

	lst = g_minishell->nodes;
	ft_printf("-------- 🚧 DEBUG 🚧 --------\n");
	while (lst)
	{
		ft_printf("         🧩 node: %d \n", ((t_node *)lst->content)->id);
		ft_printf("  builtin: %d \n", ((t_node *)lst->content)->builtin_id);
		ft_printf("  path: %s \n", ((t_node *)lst->content)->path);
		dbg_showargs(((t_node *)lst->content)->parsed_cmd);
		ft_printf("  fd_in : %d \n", ((t_node *)lst->content)->fd_in);
		ft_printf("  fd_out : %d \n", ((t_node *)lst->content)->fd_out);
		ft_printf("  pipe_in : %d \n", ((t_node *)lst->content)->pipe[0]);
		ft_printf("  pipe_out : %d \n", ((t_node *)lst->content)->pipe[1]);
		ft_printf("  error: %d \n", ((t_node *)lst->content)->error);
		ft_printf("\n");
		lst = lst->next;
	}
	ft_printf("-----------------------------\n\n");
}

/*
	Displays parsing debug menu.
*/
void	dbg_showparsing(void)
{
	int		lst_size;
	t_list	*lst;

	lst_size = ft_lstsize(g_minishell->lst_parsed_input);
	lst = g_minishell->lst_parsed_input;
	ft_printf("-------- 🚧 DEBUG 🚧 --------\n");
	ft_printf("         🧲 parsing: %d \n", lst_size);
	while (lst)
	{
		if (((t_parsed_input *)lst->content)->interpret == 1)
			ft_printf("{%s}", ((t_parsed_input *)lst->content)->input);
		else
			ft_printf("[%s]", ((t_parsed_input *)lst->content)->input);
		lst = lst->next;
	}
	if (g_minishell->lst_parsed_input == NULL)
		ft_printf("{NULL}");
	ft_printf("\n\n");
	ft_printf("-----------------------------\n\n");
}

/*
	Displays env lst
*/
void	dbg_show_lstenv(void)
{
	t_list	*lst;
	int		i;

	lst = g_minishell->lst_env;
	i = 0;
	ft_printf("-------- 🚧 DEBUG 🚧 --------\n");
	ft_printf("         💈 lst_env: \n");
	while (lst)
	{
		if (i == 0)
			ft_printf("--- \n");
		ft_printf("  key : %s \n", ((t_node_env *)lst->content)->key);
		ft_printf("  value : %s \n", ((t_node_env *)lst->content)->value);
		ft_printf("  displayable : %d \n", \
		((t_node_env *)lst->content)->displayable);
		if (lst->next)
			ft_printf("--- \n");
		lst = lst->next;
		i++;
	}
	ft_printf("-----------------------------\n\n");
}
