/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kasingh <kasingh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 13:26:12 by kasingh           #+#    #+#             */
/*   Updated: 2024/02/16 17:47:59 by kasingh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

int	flag_out(int flag)
{
	if (flag == 1)
		return (O_WRONLY | O_CREAT | O_APPEND);
	else
		return (O_WRONLY | O_CREAT | O_TRUNC);
}

int	ac_loop(char **av, char **env, int ac, int pipe_fd[2])
{
	int		i;
	int		c_fd;
	pid_t	pid;

	i = 3;
	while (ac - 2 > i)
	{
		c_fd = pipe_fd[0];
		if (pipe(pipe_fd) == -1)
			return (close(c_fd), -1);
		pid = fork();
		if (pid == -1)
			return (close(pipe_fd[0]), close(c_fd), close(pipe_fd[1]), -1);
		if (pid == 0)
			child_loop(c_fd, pipe_fd, av[i], env);
		close(pipe_fd[1]);
		close(c_fd);
		i++;
	}
	return (0);
}

void	loop_here_doc(char *tmp, char **av, int fd)
{
	char	*line;

	while (1)
	{
		ft_putstr_fd("here_doc> ", 1);
		line = get_next_line(0);
		if (!line || ft_strncmp(line, tmp, ft_strlen(line)) == 0)
		{
			if (!line)
			{
				ft_putstr_fd("\nhere-document delimited ", 2);
				ft_putstr_fd("by end-of-file (wanted `", 2);
				ft_putstr_fd(av[2], 2);
				ft_putendl_fd("')", 2);
			}
			free(line);
			break ;
		}
		write(fd, line, ft_strlen(line));
		free(line);
	}
}

int	here_doc(char **av)
{
	int		fd;
	char	*tmp;

	fd = open(av[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror(av[1]);
		exit(1);
	}
	tmp = ft_strjoin(av[2], "\n");
	if (!tmp)
	{
		perror("here_doc:ft_strjoin:tmp");
		close(fd);
		exit(1);
	}
	loop_here_doc(tmp, av, fd);
	return (free(tmp), close(fd), 1);
}
