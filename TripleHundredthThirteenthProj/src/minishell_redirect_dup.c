/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_dup.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/07 02:47:30 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Use the standard input of the input file to send it to the first command and \
	send the result in the first pipe.
*/
static void	first_dup(t_list *lst)
{
	if (dup2(((t_node *)lst->content)->pipe[1], STDOUT_FILENO) == -1)
		fatal_error(E_DUP);
	if (close(((t_node *)lst->content)->pipe[0]) == -1)
		fatal_error(E_CLOSE);
}

/*
	Use the output of the previous pipe as standard input for the command and \ 
	send the result into in output file.
*/
static void	last_dup(t_list *lst)
{
	if (dup2(((t_node *)lst->previous->content)->pipe[0], \
	STDIN_FILENO) == -1)
		fatal_error(E_DUP);
	if (close(((t_node *)lst->content)->pipe[0]) == -1)
		fatal_error(E_CLOSE);
}

/*
	Use the output of the previous pipe as standard input for the command and \ 
	send the result into a new pipe.
*/
static void	middle_dup(t_list *lst)
{
	if (dup2(((t_node *)lst->previous->content)->pipe[0], \
	STDIN_FILENO) == -1)
		fatal_error(E_DUP);
	if (dup2(((t_node *)lst->content)->pipe[1], STDOUT_FILENO) == -1)
		fatal_error(E_DUP);
	if (close(((t_node *)lst->content)->pipe[0]) == -1)
		fatal_error(E_CLOSE);
}

/*
	Handles input (<), output (>), output in append mode (>>) redirects and \
	redirects outputs from pipes to pipes.
*/
void	redirect_dup(t_list *lst)
{
	if (lst->previous == NULL && lst->next != NULL)
		first_dup(lst);
	else if (lst->next == NULL && lst->previous != NULL)
		last_dup(lst);
	else if (lst->next != NULL && lst->previous != NULL)
		middle_dup(lst);
	if (((t_node *)lst->content)->fd_in > 0)
		if (dup2(((t_node *)lst->content)->fd_in, STDIN_FILENO) == -1)
			fatal_error(E_DUP);
	if (((t_node *)lst->content)->fd_out > 0)
		if (dup2(((t_node *)lst->content)->fd_out, STDOUT_FILENO) == -1)
			fatal_error(E_DUP);
}
