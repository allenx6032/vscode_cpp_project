/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/24 21:53:36 by ███████          #+#    #+#             */
/*   Updated: 2022/10/09 14:40:29 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_with_formatting(void)
{
	t_list	*lst;

	lst = g_minishell->lst_env;
	while (lst != NULL)
	{
		if (((t_node_env *)lst->content)->displayable == 1)
			ft_printf("declare -x %s=\"%s\"\n", ((t_node_env *)lst->content)->key, \
			((t_node_env *)lst->content)->value);
		else
			ft_printf("declare -x %s\n", ((t_node_env *)lst->content)->key);
		lst = lst->next;
	}
}

static void	traverse_sublist(t_list **min, t_list **temp)
{
	t_list	*lst;

	*min = *temp;
	lst = (*temp)->next;
	while (lst)
	{
		if (ft_strcmp(((t_node_env *)(*min)->content)->key, \
		((t_node_env *)lst->content)->key) > 0)
			*min = lst;
		lst = lst->next;
	}
}

/*
	*1. The first loop traverse the list.
	*2. The second loop traverse the unsorted sublist.
	*3. After the second loop, data is swapped.

	lst1 -> min
	lst2 -> tmp lst
*/
void	print_env_alphabetically(void)
{
	t_list	*lst1;
	t_list	*lst2;
	char	*x;
	char	*y;

	lst2 = g_minishell->lst_env;
	while (lst2)
	{
		traverse_sublist(&lst1, &lst2);
		x = ((t_node_env *)lst2->content)->key;
		((t_node_env *)lst2->content)->key = ((t_node_env *)lst1->content)->key;
		((t_node_env *)lst1->content)->key = x;
		y = ((t_node_env *)lst2->content)->value;
		((t_node_env *)lst2->content)->value = \
		((t_node_env *)lst1->content)->value;
		((t_node_env *)lst1->content)->value = y;
		lst2 = lst2->next;
	}
	print_with_formatting();
	g_minishell->exit_status = 0;
}

int	is_valid_env_key(char *key)
{
	int	i;

	if (ft_isdigit(key[0]))
		return (0);
	if (ft_isalpha(key[0]) || key[0] == '_')
	{
		i = 1;
		while (key[i])
		{
			if (ft_isalnum(key[i]) || key[i] == '_' || key[i] != '=')
				i++;
			else
				return (0);
		}
		return (1);
	}
	else
		return (0);
}
