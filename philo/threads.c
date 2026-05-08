/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 03:34:10 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 03:37:17 by kjikuhar         ###   ########.fr       */
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
		pthread_mutex_destroy(&forks[i]);
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

void	*philo_routine(void *arg)
{
	(void)arg;
	return (NULL);
}

int	gather_philos(int philo_num, pthread_t *philos)
{
	int	i;

	i = 0;
	while (i < philo_num)
	{
		if (pthread_create(&philos[i], NULL, philo_routine, NULL) != 0)
		{
			return (1);
		}
		i++;
	}
	return (0);
}
