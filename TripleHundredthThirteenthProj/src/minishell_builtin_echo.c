/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/11 11:55:51 by ███████          #+#    #+#             */
/*   Updated: 2022/10/17 02:05:29 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	When we have only words.
*/
static void	echo_mandatory(t_node *node, int i)
{
	while (node->parsed_cmd[i])
	{
		if (node->parsed_cmd[i + 1])
			ft_printf("%s ", node->parsed_cmd[i]);
		else
			ft_printf("%s\n", node->parsed_cmd[i]);
		i++;
	}
}

/*
	When we have one or more -n options.
*/
static void	echo_arg_n(t_node *node)
{
	int		i;
	size_t	j;

	i = 0;
	while (node->parsed_cmd[i])
	{
		j = 1;
		while (node->parsed_cmd[i][j] == 'n')
			j++;
		if (ft_strncmp(node->parsed_cmd[i], "-n", 2) == 0 && \
		ft_strlen(node->parsed_cmd[i]) == j)
			i++;
		else
			break ;
	}
	while (node->parsed_cmd[i])
	{
		if (node->parsed_cmd[i + 1])
			return (echo_mandatory(node, i));
		else
			ft_printf("%s", node->parsed_cmd[i]);
		i++;
	}
}

void	builtin_echo(t_node *node)
{
	if (node->parsed_cmd == NULL)
		ft_printf("\n");
	else if (ft_strncmp(node->parsed_cmd[0], "-n", 2) == 0)
		echo_arg_n(node);
	else
		echo_mandatory(node, 0);
	g_minishell->exit_status = 0;
}
