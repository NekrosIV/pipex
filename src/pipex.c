/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kasingh <kasingh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/28 15:58:33 by kasingh           #+#    #+#             */
/*   Updated: 2024/02/05 15:55:41 by kasingh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

// int	main(int ac, char **av, char **env)
// {
// 	int		pipe_fd[2];
// 	pid_t	pid;

// 	if (ac != 3 || pipe(pipe_fd) == -1)
// 		return (-1);
// 	if (open(av[1], O_RDONLY) < 0)
// 		return (ft_printf("error\n"), -1);
// 	pid = fork();
// 	if (pid == -1)
// 		return (-1);
// 	if (pid != 0)
// 	{
// 		close(pipe_fd[0]);
// 		dup2(pipe_fd[1], 1);
// 		close(pipe_fd[1]);
// 		execve(av[1], &av[1], env);
// 	}
// 	else
// 	{
// 		close(pipe_fd[1]);
// 		dup2(pipe_fd[0], 0);
// 		close(pipe_fd[0]);
// 		execve(av[2], &av[2], env);
// 	}

// }

char	*get_path(char **cmd, char **env, char **path)
{
	int		i;
	char	*cpy;
	char	*good_path;
	char	*tmp;

	good_path = NULL;
	i = 0;
	while (path[i])
	{
		cpy = ft_strjoin(path[i], "/");
		tmp = ft_strjoin(cpy, cmd[0]);
		free(cpy);
		if (access(tmp, F_OK) == 0)
		{
			good_path = ft_strdup(tmp);
			free(tmp);
			break ;
		}
		free(tmp);
		free(path[i]);
		i++;
	}
	free(path);
	return (good_path);
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
		perror("Error getting path\n");
		exit(EXIT_FAILURE);
	}
	path = get_path(cmd, env, tmp_path);
	if (execve(path, cmd, env) == -1)
	{
		perror("Error executing command\n");
		free(path);
		// free_split(cmd); je dois creer cette fonction
		exit(EXIT_FAILURE);
	}
}

void	child(int *pipe_fd, char **av, char **env)
{
	int		fd_in;
	char	**cmd;

	fd_in = open(av[1], O_RDONLY);
	if (fd_in == -1)
	{
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	if (dup2(fd_in, STDIN_FILENO) == -1 || dup2(pipe_fd[1], STDOUT_FILENO) ==
		-1)
		exit(EXIT_FAILURE);
	close(fd_in);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	cmd = ft_split(av[2], ' ');
	if (!cmd)
		exit(EXIT_FAILURE);
	excute(cmd, env);
}

void	parent(int *pipe_fd, char **av, char **env)
{
	int		fd_out;
	char	**cmd;

	fd_out = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out == -1)
	{
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	if (dup2(fd_out, STDOUT_FILENO) == -1 || dup2(pipe_fd[0], STDIN_FILENO) ==
		-1)
		exit(EXIT_FAILURE);
	close(fd_out);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	cmd = ft_split(av[3], ' ');
	if (!cmd)
		exit(EXIT_FAILURE);
	excute(cmd, env);
}

int	main(int ac, char **av, char **env)
{
	int pipe_fd[2];
	pid_t pid;

	if (ac != 5 || pipe(pipe_fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (-1);
	}
	if (pid == 0)
		child(pipe_fd, av, env);
	else
	{
		wait(NULL);
		parent(pipe_fd, av, env);
	}
	return (0);
}