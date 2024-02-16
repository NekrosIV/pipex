/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kasingh <kasingh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 14:56:27 by kasingh           #+#    #+#             */
/*   Updated: 2024/02/16 13:03:34 by kasingh          ###   ########.fr       */
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
	else if (access(path, F_OK) == 0 && access(path, X_OK) != 0)
	{
		ft_putstr_fd("Permission denied: ", 2);
		ft_putendl_fd(cmd[0], 2);
		free_split(cmd);
		free(path);
		exit(126);
	}
	else
		ft_putstr_fd("command not found: ", 2);
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

void	child_in(int pipe_fd[2], char **av, char **env, int flag)
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
int	flag_out(int flag)
{
	if (flag == 1)
		return (O_WRONLY | O_CREAT | O_APPEND);
	else
		return (O_WRONLY | O_CREAT | O_TRUNC);
}

void	child_out(int pipe_fd[2], char **av, char **env, int flag)
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

int	wait_for_child(pid_t pid, int flag, char **av)
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
				ft_putstr_fd("\nhere-document delimited by end-of-file (wanted `",
					2);
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
	char	*line;
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
	if (ac > 5 && ac_loop(av, env, ac, pipe_fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (close(pipe_fd[0]), -1);
	if (pid == 0)
		child_out(pipe_fd, &(av[ac - 2]), env, flag);
	return (close(pipe_fd[0]), wait_for_child(pid, flag, av));
}
