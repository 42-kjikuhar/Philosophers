/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:52:32 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 05:24:54 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	is_finished(t_sim *sim)
{
	int	f;

	pthread_mutex_lock(&sim->death_mutex);
	f = sim->finished;
	pthread_mutex_unlock(&sim->death_mutex);
	return (f);
}

static void	declare_death(t_sim *sim, int id, long now)
{
	pthread_mutex_lock(&sim->print_mutex);
	pthread_mutex_lock(&sim->death_mutex);
	if (!sim->finished)
	{
		printf("%ld %d died\n", now, id);
		sim->finished = 1;
	}
	pthread_mutex_unlock(&sim->death_mutex);
	pthread_mutex_unlock(&sim->print_mutex);
}

static int	check_one_death(t_sim *sim)
{
	int		i;
	long	now;
	long	last;

	i = 0;
	while (i < sim->n)
	{
		pthread_mutex_lock(&sim->philos[i].meal_mutex);
		last = sim->philos[i].last_meal_time;
		now = current_time_ms() - sim->start_time;
		pthread_mutex_unlock(&sim->philos[i].meal_mutex);
		if (now - last > sim->time_to_die)
		{
			declare_death(sim, sim->philos[i].id, now);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	check_all_satisfied(t_sim *sim)
{
	int	i;
	int	meals;

	if (sim->max_meals < 0)
		return (0);
	i = 0;
	while (i < sim->n)
	{
		pthread_mutex_lock(&sim->philos[i].meal_mutex);
		meals = sim->philos[i].meals_eaten;
		pthread_mutex_unlock(&sim->philos[i].meal_mutex);
		if (meals < sim->max_meals)
			return (0);
		i++;
	}
	pthread_mutex_lock(&sim->death_mutex);
	sim->finished = 1;
	pthread_mutex_unlock(&sim->death_mutex);
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;

	sim = (t_sim *)arg;
	while (!is_finished(sim))
	{
		if (check_all_satisfied(sim))
			return (NULL);
		if (check_one_death(sim))
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
