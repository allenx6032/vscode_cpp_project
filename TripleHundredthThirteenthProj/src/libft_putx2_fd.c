/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   putx2_fd.c                   	                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/02 16:09:35 by ███████           #+#    #+#             */
/*   Updated: 2021/11/16 13:43:12 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	putx2_fd(int n, int fd)
{
	char		*hex;
	int			i;

	hex = i_to_hex(n);
	if (!hex)
		return (0);
	i = 0;
	while (hex[i])
	{
		hex[i] = ft_toupper(hex[i]);
		i++;
	}
	i = puts_fd(hex, fd);
	free(hex);
	return (i);
}
