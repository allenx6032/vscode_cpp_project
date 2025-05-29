/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_parsing.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/16 11:45:53 by ███████           #+#    #+#             */
/*   Updated: 2022/10/01 13:04:12 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Detect if char contains quote
*/
static void	input_quote_detector(char c, int *quote_type, int *have_quote)
{
	if (c == '"' && (*quote_type == 0 || *quote_type == 1))
	{
		*quote_type = 1;
		if (*have_quote == 0)
			*have_quote = 1;
		else
		{
			*have_quote = 0;
			*quote_type = 0;
		}
	}
	if (c == '\'' && (*quote_type == 0 || *quote_type == 2))
	{
		*quote_type = 2;
		if (*have_quote == 0)
			*have_quote = 1;
		else
		{
			*have_quote = 0;
			*quote_type = 0;
		}
	}
}

/*
	Transform the raw input into a linked list, 
	expand the environment variables and remove the quotes.
	i[0] -> first iterator
	i[1] -> second iterator
	q[0] -> have_quote
	q[1] -> quote_type
*/
t_list	*input_parsing(char const *s)
{
	int		i[2];
	int		q[2];
	t_list	*lst_input;
	char	*input;

	lst_input = NULL;
	ft_bzero(i, sizeof(int) * 2);
	ft_bzero(q, sizeof(int) * 2);
	while (s[i[1]])
	{
		input_quote_detector(s[i[1]], &q[1], &q[0]);
		if (s[i[1]] == ' ' && q[0] == 0)
		{
			input = ft_strndup(&s[i[0]], i[1] - i[0]);
			if (ft_strlen(input) != 0)
				lst_input = ft_lst_push(lst_input, input);
			else
				free(input);
			q[1] = 0;
			i[0] = i[1] + 1;
		}
		i[1]++;
	}
	lst_input = ft_lst_push(lst_input, ft_strndup(&s[i[0]], i[1] - i[0]));
	return (lst_input);
}
