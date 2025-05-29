/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Expand environment variable
*/
static char	*get_env_var(char *s1)
{
	char	*s2;
	t_list	*lst_env;

	s2 = ft_strdup("");
	lst_env = g_minishell->lst_env;
	while (lst_env)
	{
		if (s1[1] == '?' || \
		ft_strcmp(((t_node_env *)lst_env->content)->key, &s1[1]) == 0)
		{
			free(s2);
			if (s1[1] == '?')
				s2 = ft_itoa(g_minishell->exit_status);
			else
				s2 = ft_strdup(((t_node_env *)lst_env->content)->value);
			if (!s2)
				fatal_error(E_MALLOC);
			break ;
		}
		lst_env = lst_env->next;
	}
	return (s2);
}

/*
	Find position of end of environment variable
*/
static int	analyze_end_pos(int j, char *s2)
{
	if (s2[j] == '$')
	{
		j++;
		if (ft_isalpha(s2[j]) || s2[j] == '_')
		{
			j++;
			while (ft_isalpha(s2[j]) || ft_isalnum(s2[j]) || s2[j] == '_')
			{
				j++;
			}
		}
	}
	return (j);
}

/*
	Replace the environment variable in the input string
*/
static char	*expand_var(char *s1, int *i, int *j)
{
	char	*s2;
	char	*s3;
	char	*s4;
	char	*s5;

	*j = analyze_end_pos(*i, s1);
	if (ft_strnstr(s1, "$?", ft_strlen(s1)))
		s3 = ft_strndup(&s1[*i], *j +1 - *i);
	else
		s3 = ft_strndup(&s1[*i], *j - *i);
	if (!s3)
		fatal_error(E_MALLOC);
	s4 = get_env_var(s3);
	if (!s4)
		fatal_error(E_MALLOC);
	s5 = s1;
	s2 = ft_str_repl(s1, s3, s4);
	if (!s1)
		fatal_error(E_MALLOC);
	free(s3);
	free(s4);
	free(s5);
	*i = 0;
	return (s2);
}

/*
	Expand a string with environment variable
*/
char	*analyze_expand_input(char *s1)
{
	int		i;
	int		j;
	char	*s2;

	s2 = ft_strdup(s1);
	i = 0;
	j = 0;
	while (s2[i])
	{
		if (s2[i] == '$')
		{
			s2 = expand_var(s2, &i, &j);
			if (!s2)
				fatal_error(E_MALLOC);
		}
		if (s2[i])
			i++;
	}
	return (s2);
}
