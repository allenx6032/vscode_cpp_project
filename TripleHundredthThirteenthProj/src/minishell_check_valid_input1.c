/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_valid_input1.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Detect if quotes and double quotes are closed correctly
*/
int	check_valid_quote(void)
{
	int	i[3];

	ft_bzero(i, sizeof(int) * 3);
	while (g_minishell->raw_input[i[0]])
	{
		if (g_minishell->raw_input[i[0]] == '\'' && i[2] != 1)
		{
			if (i[1] == 0)
				i[1] = 1;
			else
				i[1] = 0;
		}
		if (g_minishell->raw_input[i[0]] == '"' && i[1] != 1)
		{
			if (i[2] == 0)
				i[2] = 1;
			else
				i[2] = 0;
		}
		i[0]++;
	}
	if (i[1] == 0 && i[2] == 0)
		return (1);
	return (0);
}

/*
	Checks if the meta character is valid compared to the previous character.
*/
static int	is_metachar_valid(char *input, int i)
{
	if (input[i] == '>' && input[i - 1] == '>')
		if (input[i - 2] == '>' || input[i - 2] == '<' || \
		input[i - 2] == '|')
			return (0);
	if (input[i] == '<' && input[i - 1] == '<')
		if (input[i - 2] == '>' || input[i - 2] == '<')
			return (0);
	if (input[i] == '>')
		if (input[i - 1] == '<' || input[i - 1] == '|')
			return (0);
	if (input[i] == '<')
		if (input[i - 1] == '>')
			return (0);
	if (input[i] == '|')
		if (input[i - 1] == '>' || input[i - 1] == '<' || \
		input[i - 1] == '|')
			return (0);
	return (1);
}

/*
	Checks that the character(s) before a metacharacters are valid.
*/
static int	analyze_before_metachar(int i, char *input)
{
	if (ft_strlen(input) == 1 && (input[i] == '>' || input[i] == '<' \
	|| input[i] == '|'))
		return (0);
	if (ft_strlen(input) == 2 && (ft_strcmp(input, ">>") == 0 \
	|| ft_strcmp(input, "<<") == 0 || ft_strcmp(input, "||") == 0))
		return (0);
	if (i > 1)
		if (is_metachar_valid(input, i) == 0)
			return (0);
	return (1);
}

/*
	Check if the metacharacters are placed in the right places.
*/
int	check_valid_metacharacters(void)
{
	int		i;
	int		in_quote[2];
	int		input_len;

	i = 0;
	ft_bzero(in_quote, sizeof(int) * 2);
	input_len = ft_strlen(g_minishell->raw_input);
	while (input_len > i)
	{
		if (escape_quote(&i, in_quote, 0))
			i++;
		if (escape_quote(&i, in_quote, 1))
			i++;
		if (analyze_before_metachar(i, g_minishell->raw_input) == 0)
			return (0);
		if (analyze_after_metachar(i, g_minishell->raw_input) == 0)
			return (0);
		if (analyze_output_exist(i, g_minishell->raw_input) == 0)
			return (0);
		i++;
	}
	return (1);
}
