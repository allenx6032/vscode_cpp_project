/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deglueing.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	add_space_before_after(int i, char c, int pos)
{
	char	s0[3];
	char	*s1;
	char	*s2;
	char	*s3;

	ft_bzero(s0, sizeof(char) * 3);
	ft_memset(s0, ' ', 2);
	if (pos == 1)
		s0[0] = c;
	else
		s0[1] = c;
	s1 = ft_strjoin(s0, &g_minishell->raw_input[i +1]);
	if (!s1)
		fatal_error(E_MALLOC);
	s2 = ft_strndup(g_minishell->raw_input, i);
	if (!s2)
		fatal_error(E_MALLOC);
	s3 = ft_strjoin(s2, s1);
	if (!s3)
		fatal_error(E_MALLOC);
	free(g_minishell->raw_input);
	g_minishell->raw_input = ft_strdup(s3);
	free(s1);
	free(s2);
	free(s3);
}

static void	add_space_metacharacters(int i, int *input_len, char c)
{
	if (g_minishell->raw_input[i] == c)
	{
		if (g_minishell->raw_input[i + 1] != ' ' && \
		g_minishell->raw_input[i + 1] != c)
		{
			add_space_before_after(i, c, 1);
			*input_len = (*input_len + 1);
		}
		if (i != 0 && (g_minishell->raw_input[i - 1] != ' ' && \
		g_minishell->raw_input[i - 1] != c))
		{
			add_space_before_after(i, c, 2);
			*input_len = (*input_len + 1);
		}
	}
}

/*
	!Peel off commands, metacharacters and input / output
	Exemple : "ls|cat|rev>output.txt" is going \
	to become "ls | cat | rev > output.txt"
*/
void	deglueing(void)
{
	int		i;
	int		in_quote[2];
	int		input_len;

	i = 0;
	ft_bzero(in_quote, sizeof(int) * 2);
	input_len = ft_strlen(g_minishell->raw_input);
	while (input_len > i)
	{
		if (escape_quote(&i, in_quote, 0) == 1)
		{
			i++;
			continue ;
		}
		else if (escape_quote(&i, in_quote, 1) == 1)
		{
			i++;
			continue ;
		}
		add_space_metacharacters(i, &input_len, '>');
		add_space_metacharacters(i, &input_len, '<');
		add_space_metacharacters(i, &input_len, '|');
		i++;
	}
}
