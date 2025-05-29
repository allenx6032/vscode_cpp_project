/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_valid_input2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_redirect_valid(char *input, int *j, int is_pipe)
{
	*j = (*j + 1);
	while (input[*j] == ' ')
		*j = (*j + 1);
	if (input[*j - 1])
	{
		if (is_pipe)
		{
			if (input[*j - 1])
				if (input[*j] == 0)
					return (0);
		}
		else
		{
			if (input[*j] == 0 || input[*j] == '|')
				return (0);
		}
	}
	return (1);
}

int	analyze_output_exist(int i, char *input)
{
	int	j;

	j = i;
	if (input[j] && input[j] == '>')
	{
		if (is_redirect_valid(input, &j, 0) == 0)
			return (0);
	}
	else if (input[j] && input[j] == '<')
	{
		if (is_redirect_valid(input, &j, 0) == 0)
			return (0);
	}
	else if (input[j] && input[j] == '|')
	{
		if (is_redirect_valid(input, &j, 1) == 0)
			return (0);
	}
	return (1);
}

/*
	Check that the output file is possible.
*/
static int	analyze_after_metachar_oredirect(int j, char *input)
{
	while (input[j +1] && input[++j +1] == ' ')
		;
	if (input[j] != '"' && input[j] != '\'')
	{
		if ((g_minishell->raw_input[j -2] != '"' && \
		g_minishell->raw_input[j -2] != '\'') && \
		((g_minishell->raw_input[j +1] == '>') || input[j +1] == '<'))
			return (1);
	}
	return (0);
}

/*
	Check that the input file is possible.
*/
int	analyze_after_metachar_iredirect(int j, char *input)
{
	while (input[j +1] && input[++j +1] == ' ')
		;
	if (input[j] != '"' && input[j] != '\'')
	{
		while (input[j +1] && input[++j +1] == '<')
			;
		if ((g_minishell->raw_input[j -2] != '"' && \
		g_minishell->raw_input[j -2] != '\'') && \
		((g_minishell->raw_input[j +1] == '>') || input[j +1] == '<')
		)
			return (1);
	}
	return (0);
}

/*
	Checks that the character(s) after a metacharacters are valid.
*/
int	analyze_after_metachar(int i, char *input)
{
	int	j;

	j = i;
	if (input[j] && input[++j] == '|')
	{
		while (input[++j] == ' ')
			;
		if (input[j] == '|')
			return (0);
	}
	if (input[j] && input[++j] == '>' )
		if (analyze_after_metachar_oredirect(j, input))
			return (0);
	if (input[j] && input[++j] == '<')
		if (analyze_after_metachar_iredirect(j, input))
			return (0);
	return (1);
}
