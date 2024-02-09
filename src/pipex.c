/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kasingh <kasingh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/28 15:58:33 by kasingh           #+#    #+#             */
/*   Updated: 2024/02/09 12:20:36 by kasingh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

void	free_split(char **split)
{
	int	i;

	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

char	*get_path(char **cmd, char **env, char **path)
{
	int		i;
	char	*good_path;
	char	*tmp;
	char	*slash_cmd;

	if (access(cmd[0], F_OK) == 0)
		return (ft_strdup(cmd[0]));
	good_path = NULL;
	slash_cmd = ft_strjoin("/", cmd[0]);
	if (!slash_cmd)
		return (NULL);
	i = 0;
	while (path[i])
	{
		tmp = ft_strjoin(path[i], slash_cmd);
		if (!tmp)
			return (free(slash_cmd), NULL);
		if (access(tmp, F_OK) == 0)
			good_path = ft_strdup(tmp);
		free(tmp);
		i++;
	}
	free(slash_cmd);
	free_split(path);
	return (good_path);
}
void	error_msg(char *path, char **cmd)
{
	ft_putstr_fd("command not found: ", 2);
	ft_putendl_fd(cmd[0], 2);
	free_split(cmd);
	free(path);
	exit(127);
}

void	excute(char **cmd, char **env)
{
	int		i;
	char	*path;
	char	**tmp_path;

	i = 0;
	while (ft_strncmp(env[i], "PATH=", 5) != 0 && env[i])
		i++;
	if (env[i])
		tmp_path = ft_split(&env[i][5], ':');
	if (!tmp_path)
	{
		perror("Error getting path");
		exit(EXIT_FAILURE);
	}
	path = get_path(cmd, env, tmp_path);
	if (!path)
		error_msg(path, cmd);
	execve(path, cmd, env);
	error_msg(path, cmd);
}
char	**get_cmd(char *cmd)
{
	char	**cmd_split;

	cmd_split = ft_split(cmd, ' ');
	if (!cmd_split)
	{
		perror("Error getting command");
		exit(EXIT_FAILURE);
	}
	return (cmd_split);
}

void	child(int pipe_fd[2], char **av, char **env)
{
	int	fd_in;

	close(pipe_fd[0]);
	fd_in = open(av[1], O_RDONLY);
	if (fd_in == -1)
	{
		perror(av[1]);
		exit(1);
	}
	if (dup2(fd_in, STDIN_FILENO) == -1)
	{
		close(fd_in);
		exit(EXIT_FAILURE);
	}
	close(fd_in);
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		close(pipe_fd[1]);
		exit(EXIT_FAILURE);
	}
	close(pipe_fd[1]);
	excute(get_cmd(av[2]), env);
}

void	child2(int pipe_fd[2], char **av, char **env)
{
	int	fd_out;

	close(pipe_fd[1]);
	fd_out = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out == -1)
	{
		perror(av[4]);
		exit(EXIT_FAILURE);
	}
	if (dup2(fd_out, STDOUT_FILENO) == -1)
	{
		close(fd_out);
		exit(EXIT_FAILURE);
	}
	close(fd_out);
	if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
	{
		close(pipe_fd[0]);
		exit(EXIT_FAILURE);
	}
	close(pipe_fd[0]);
	excute(get_cmd(av[3]), env);
}

int	main(int ac, char **av, char **env)
{
	int		pipe_fd[2];
	pid_t	pid;
	pid_t	r_waitpid;
	int		status[2];

	if (ac != 5 || pipe(pipe_fd) == -1)
		return (1);
	pid = fork();
	if (pid == -1)
		return (close(pipe_fd[0]), close(pipe_fd[1]), -1);
	if (pid == 0)
		child(pipe_fd, av, env);
	pid = fork();
	if (pid == -1)
		return (close(pipe_fd[0]), close(pipe_fd[1]), -1);
	if (pid == 0)
		child2(pipe_fd, av, env);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	while ((r_waitpid = waitpid(-1, &status[0], 0)) > 0)
		if (r_waitpid == pid)
			status[1] = status[0];
	if (WIFEXITED(status[1]))
		return (WEXITSTATUS(status[1]));
	return (0);
}
