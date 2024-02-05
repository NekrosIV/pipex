/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kasingh <kasingh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/28 15:58:33 by kasingh           #+#    #+#             */
/*   Updated: 2024/02/05 15:06:14 by kasingh          ###   ########.fr       */
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


int	main(int ac, char **av, char **env)
{
	int		pipe_fd[2];
	pid_t	pid;
	int     fd_in, fd_out;

	if (ac != 5 || pipe(pipe_fd) == -1)
		return (-1);
	fd_in = open(av[1], O_RDONLY);
	fd_out = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_in == -1 || fd_out == -1)
	{
		perror("Error opening file");
		return (-1);
	}
	
	pid = fork();
	if (pid == -1)
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (-1);
	}
	if (pid == 0)
	{
		ft_printf("in pid child\n");
		char *args[] = {"cat", "-e", NULL};
		if (dup2(fd_in, STDIN_FILENO) == -1 || dup2(pipe_fd[1], STDOUT_FILENO) == -1)
			exit(EXIT_FAILURE);
		close(fd_in);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		if (execve("/bin/cat", args, env) == -1)
			exit(EXIT_FAILURE);
	}
	else
	{
		wait(NULL);
		char *args[] = {"wc", "-l", NULL};
		ft_printf("in pid parent\n");
		if (dup2(fd_out, STDOUT_FILENO) == -1 || dup2(pipe_fd[0], STDIN_FILENO) == -1)
			exit(EXIT_FAILURE);
		close(fd_out);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		if (execve("/usr/bin/wc", args, env) == -1)
			exit(EXIT_FAILURE);
	}	
	// while (ft_strncmp(env[i], "PATH=", 5) != 0 && env[i])
	// 	i++;
	// if (env[i + 1])
	// 	path = ft_split(&env[i][5], ':');
	return (0);
}