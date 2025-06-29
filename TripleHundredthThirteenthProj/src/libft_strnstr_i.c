/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr_i.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/02 16:09:35 by ███████           #+#    #+#             */
/*   Updated: 2022/09/09 06:34:11 by ███████          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strnstr_i(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	int		j;

	if (ft_strlen(needle) < 1)
		return (0);
	i = 0;
	while (haystack[i] && i < len)
	{
		j = 0;
		while (needle[j] == haystack[i + j] && i + j < len)
		{
			if (haystack[i + j] == needle[j])
			{
				if (needle[j + 1] == '\0')
					return (i);
			}
			else
				break ;
			j++;
		}
		i++;
	}
	return (0);
}
