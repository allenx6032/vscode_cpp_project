/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/03 02:11:00 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	The goal of lexing is to identify a sequence of words and to extract \
	a meaning.
	Then, it identifies if there are redirections or a here_doc and save the file \
	descriptors of redirections.
	!Finally a new node is created at each pipe.

	*- node control -
	t[0] 0 identify a new node
	t[0] 1 identify an existing node

	*- cmd or builtin control -
	t[1] to 0 identify that the word is a command
	t[1] to 1 identify that the word is an argument

	*- redirect control -
	t[2] to 1 identify that the previous word was an input (<) redirect.
	t[3] to 1 identify that the previous word was a here_doc (<<) redirect.
	t[4] to 1 identify that the previous word was an output (>) redirect.
	t[5] to 1 identify that the previous word was an output (>>) redirect.
*/
void	lexing(void)
{
	t_list	*parsed_lst;
	t_list	*current_parsed_lst;
	int		t[6];

	ft_memset(t, 0, sizeof(t));
	parsed_lst = g_minishell->lst_parsed_input;
	while (parsed_lst)
	{
		current_parsed_lst = parsed_lst;
		parsed_lst = parsed_lst->next;
		node_control(t);
		if (analyze_pipe(current_parsed_lst, t))
			continue ;
		if (analyze_heredoc_iredirect_file(current_parsed_lst, t))
			continue ;
		if (t[1] == 0)
		{
			t[1] = 1;
			if (!analyze_cmd(current_parsed_lst))
				continue ;
		}
		else if (t[1] == 1)
			if (analyze_arg_oredirect_file(current_parsed_lst, t))
				continue ;
	}
}
