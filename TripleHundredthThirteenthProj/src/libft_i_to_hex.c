/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   i_to_hex.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/11 00:30:00 by ███████           #+#    #+#             */
/*   Updated: 2022/05/10 11:07:38 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	i_len(uint64_t ull)
{
	int	i;

	i = 0;
	if (!ull)
		i = 1;
	while (ull != 0)
	{
		ull = ull / 16;
		i++;
	}
	if (i > 8)
		return (8);
	return (i);
}

char	*i_to_hex(uint64_t ull)
{
	char		*s;
	char		*hex;
	int			i;

	hex = "0123456789abcdef";
	i = i_len(ull);
	s = ft_calloc(i + 1, sizeof(char));
	if (!s)
		return (NULL);
	while (i-- > 0)
	{
		s[i] = hex[ull % 16];
		ull = ull / 16;
	}
	return (s);
}
