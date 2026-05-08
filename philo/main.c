/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:03:30 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 04:32:19 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	free_sim(t_sim *sim)
{
	free(sim->philos);
	free(sim->fork_used);
}

static int	all_satisfied(t_sim *sim)
{
	int	i;

	if (sim->max_meals < 0)
		return (0);
	i = 0;
	while (i < sim->n)
	{
		if (sim->philos[i].meals_eaten < sim->max_meals)
			return (0);
		i++;
	}
	return (1);
}

void	run_loop(t_sim *sim)
{
	long	now;
	int		i;

	while (!sim->finished)
	{
		now = current_time_ms() - sim->start_time;
		i = 0;
		while (i < sim->n)
		{
			update_philo(sim, &sim->philos[i], now);
			i++;
		}
		check_death(sim, now);
		if (all_satisfied(sim))
			sim->finished = 1;
		usleep(1000);
	}
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
	run_loop(&sim);
	free_sim(&sim);
	return (0);
}
