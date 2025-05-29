/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dequoting2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/05 11:50:08 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Split the first part of a string that is before a simple quote
*/
void	squote_start(char *s1, char **expanded_input, \
t_dequoting_expand *de, int *is_dequoting)
{
	char	*s2;
	char	*s3;
	char	*s4;

	if (de->s_quote_pos1 == -1 && de->s_quote_pos2 == -1)
	{
		de->s_quote_pos1 = de->i;
		if (de->d_quote_pos1 != de->j)
		{
			s2 = ft_strndup(&s1[de->j], de->s_quote_pos1 - de->j);
			s3 = analyze_expand_input(s2);
			free(s2);
			s4 = *expanded_input;
			*expanded_input = ft_strjoin(*expanded_input, s3);
			free(s3);
			free(s4);
			*is_dequoting = 1;
		}
	}
	else if (de->s_quote_pos1 != -1 && de->s_quote_pos2 == -1)
		de->s_quote_pos2 = de->i;
}

/*
	Split the part of a string that is in a single quote
*/
void	squote_inside(char *s1, char **expanded_input, \
t_dequoting_expand *de, int *is_dequoting)
{
	char	*s2;
	char	*s4;

	if (de->s_quote_pos1 != -1 && de->s_quote_pos2 != -1)
	{
		s2 = ft_strndup(&s1[de->s_quote_pos1 + 1], \
		de->s_quote_pos2 - de->s_quote_pos1 -1);
		s4 = *expanded_input;
		*expanded_input = ft_strjoin(*expanded_input, s2);
		free(s2);
		free(s4);
		de->s_quote_pos1 = -1;
		de->s_quote_pos2 = -1;
		de->j = de->i +1;
		de->l_quote_type = 2;
		*is_dequoting = 1;
	}
}

/*
	Split the last part of a string that is after a simple quote
*/
void	squote_end(char *s1, char **expanded_input, t_dequoting_expand *de)
{
	char	*s2;
	char	*s3;
	char	*s4;

	s2 = ft_strndup(&s1[de->j], de->i - de->s_quote_pos2);
	s3 = analyze_expand_input(s2);
	free(s2);
	s4 = *expanded_input;
	*expanded_input = ft_strjoin(*expanded_input, s3);
	free(s3);
	free(s4);
}

/*
	Structure initialization of dequoting_expand struct
*/
void	init_dequoting_expand(t_dequoting_expand *de)
{
	de->i = 0;
	de->j = 0;
	de->d_quote_pos1 = -1;
	de->d_quote_pos2 = -1;
	de->s_quote_pos1 = -1;
	de->s_quote_pos2 = -1;
	de->l_quote_type = 0;
}
