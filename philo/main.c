/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 01:36:25 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/04/23 02:16:50 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	free_all(int philo_num, pthread_t *philos, pthread_mutex_t *forks)
{
	int	i;

	i = 0;
	while (i < philo_num)
	{
		if (philos[i])
		{
			pthread_join(philos[i], NULL);
		}
		if (&forks[i])
		{
			pthread_mutex_destroy(&forks[i]);
		}
		i++;
	}
	if (philos)
		free(philos);
	if (forks)
		free(forks);
	return (0);
}

int	prepare_forks(int philo_num, pthread_mutex_t *forks)
{
	int	i;

	i = 0;
	while (i < philo_num)
	{
		if (pthread_mutex_init(&forks[i], NULL) != 0)
		{
			return (1);
		}
		i++;
	}
	return (0);
}

int	test(void *arg)
{
}

int	prepare_philos(int philo_num, pthread_t *philos)
{
	int	i;

	i = 0;
	while (i < philo_num)
	{
		if (pthread_create(&philos[i], NULL, test, NULL) != 0)
		{
			return (1);
		}
		i++;
	}
	return (0);
}

int	main(int argc, char const *argv[])
{
	t_philo_info	info;
	pthread_t		*philos;
	pthread_mutex_t	*forks;

	if (argc != 5 && argc != 6)
	{
		printf("Your input is wrong.\n");
		return (1);
	}
	info.philo_num = atoi(argv[1]);
	info.time_to_die_ms = atoi(argv[2]);
	info.time_to_eat_ms = atoi(argv[3]);
	info.time_to_sleep_ms = atoi(argv[4]);
	if (argc == 6)
		info.number_of_times_each_philosopher_must_eat = atoi(argv[5]);
	philos = malloc(sizeof(pthread_t) * info.philo_num);
	forks = malloc(sizeof(pthread_mutex_t) * info.philo_num);
	if (!philos || !forks)
		return (free_all(info.philo_num, philos, forks));
	prepare_forks(info.philo_num, forks);
	prepare_philos(info.philo_num, philos);
	printf("Hello World!");
	free_all(info.philo_num, philos, forks);
	return (0);
}
