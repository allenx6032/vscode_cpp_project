/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/13 21:53:31 by ███████          #+#    #+#             */
/*   Updated: 2022/10/09 15:34:40 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*get_env_oldpwd(void)
{
	char	*old_path;
	char	*env_oldpwd;

	old_path = getcwd(NULL, 0);
	if (!old_path)
	{
		del_env("OLDPWD");
		return (NULL);
	}
	else
	{
		env_oldpwd = ft_strjoin("OLDPWD=", old_path);
		if (!env_oldpwd)
			fatal_error(E_MALLOC);
		free(old_path);
		return (env_oldpwd);
	}
}

static char	*get_path(t_node *node)
{
	char	*path;

	path = NULL;
	if (node->parsed_cmd)
		path = node->parsed_cmd[0];
	else
	{
		path = getenv("HOME");
		if (!path)
			msg_error(E_HOME);
	}
	return (path);
}

static void	update_env(char **env_oldpwd)
{
	char	*new_path;
	char	*env_pwd;

	if (*env_oldpwd)
	{
		push_env(*env_oldpwd);
		free(*env_oldpwd);
	}
	new_path = getcwd(NULL, 0);
	if (!new_path)
		del_env("PWD");
	else
	{
		env_pwd = ft_strjoin("PWD=", new_path);
		if (!env_pwd)
			fatal_error(E_MALLOC);
		push_env(env_pwd);
		free(env_pwd);
		free(new_path);
	}
	ft_arr2dfree((void **)g_minishell->env);
	linked_list_to_arr();
}

void	builtin_cd(t_node *node)
{
	char	*env_oldpwd;
	char	*path;

	env_oldpwd = get_env_oldpwd();
	path = get_path(node);
	if (chdir(path))
	{
		g_minishell->exit_status = 1;
		if (env_oldpwd)
			free(env_oldpwd);
		perror(E_);
	}
	else
		update_env(&env_oldpwd);
}
