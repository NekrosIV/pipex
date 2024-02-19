/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kasingh <kasingh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 14:56:27 by kasingh           #+#    #+#             */
/*   Updated: 2024/02/19 14:12:42 by kasingh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

static void	child_in(int pipe_fd[2], char **av, char **env, int flag)
{
	int	fd_in;

	close(pipe_fd[0]);
	fd_in = open(av[1], O_RDONLY);
	if (fd_in == -1)
	{
		perror(av[1]);
		close(pipe_fd[1]);
		exit(1);
	}
	if (dup2(fd_in, STDIN_FILENO) == -1)
	{
		perror("child_loop:dup2(fd_in)");
		close(fd_in);
		exit(EXIT_FAILURE);
	}
	close(fd_in);
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		perror("child_loop:dup2(pipe_fd[1])");
		close(pipe_fd[1]);
		exit(EXIT_FAILURE);
	}
	close(pipe_fd[1]);
	excute(get_cmd(av[2 + flag]), env);
}

void	child_loop(int c_fd, int pipe_fd[2], char *av, char **env)
{
	close(pipe_fd[0]);
	if (dup2(c_fd, STDIN_FILENO) == -1)
	{
		perror("child_loop:dup2(c_fd)");
		close(c_fd);
		exit(EXIT_FAILURE);
	}
	close(c_fd);
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		perror("child_loop:dup2(pipe_fd[1])");
		close(pipe_fd[1]);
		exit(EXIT_FAILURE);
	}
	close(pipe_fd[1]);
	excute(get_cmd(av), env);
}

static void	child_out(int pipe_fd[2], char **av, char **env, int flag)
{
	int	fd_out;

	close(pipe_fd[1]);
	fd_out = open(av[1], flag_out(flag), 0644);
	if (fd_out == -1)
	{
		perror(av[1]);
		close(pipe_fd[0]);
		exit(EXIT_FAILURE);
	}
	if (dup2(fd_out, STDOUT_FILENO) == -1)
	{
		perror("child_out:dup2(fd_out)");
		close(fd_out);
		exit(EXIT_FAILURE);
	}
	close(fd_out);
	if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
	{
		perror("child_out:dup2(pipe_fd[0])");
		close(pipe_fd[0]);
		exit(EXIT_FAILURE);
	}
	close(pipe_fd[0]);
	excute(get_cmd(av[0]), env);
}

static int	wait_for_child(pid_t pid, int flag, char **av)
{
	int		status[2];
	pid_t	r_waitpid;

	while (1)
	{
		r_waitpid = waitpid(-1, &status[0], 0);
		if (r_waitpid == -1)
			break ;
		if (r_waitpid == pid)
			status[1] = status[0];
	}
	if (flag == 1)
		unlink(av[1]);
	if (WIFEXITED(status[1]))
		return (WEXITSTATUS(status[1]));
	else if (WIFSIGNALED(status[1]))
		return (WTERMSIG(status[1]) + 128);
	return (0);
}

int	main(int ac, char **av, char **env)
{
	int		pipe_fd[2];
	pid_t	pid;
	int		flag;

	flag = 0;
	if ((ac < 5 || (ft_strncmp(av[1], "here_doc", 8) == 0 && ac < 6))
		|| pipe(pipe_fd) == -1)
		return (1);
	if (ft_strncmp(av[1], "here_doc", 8) == 0)
		flag = here_doc(av);
	pid = fork();
	if (pid == -1)
		return (close(pipe_fd[0]), close(pipe_fd[1]), -1);
	if (pid == 0)
		child_in(pipe_fd, av, env, flag);
	close(pipe_fd[1]);
	if (ac > 6 && ac_loop(av, env, ac, pipe_fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (close(pipe_fd[0]), -1);
	if (pid == 0)
		child_out(pipe_fd, &(av[ac - 2]), env, flag);
	return (close(pipe_fd[0]), wait_for_child(pid, flag, av));
}
