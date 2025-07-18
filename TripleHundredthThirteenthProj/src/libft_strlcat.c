/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/02 16:09:35 by ███████           #+#    #+#             */
/*   Updated: 2022/05/10 11:08:27 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *restrict dst, const char *restrict src, size_t dsts)
{
	unsigned int	src_len;
	unsigned int	dst_len;
	unsigned int	i;
	unsigned int	dist;

	src_len = ft_strlen(src);
	dst_len = ft_strlen(dst);
	i = 0;
	while (i < dsts && dst[i])
		i++;
	dist = i;
	if (dsts < dst_len || dsts == 0)
		return (src_len + dsts);
	if (dst_len < dsts)
	{
		i = 0;
		while ((i < dsts - dst_len - 1) && src[i])
		{
			dst[dst_len + i] = src[i];
			i++;
		}
	}
	if (i > 0)
		dst[dst_len + i] = '\0';
	return (src_len + dist);
}
