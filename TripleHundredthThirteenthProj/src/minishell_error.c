/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/09 14:40:53 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Displays an error message on the standard error output.
*/
void	msg_error(char *str)
{
	ft_dprintf(2, "error : %s\n", str);
}

/*
	Cleans and exits the program in case of error.
*/
void	fatal_error(char *str)
{
	msg_error(str);
	clear_lst_env(1);
	if (g_minishell->exit_status > 0)
		exit(g_minishell->exit_status % 256);
	else
		exit(EXIT_FAILURE);
}
