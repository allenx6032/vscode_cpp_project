/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utest.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/14 01:04:58 by ███████           #+#    #+#             */
/*   Updated: 2022/05/10 11:07:33 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libtest.h"

void	utest(int is_error, char *s, t_test *test)
{
	test->nb_tests++;
	if (is_error == -1)
	{
		ft_printf("%s %s[FAIL]%s \n", s, TRED, TNRM);
		test->nb_errors++;
	}
	else
	{
		ft_printf("%s %s[SUCCESS]%s \n", s, TGRN, TNRM);
		test->nb_success++;
	}
}
