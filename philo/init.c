/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:03:30 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 04:32:19 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	init_philo(t_sim *sim, int i)
{
	sim->fork_used[i] = 0;
	sim->philos[i].id = i;
	sim->philos[i].left_fork = i;
	sim->philos[i].right_fork = (i + 1) % sim->n;
	sim->philos[i].state = THINKING;
	sim->philos[i].last_meal_time = 0;
	sim->philos[i].state_until = 0;
	sim->philos[i].meals_eaten = 0;
}

static int	alloc_sim(t_sim *sim)
{
	sim->philos = malloc(sizeof(t_philo) * sim->n);
	sim->fork_used = malloc(sizeof(int) * sim->n);
	if (!sim->philos || !sim->fork_used)
	{
		free(sim->philos);
		free(sim->fork_used);
		return (1);
	}
	return (0);
}

int	init_sim(t_sim *sim, int argc, char **argv)
{
	int	i;

	if (argc != 5 && argc != 6)
		return (1);
	sim->n = ft_atoi(argv[1]);
	sim->time_to_die = ft_atoi(argv[2]);
	sim->time_to_eat = ft_atoi(argv[3]);
	sim->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		sim->max_meals = ft_atoi(argv[5]);
	else
		sim->max_meals = -1;
	sim->start_time = current_time_ms();
	sim->finished = 0;
	if (alloc_sim(sim) != 0)
		return (1);
	i = 0;
	while (i < sim->n)
	{
		init_philo(sim, i);
		i++;
	}
	return (0);
}
