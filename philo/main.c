/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 01:36:25 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 05:08:43 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	start_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n)
	{
		if (pthread_create(&sim->philos[i].thread, NULL, philo_routine,
				&sim->philos[i]) != 0)
			return (1);
		i++;
	}
	if (pthread_create(&sim->monitor, NULL, monitor_routine, sim) != 0)
		return (1);
	return (0);
}

static void	join_all(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n)
	{
		pthread_join(sim->philos[i].thread, NULL);
		i++;
	}
	pthread_join(sim->monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (init_sim(&sim, argc, argv) != 0)
	{
		printf("usage: %s n time_to_die time_to_eat time_to_sleep [must_eat]\n",
			argv[0]);
		return (1);
	}
	if (start_threads(&sim) != 0)
		return (1);
	join_all(&sim);
	free_sim(&sim);
	return (0);
}
