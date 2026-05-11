/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:03:30 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/11 20:48:24 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	alloc_buffers(t_sim *sim)
{
	sim->philos = malloc(sizeof(t_philo) * sim->n);
	if (!sim->philos)
		return (1);
	sim->forks = malloc(sizeof(pthread_mutex_t) * sim->n);
	if (!sim->forks)
	{
		free(sim->philos);
		return (1);
	}
	return (0);
}

static int	init_global_mutexes(t_sim *sim)
{
	if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->death_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->print_mutex);
		return (1);
	}
	return (0);
}

static int	init_per_philo_mutexes(t_sim *sim)
{
	int	i;
	int	j;

	i = 0;
	while (i < sim->n && pthread_mutex_init(&sim->forks[i], NULL) == 0)
		i++;
	j = 0;
	if (i == sim->n)
		while (j < sim->n && pthread_mutex_init(&sim->philos[j].meal_mutex,
				NULL) == 0)
			j++;
	if (i == sim->n && j == sim->n)
		return (0);
	while (j > 0)
	{
		j--;
		pthread_mutex_destroy(&sim->philos[j].meal_mutex);
	}
	while (i > 0)
	{
		i--;
		pthread_mutex_destroy(&sim->forks[i]);
	}
	return (1);
}

static void	init_philos(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n)
	{
		sim->philos[i].id = i + 1;
		sim->philos[i].left_fork = i;
		sim->philos[i].right_fork = (i + 1) % sim->n;
		sim->philos[i].last_meal_time = 0;
		sim->philos[i].meals_eaten = 0;
		sim->philos[i].sim = sim;
		i++;
	}
}

int	init_sim(t_sim *sim, int argc, char **argv)
{
	if (parse_args(sim, argc, argv) != 0)
		return (1);
	sim->finished = 0;
	sim->start_time = current_time_ms();
	if (init_global_mutexes(sim) != 0)
		return (1);
	if (alloc_buffers(sim) != 0)
	{
		pthread_mutex_destroy(&sim->print_mutex);
		pthread_mutex_destroy(&sim->death_mutex);
		return (1);
	}
	if (init_per_philo_mutexes(sim) != 0)
	{
		pthread_mutex_destroy(&sim->print_mutex);
		pthread_mutex_destroy(&sim->death_mutex);
		free(sim->philos);
		free(sim->forks);
		return (1);
	}
	init_philos(sim);
	return (0);
}
