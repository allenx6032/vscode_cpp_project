/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   analyze_pipe.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/03 02:31:29 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	*- reminder -

	*- node control -
	t[0] 0 identify a new node
	t[0] 1 identify an existing node

*/
int	analyze_pipe(t_list *lst, int *t)
{
	if (((t_parsed_input *)lst->content)->interpret == 1 \
	&& ft_strcmp("|", ((t_parsed_input *)lst->content)->input) == 0)
	{
		ft_memset(t, 0, sizeof(t));
		return (1);
	}
	return (0);
}
