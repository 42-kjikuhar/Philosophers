/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 20:42:12 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/11 21:03:28 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static bool	is_valid_int(const char *s)
{
	long	n;

	if (!s || !*s)
		return (false);
	if (*s == '+')
		s++;
	if (!*s)
		return (false);
	n = 0;
	while (*s)
	{
		if (*s < '0' || *s > '9')
			return (false);
		n = n * 10 + (*s - '0');
		if (n > INT_MAX)
			return (false);
		s++;
	}
	return (true);
}

int	parse_args(t_sim *sim, int argc, char **argv)
{
	if (argc != 5 && argc != 6)
		return (1);
	if (!is_valid_int(argv[1]) || !is_valid_int(argv[2])
		|| !is_valid_int(argv[3]) || !is_valid_int(argv[4]))
		return (1);
	if (argc == 6 && !is_valid_int(argv[5]))
		return (1);
	sim->n = ft_atoi(argv[1]);
	sim->time_to_die = ft_atoi(argv[2]);
	sim->time_to_eat = ft_atoi(argv[3]);
	sim->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		sim->max_meals = ft_atoi(argv[5]);
	else
		sim->max_meals = -1;
	if (sim->n <= 0 || sim->time_to_die <= 0 || sim->time_to_eat <= 0
		|| sim->time_to_sleep <= 0)
		return (1);
	return (0);
}
