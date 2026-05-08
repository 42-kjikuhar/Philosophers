/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:03:30 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 04:15:07 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	free_sim(t_sim *sim)
{
	free(sim->philos);
	free(sim->fork_used);
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
		if (now >= sim->simulation_time)
			sim->finished = 1;
		usleep(1000);
	}
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (init_sim(&sim, argc, argv) != 0)
	{
		printf("usage: %s n time_to_die time_to_eat time_to_sleep sim_time\n",
			argv[0]);
		return (1);
	}
	run_loop(&sim);
	free_sim(&sim);
	return (0);
}
