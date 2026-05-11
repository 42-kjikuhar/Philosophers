/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:03:30 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/11 20:01:59 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	parse_args(t_sim *sim, int argc, char **argv)
{
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
	if (sim->n <= 0 || sim->time_to_die <= 0 || sim->time_to_eat <= 0
		|| sim->time_to_sleep <= 0)
		return (1);
	return (0);
}

static int	init_mutexes(t_sim *sim)
{
	int	i;

	if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->death_mutex, NULL) != 0)
		return (1);
	i = 0;
	while (i < sim->n)
	{
		if (pthread_mutex_init(&sim->forks[i], NULL) != 0)
			return (1);
		if (pthread_mutex_init(&sim->philos[i].meal_mutex, NULL) != 0)
			return (1);
		i++;
	}
	return (0);
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
	sim->philos = malloc(sizeof(t_philo) * sim->n);
	sim->forks = malloc(sizeof(pthread_mutex_t) * sim->n);
	if (!sim->philos || !sim->forks)
		return (1);
	if (init_mutexes(sim) != 0)
		return (1);
	init_philos(sim);
	return (0);
}
