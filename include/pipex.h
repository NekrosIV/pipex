/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kasingh <kasingh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/29 13:17:24 by kasingh           #+#    #+#             */
/*   Updated: 2024/02/16 17:48:09 by kasingh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "../libft/libft.h"
# include <fcntl.h>
# include <limits.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <wait.h>

void	free_split(char **split);
void	error_msg(char *path, char **cmd);
char	**get_cmd(char *cmd);
char	*get_path(char **cmd, char **path);
void	excute(char **cmd, char **env);

int		flag_out(int flag);
int		ac_loop(char **av, char **env, int ac, int pipe_fd[2]);
void	loop_here_doc(char *tmp, char **av, int fd);
int		here_doc(char **av);
void	child_loop(int c_fd, int pipe_fd[2], char *av, char **env);

#endif