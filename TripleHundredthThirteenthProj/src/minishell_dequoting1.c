/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dequoting1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/05 11:50:08 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Split the first part of a string that is before a double quote
*/
void	dquote_start(char *s1, char **expanded_input, \
t_dequoting_expand *de, int *is_dequoting)
{
	char	*s2;
	char	*s3;
	char	*s4;

	if (de->d_quote_pos1 == -1 && de->d_quote_pos1 == -1)
	{
		de->d_quote_pos1 = de->i;
		if (de->d_quote_pos1 != de->j)
		{
			s2 = ft_strndup(&s1[de->j], \
			de->d_quote_pos1 - de->j);
			s3 = analyze_expand_input(s2);
			free(s2);
			s4 = *expanded_input;
			*expanded_input = ft_strjoin(*expanded_input, s3);
			free(s3);
			free(s4);
			*is_dequoting = 1;
		}
	}
	else if (de->d_quote_pos1 != -1 && de->d_quote_pos2 == -1)
		de->d_quote_pos2 = de->i;
}

/*
	Split the part of a string that is in a double quote
*/
void	dquote_inside(char *s1, char **expanded_input, \
t_dequoting_expand *de, int *is_dequoting)
{
	char	*s2;
	char	*s3;
	char	*s4;

	if (de->d_quote_pos1 != -1 && de->d_quote_pos2 != -1)
	{
		s2 = ft_strndup(&s1[de->d_quote_pos1 + 1], \
		de->d_quote_pos2 - de->d_quote_pos1 - 1);
		s3 = analyze_expand_input(s2);
		free(s2);
		s4 = *expanded_input;
		*expanded_input = ft_strjoin(s4, s3);
		free(s3);
		free(s4);
		de->d_quote_pos1 = -1;
		de->d_quote_pos2 = -1;
		de->j = de->i +1;
		de->l_quote_type = 1;
		*is_dequoting = 1;
	}
}

/*
	Split the last part of a string that is after a double quote
*/
void	dquote_end(char *s1, char **expanded_input, t_dequoting_expand *de)
{
	char	*s2;
	char	*s3;
	char	*s4;

	s2 = ft_strndup(&s1[de->j], de->i - de->d_quote_pos2);
	s3 = analyze_expand_input(s2);
	free(s2);
	s4 = *expanded_input;
	*expanded_input = ft_strjoin(*expanded_input, s3);
	free(s3);
	free(s4);
}

/*
	Split the part of a string that is not which two quotes
*/
void	noquote_end(char *s1, char **expanded_input, t_dequoting_expand *de)
{
	char	*s2;
	char	*s3;
	char	*s4;

	s2 = ft_strndup(&s1[de->j], de->i);
	s3 = analyze_expand_input(s2);
	free(s2);
	s4 = *expanded_input;
	*expanded_input = ft_strjoin(*expanded_input, s3);
	free(s3);
	free(s4);
}
