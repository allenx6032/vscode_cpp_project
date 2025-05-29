/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 12:27:36 by ███████           #+#    #+#             */
/*   Updated: 2022/10/11 14:27:55 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Returns the PATH variable saved in the environment.
*/
static char	*get_env_paths(void)
{
	int		arr_len;
	int		line_len;
	int		i;
	char	*path;

	arr_len = ft_arr2dlen((void **)g_minishell->env);
	path = NULL;
	i = 0;
	while (i < arr_len)
	{
		line_len = ft_strlen(g_minishell->env[i]);
		path = ft_strnstr(g_minishell->env[i], "PATH=", line_len);
		if (path != NULL)
			return (&path[5]);
		i++;
	}
	return (NULL);
}

/*
	Returns when possible the executable path of a command.
*/
static char	*search_cmd_path(char *cmd, char **paths)
{
	char	*cmd_path1;
	char	*cmd_path2;
	int		arr_len;
	int		i;

	arr_len = ft_arr2dlen((void **)paths);
	i = 0;
	while (i++ < arr_len)
	{
		cmd_path1 = ft_strjoin(paths[i - 1], "/");
		if (!cmd_path1)
			fatal_error(E_MALLOC);
		cmd_path2 = ft_strjoin(cmd_path1, cmd);
		if (!cmd_path2)
			fatal_error(E_MALLOC);
		free(cmd_path1);
		if (access(cmd_path2, X_OK) == 0)
		{
			ft_arr2dfree((void **)paths);
			return (cmd_path2);
		}
		free(cmd_path2);
	}
	ft_arr2dfree((void **)paths);
	return (NULL);
}

/*
	Converts command name to a executable path.
*/
char	*convert_cmd_to_path(char *cmd)
{
	char	*env_path;
	char	**paths;

	env_path = get_env_paths();
	if (env_path)
	{
		paths = ft_split(env_path, ':');
		if (!paths)
			return (NULL);
	}
	else
		return (NULL);
	return (search_cmd_path(cmd, paths));
}
