/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kasingh <kasingh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 11:15:26 by kasingh           #+#    #+#             */
/*   Updated: 2023/11/17 10:43:09 by kasingh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "unistd.h"

/*ft_putstr_fd: Affiche une chaîne sur un descripteur de fichier*/
void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	i = 0;
	while (s[i])
	{
		write(fd, &s[i], 1);
		i++;
	}
}
