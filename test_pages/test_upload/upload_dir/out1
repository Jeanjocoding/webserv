/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbellavi <bbellavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/29 02:00:27 by bbellavi          #+#    #+#             */
/*   Updated: 2020/12/10 11:59:10 by bbellavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include "lexer.h"
#include "reader.h"
#include "parser.h"
#include "ft_stdlib.h"
#include "ft_stdio.h"
#include "eval.h"
#include "debug.h"
#include "environ.h"

char		**g_env = NULL;
int			g_exec_pid = -1;
int			g_exitstatus = 0;
t_queue		*g_queue = NULL;
int			g_pid_to_kill = 0;
int			g_in_eval = 0;
int			g_quote_parity_error = 0;
int			**g_pipe_array;

int		run(const char *line)
{
	t_lexer	lexer;
	int		out;

	if ((out = parser(line)) > 0)
		return (out);
	lexer = (t_lexer){IS_COMMAND, 0, NULL, (char*)line};
	while (TRUE)
	{
		get_next_tokens(&lexer);
		if (lexer.head == NULL)
			return (SUCCESS);
		lexer.head = queue_join(lexer.head);
		if (lexer.head == NULL)
			return (FAILURE);
		g_in_eval = 1;
		if (eval(lexer.head) == -1)
			return (-1);
		g_in_eval = 0;
		queue_delete(&lexer.head);
	}
	return (0);
}

int		run_shell(void)
{
	char	*line;

	g_flawed = 0;
	while (TRUE)
	{
		g_quote_parity_error = 0;
		if (reader(&line) == -1)
			return (-1);
		run(line);
		free(line);
	}
}

void	wait_and_exit(void)
{
	int status;

	wait(&status);
	if (WIFEXITED(status))
		exit(WEXITSTATUS(status));
}

int		main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	(void)argv;
	if (!(ft_tab_copy(&g_env, envp)))
		return (-1);
	if (*envp == NULL)
		export_basic_environ();
	else
		inc_shlvl();
	signal(SIGQUIT, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, SIG_IGN);
	g_pid_to_kill = fork();
	if (g_pid_to_kill == 0)
	{
		signal(SIGTERM, sigterm_handler);
		if (run_shell() == -1)
		{
			ft_freetab(&g_env);
			exit(EXIT_FAILURE);
		}
	}
	else
		wait_and_exit();
	return (0);
}
