/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   analyze_heredoc_iredirect_file2.c                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████          #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Handling signals in the here_doc
*/
void	signal_handler_heredoc(int signum)
{
	(void)signum;
	g_minishell->sigint_hd = 1;
	g_minishell->exit_status = 1;
	close(0);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	ft_printf("\n");
}

/*
	!Here_doc routine
	*1. Stop if the delimiter was found
	*2. Copy the text already present in the here_doc with the new text \
	entered in the here_doc
	*3. Being the environment variables that are in the here_doc
*/
int	heredoc_routine(char **limiter, char **s2, char **s3, char **s4)
{
	char	*s5;

	if (g_minishell->sigint_hd == 1 || !*s4 || \
	(ft_strncmp(*limiter, *s4, ft_strlen(*s4) - 1) == 0 && \
	ft_strncmp("\n", *s4, 1) != 0 && ft_strlen(*limiter) == ft_strlen(*s4) - 1) \
	|| (!*limiter[0] && *s4[0] == '\n'))
		return (0);
	*s3 = ft_strdup(*s2);
	if (!*s3)
		fatal_error(E_MALLOC);
	free(*s2);
	*s2 = ft_strjoin(*s3, *s4);
	if (!*s2)
		fatal_error(E_MALLOC);
	s5 = ft_strdup(*s2);
	free(*s2);
	*s2 = analyze_expand_input(s5);
	free(s5);
	free(*s4);
	free(*s3);
	return (1);
}
