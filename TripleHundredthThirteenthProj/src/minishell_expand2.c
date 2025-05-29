/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/03 22:03:11 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Split the last part of a string that is after a quote
*/
void static	dsend_quote_end(char *s1, char **expanded_input,
t_dequoting_expand *de)
{
	if (de->l_quote_type == 1)
		dquote_end(s1, expanded_input, de);
	else if (de->l_quote_type == 2)
		squote_end(s1, expanded_input, de);
	else
		noquote_end(s1, expanded_input, de);
}

/*
	Expand environment values and dequoting a string
*/
char	*expand_dequoting_string(char *s1, int *is_dequoting)
{
	t_dequoting_expand	de;
	int					len;
	char				*expanded_input;

	init_dequoting_expand(&de);
	len = ft_strlen(s1);
	expanded_input = ft_strdup("");
	*is_dequoting = 0;
	while (de.i <= len)
	{
		if (s1[de.i] == '"' && de.s_quote_pos1 == -1)
		{
			dquote_start(s1, &expanded_input, &de, is_dequoting);
			dquote_inside(s1, &expanded_input, &de, is_dequoting);
		}
		else if (s1[de.i] == '\'' && de.d_quote_pos1 == -1)
		{
			squote_start(s1, &expanded_input, &de, is_dequoting);
			squote_inside(s1, &expanded_input, &de, is_dequoting);
		}
		else if (s1[de.i] == 0 && len != de.j)
			dsend_quote_end(s1, &expanded_input, &de);
		de.i++;
	}
	return (expanded_input);
}
