/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 18:47:36 by kjikuhar          #+#    #+#             */
/*   Updated: 2025/11/24 18:47:37 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	free_all(philo_info_t *info, pthread_t *philos, pthread_mutex_t *forks)
{
	int	i;

	i = 0;
	while (i < info->philo_num)
	{
		// if (philos[i])
		// {
		// 	pthread_join(philos[i], NULL);
		// }
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

int	prepare_forks(philo_info_t *info, pthread_mutex_t *forks)
{
	int	i;

	i = 0;
	while (i < info->philo_num)
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

int	prepare_philos(philo_info_t *info, pthread_t *philos)
{
	int	i;

	i = 0;
	while (i < info->philo_num)
	{
		if (pthread_create(&philos[i], NULL, test, NULL) == 0)
		{
			return (1);
		}
		i++;
	}
	return (0);
}

int	main(int argc, char const *argv[])
{
	philo_info_t	*info;
	pthread_t		*philos;
	pthread_mutex_t	*forks;

	if (argc != 5 && argc != 6)
	{
		printf("Your input is wrong.\n");
		return (1);
	}
	info->philo_num = atoi(argv[1]);
	info->time_to_die_ms = atoi(argv[2]);
	info->time_to_eat_ms = atoi(argv[3]);
	info->time_to_sleep_ms = atoi(argv[4]);
	if (argc == 6)
		info->number_of_times_each_philosopher_must_eat = atoi(argv[5]);
	philos = malloc(sizeof(pthread_t) * info->philo_num);
	forks = malloc(sizeof(pthread_mutex_t) * info->philo_num);
	if (!philos || !forks)
		return (free_all(info, philos, forks));
	prepare_forks(info, forks);
	prepare_philos(info, philos);
	printf("Hello World!");
	free_all(info, philos, forks);
	return (0);
}
