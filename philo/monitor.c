/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:52:32 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 05:08:43 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	is_finished(t_sim *sim)
{
	int	f;

	pthread_mutex_lock(&sim->state_mutex);
	f = sim->finished;
	pthread_mutex_unlock(&sim->state_mutex);
	return (f);
}

static int	check_one_death(t_sim *sim)
{
	int		i;
	long	now;
	long	last;

	i = 0;
	while (i < sim->n)
	{
		pthread_mutex_lock(&sim->state_mutex);
		now = current_time_ms() - sim->start_time;
		last = sim->philos[i].last_meal_time;
		if (now - last > sim->time_to_die && !sim->finished)
		{
			pthread_mutex_lock(&sim->print_mutex);
			printf("%ld %d died\n", now, sim->philos[i].id);
			pthread_mutex_unlock(&sim->print_mutex);
			sim->finished = 1;
			pthread_mutex_unlock(&sim->state_mutex);
			return (1);
		}
		pthread_mutex_unlock(&sim->state_mutex);
		i++;
	}
	return (0);
}

static int	check_all_satisfied(t_sim *sim)
{
	int	i;

	if (sim->max_meals < 0)
		return (0);
	pthread_mutex_lock(&sim->state_mutex);
	i = 0;
	while (i < sim->n)
	{
		if (sim->philos[i].meals_eaten < sim->max_meals)
		{
			pthread_mutex_unlock(&sim->state_mutex);
			return (0);
		}
		i++;
	}
	sim->finished = 1;
	pthread_mutex_unlock(&sim->state_mutex);
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!is_finished(sim))
	{
		if (check_one_death(sim))
			return (NULL);
		if (check_all_satisfied(sim))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
