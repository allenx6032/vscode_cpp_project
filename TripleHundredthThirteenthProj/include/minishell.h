/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ███████ <███████@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 11:45:53 by ███████           #+#    #+#             */
/*   Updated: 2022/10/17 01:41:37 by ███████          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <readline.h>
# include <readline_history.h>
# include <signal.h>
# include <termios.h>
# include <fcntl.h>
# include <errno.h>

# define E_ "minishell error"
# define E_CMD_NFND "Command not found"
# define E_PERM_DENIED "Permission denied"
# define E_FORK "Resource temporarily unavailable"
# define E_MALLOC "Malloc malfunction"
# define E_HEREDOC "Heredoc malfunction"
# define E_PIPE "Pipe malfunction"
# define E_CLOSE "Close malfunction"
# define E_DUP "Dup malfunction"
# define E_GC_ACCESS "No such file or directory"
# define E_GC_ACCESS_PARENT "Cannot access parent directories: No such file or \
directory"
# define E_QUOTE "Unclosed quotes or double quotes"
# define E_SYNTAX "Syntax error"
# define E_HOME "HOME not set"
# define E_NUMERIC "Numeric argument required"
# define E_TOMANY_ARG "Too many arguments"
# define E_BADIDENTF "Bad identifier"

// structure of the minishell
typedef struct s_minishell {
	int		dbg;
	int		exit;
	int		exit_status;
	char	**env;
	t_list	*lst_env;
	char	*raw_input;
	int		d_fd_in;
	int		d_fd_out;
	t_list	*lst_parsed_input;
	t_list	*nodes;
	int		sigint_hd;
}	t_minishell;

// linked list of node
typedef struct s_node {
	int		id;
	char	**parsed_cmd;
	char	*path;
	int		pid;
	int		fd_in;
	int		fd_out;
	int		pipe[2];
	int		error;
	int		builtin_id;
}	t_node;

// linked list of env
typedef struct s_node_env
{
	char	*key;
	char	*value;
	int		displayable;
}	t_node_env;

// dequoting expand
typedef struct s_dequoting_expand {
	int	i;
	int	j;
	int	d_quote_pos1;
	int	d_quote_pos2;
	int	s_quote_pos1;
	int	s_quote_pos2;
	int	l_quote_type;
}	t_dequoting_expand;

// parsed input struct
typedef struct s_parsed_input
{
	char	*input;
	int		interpret;
}	t_parsed_input;

// global variable
t_minishell	*g_minishell;

// avoid compilation crash
void	rl_replace_line(const char *text, int clear_undo);
void	rl_clear_history(void);

// core
void	init_s_minishell(char **argv, char **env);
void	asking_input(void);
int		prompting(void);
int		parsing(void);
void	lexing(void);

// exec and pipe
void	execution(void);
void	redirect_dup(t_list *lst);

// error
void	fatal_error(char *str);
void	msg_error(char *str);

// clear
void	clear(void);
void	loop_clear(void);
void	clear_arr_env(void);
void	clear_lst_env(int type);

// debug
void	dbg_showlexing(void);
void	dbg_show_lstenv(void);
void	dbg_showparsing(void);

// env
void	rebuild_necessary_env(void);

// checks
int		check_valid_quote(void);
int		check_valid_metacharacters(void);
int		analyze_output_exist(int i, char *input);
int		analyze_after_metachar(int i, char *input);

// deglued
void	deglueing(void);

// parsing
char	*analyze_expand_input(char *s2);
t_list	*input_parsing(char const *s);
char	*expand_dequoting_string(char *s1, int *is_dequoting);

// quote
int		escape_quote(int *i, int *in_quote, int quote_type);

// dequoting
void	init_dequoting_expand(t_dequoting_expand *de);
void	dquote_start(char *s1, char **expanded_input, \
t_dequoting_expand *de, int *is_dequoting);
void	dquote_inside(char *s1, char **expanded_input, \
t_dequoting_expand *de, int *is_dequoting);
void	dquote_end(char *s1, char **expanded_input, \
t_dequoting_expand *de);
void	squote_start(char *s1, char **expanded_input, \
t_dequoting_expand *de, int *is_dequoting);
void	squote_inside(char *s1, char **expanded_input, \
t_dequoting_expand *de, int *is_dequoting);
void	squote_end(char *s1, char **expanded_input, \
t_dequoting_expand *de);
void	noquote_end(char *s1, char **expanded_input, \
t_dequoting_expand *de);

// lexing
void	node_control(int *t);
int		analyze_cmd(t_list *lst);
int		analyze_arg_oredirect_file(t_list *lst, int *t);
int		analyze_heredoc_iredirect_file(t_list *lst, int *t);
int		analyze_pipe(t_list *lst, int *t);
int		heredoc_routine(char **limiter, char **s2, char **s3, char **s4);

// convert to path
char	*convert_cmd_to_path(char *cmd);

// builtin
void	exec_builtin(t_node	*node, int last_node_id);
int		get_builtin_id(char *str);

// echo builtin
void	builtin_echo(t_node *node);

// pwd builtin
void	builtin_pwd(t_node *node);

// cd builtin
void	builtin_cd(t_node *node);

// exit builtin
void	builtin_exit(t_node *node);

// env builtin
void	linked_list_to_arr(void);
void	arr_to_linked_list(void);
void	builtin_env(t_node *node);

// export builtin
void	builtin_export(t_node *node);
int		push_env(char *parsed_cmd);
void	print_env_alphabetically(void);
int		is_valid_env_key(char *key);

// unset builtin
void	builtin_unset(t_node *node);
int		del_env(char *parsed_cmd);

// signal
void	signal_recept_not_blocking_cmd(void);
void	signal_recept_blocking_cmd(void);
void	signal_handler_nonblocking_cmd(int signum);
void	signal_handler_blocking_cmd(int signum);
void	signal_handler_heredoc(int signum);

#endif