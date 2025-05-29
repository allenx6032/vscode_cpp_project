/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Ge the position of the next character which is not between a single or \
	double quote.
*/
int	escape_quote(int *i, int *in_quote, int quote_type)
{	
	char	quote;

	if (quote_type == 1)
		quote = '\'';
	else
		quote = '"';
	if (g_minishell->raw_input[*i] == quote && in_quote[1 - quote_type] == 0)
	{
		in_quote[quote_type] = 1;
		*i = (*i + 1);
		while (g_minishell->raw_input[*i])
		{
			if (g_minishell->raw_input[*i] == quote)
			{
				in_quote[quote_type] = 0;
				return (1);
			}
			*i = (*i + 1);
		}
		return (0);
	}
	return (0);
}
