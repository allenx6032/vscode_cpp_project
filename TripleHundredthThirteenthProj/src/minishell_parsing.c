/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	interpret_parsed_input(int is_dequoting, t_list *lst_parsed_input, \
t_parsed_input *parsed_input)
{
	if (is_dequoting)
		parsed_input->interpret = 0;
	else
		parsed_input->interpret = 1;
	lst_parsed_input->content = parsed_input;
}

/*
	Expand environment values and dequoting
*/
static void	expand_lst_parsed_input(void)
{
	t_list			*lst_parsed_input;
	char			*s1;
	char			*s2;
	int				is_dequoting;
	t_parsed_input	*parsed_input;

	lst_parsed_input = g_minishell->lst_parsed_input;
	while (lst_parsed_input)
	{
		s1 = ft_strdup(lst_parsed_input->content);
		if (!s1)
			fatal_error(E_MALLOC);
		free(lst_parsed_input->content);
		s2 = expand_dequoting_string(s1, &is_dequoting);
		if (!s2)
			fatal_error(E_MALLOC);
		free(s1);
		parsed_input = ft_calloc(1, sizeof(t_parsed_input));
		parsed_input->input = s2;
		interpret_parsed_input(is_dequoting, lst_parsed_input, parsed_input);
		lst_parsed_input = lst_parsed_input->next;
	}
}

/*
	Transform raw input into linked list of programs / arguments
*/
static void	convert_raw_input_lst(void)
{
	char	*raw_imput_trimed;

	if (ft_strcmp(g_minishell->raw_input, "") == 0)
		g_minishell->lst_parsed_input = NULL;
	else
	{
		raw_imput_trimed = ft_strtrim(g_minishell->raw_input, " ");
		if (raw_imput_trimed[0] == 0)
		{
			g_minishell->lst_parsed_input = NULL;
			free(raw_imput_trimed);
			return ;
		}
		g_minishell->lst_parsed_input = input_parsing(raw_imput_trimed);
		free(raw_imput_trimed);
	}
}

/*
	ft_split() will convert the user's commands into words arrays which \
	will then be analyzed by the lexer.
*/
int	parsing(void)
{
	if (g_minishell->raw_input != NULL)
	{
		if (g_minishell->raw_input != NULL && \
		ft_strlen(g_minishell->raw_input) != 0)
			add_history(g_minishell->raw_input);
		if (check_valid_quote() != 1)
		{
			g_minishell->exit_status = 258;
			msg_error(E_QUOTE);
			return (0);
		}
		if (check_valid_metacharacters() != 1)
		{
			g_minishell->exit_status = 258;
			msg_error(E_SYNTAX);
			return (0);
		}
		deglueing();
		convert_raw_input_lst();
		expand_lst_parsed_input();
	}
	if (g_minishell->lst_parsed_input == NULL || \
		g_minishell->lst_parsed_input->content == NULL)
		return (0);
	return (1);
}
