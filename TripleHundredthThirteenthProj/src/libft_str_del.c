/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_str_del.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/09 13:40:56 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_str_del(char *str, char *old_word)
{
	int		i;
	char	*s1;
	char	*s2;

	i = ft_strnstr_i(str, old_word, ft_strlen(str));
	s1 = ft_strndup(str, i);
	if (!s1)
		return (NULL);
	s2 = ft_strjoin(s1, " ");
	if (!s2)
		return (NULL);
	free(s1);
	i = i + ft_strlen(old_word);
	s1 = ft_strjoin(s2, &str[i]);
	if (!s1)
		return (NULL);
	free(s2);
	return (s1);
}
