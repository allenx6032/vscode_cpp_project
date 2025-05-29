/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   analyze_heredoc_iredirect_file1.c                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Redirects the file to the standard input of the program.
*/
static int	i_redirect(t_list *lst, int *t, t_node *node)
{
	if (t[2] == 1)
	{
		t[2] = 0;
		if (node->fd_in)
			close(node->fd_in);
		node->fd_in = open(((t_parsed_input *)lst->content)->input, \
			O_RDONLY);
		if (node->fd_in < 0)
		{
			perror(E_);
			node->error = 2;
		}
		return (1);
	}
	if (((t_parsed_input *)lst->content)->interpret == 1 && \
	ft_strcmp("<", ((t_parsed_input *)lst->content)->input) == 0)
	{
		t[2] = 1;
		return (1);
	}
	return (0);
}

/*
	Returns the text which is inside of the heredoc.
*/
static char	*get_here_doc_input(char *limiter)
{
	char	*s1;
	char	*s2;
	char	*s3;
	char	*s4;

	s2 = ft_calloc(1, sizeof(char *));
	signal(SIGINT, signal_handler_heredoc);
	signal(SIGQUIT, SIG_IGN);
	while (1)
	{
		s1 = readline("> ");
		s4 = ft_strjoin(s1, "\n");
		free(s1);
		if (!heredoc_routine(&limiter, &s2, &s3, &s4))
			break ;
	}
	if (g_minishell->sigint_hd == 1)
	{
		free(s1);
		free(s2);
	}
	free(s4);
	return (s2);
}

/*
	Opens the temporary heredoc file.
*/
static int	open_heredoc_tmpfile(char *s1, t_node *node)
{
	node->fd_in = open("/tmp/.pipex_here_doc", O_WRONLY \
	| O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
	if (node->fd_in < 1)
		return (0);
	ft_putstr_fd(s1, node->fd_in);
	free(s1);
	close(node->fd_in);
	node->fd_in = open("/tmp/.pipex_here_doc", O_RDONLY);
	if (node->fd_in < 1)
		return (0);
	if (unlink("/tmp/.pipex_here_doc") == -1)
		fatal_error(E_HEREDOC);
	return (1);
}

/*
	Redirects the text typed by the user in a temporary file which will
	be saved in the node then used as standard input by the program.
*/
static int	i_heredoc(t_list *lst, int *t, t_node *node)
{
	char	*s1;

	if (t[3] == 1)
	{
		t[3] = 0;
		s1 = get_here_doc_input(((t_parsed_input *)lst->content)->input);
		if (!s1)
			fatal_error(E_MALLOC);
		if (open_heredoc_tmpfile(s1, node) != 1 && g_minishell->sigint_hd != 1)
			fatal_error(E_HEREDOC);
		return (1);
	}
	if (((t_parsed_input *)lst->content)->interpret == 1 && \
	ft_strcmp("<<", ((t_parsed_input *)lst->content)->input) == 0)
	{
		t[3] = 1;
		return (1);
	}
	return (0);
}

/*
	Identifies if the string being analyzed by lexing is a heredoc sign (<<),
	a delimiter of heredoc, a redirection sign of a standard input file (<),
	or a input file that will be saved in the node.

	- Returns 0 when the analyzed string is not a heredoc sign (<<) or
	is not a redirect to a standard input file (<) or
	is not a heredoc delimiter or is not input file.
	
	- Returns 1 when the analyzed string is a heredoc sign (<<) or
	is a redirect to a standard input file (<) or is a heredoc 
	delimiter or a input file.

	*- reminder -
	t[2] to 1 identify that the previous word was an input (<) redirect.
	t[3] to 1 identify that the previous word was a here_doc (<<) redirect.
*/
int	analyze_heredoc_iredirect_file(t_list *lst, int *t)
{
	t_node	*node;

	node = ft_lstlast(g_minishell->nodes)->content;
	if (i_redirect(lst, t, node))
		return (1);
	if (i_heredoc(lst, t, node))
		return (1);
	return (0);
}
