/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kasingh <kasingh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/28 15:58:33 by kasingh           #+#    #+#             */
/*   Updated: 2024/02/16 12:11:45 by kasingh          ###   ########.fr       */
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
	if (!path && ft_strchr(cmd[0], '/') != NULL)
		ft_putstr_fd("No such file or directory : ", 2);
	else if (!path)
		ft_putstr_fd("command not found: ", 2);
	else if (access(path, F_OK) == 0 && access(path, X_OK) != 0)
	{
		ft_putstr_fd("Permission denied: ", 2);
		ft_putendl_fd(cmd[0], 2);
		free_split(cmd);
		free(path);
		exit(126);
	}
	if (path)
		free(path);
	ft_putendl_fd(cmd[0], 2);
	free_split(cmd);
	exit(127);
}

void	excute(char **cmd, char **env)
{
	int		i;
	char	*path;
	char	**tmp_path;

	tmp_path = NULL;
	path = NULL;
	i = 0;
	if (access(cmd[0], F_OK) == 0)
		path = ft_strdup(cmd[0]);
	else if (env[i])
	{
		while (env[i] && ft_strncmp(env[i], "PATH=", 5) != 0)
			i++;
		if (env[i])
			tmp_path = ft_split(&env[i][5], ':');
		if (!tmp_path)
			error_msg(path, cmd);
		path = get_path(cmd, env, tmp_path);
	}
	if (!path)
		error_msg(path, cmd);
	execve(path, cmd, env);
	error_msg(path, cmd);
}

char	**get_cmd(char *cmd)
{
	char	**cmd_split;

	cmd_split = ft_split(cmd, ' ');
	if (!cmd_split || !cmd_split[0])
	{
		ft_putstr_fd("Command not found: ", 2);
		ft_putendl_fd(cmd, 2);
		free(cmd_split);
		exit(127);
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
		close(pipe_fd[1]);
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
		close(pipe_fd[0]);
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

int	wait_for_child(pid_t pid)
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
	return (wait_for_child(pid));
}
