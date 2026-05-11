/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:03:30 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/11 21:05:06 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	current_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long)tv.tv_sec * 1000 + (long)tv.tv_usec / 1000);
}

void	log_event(t_sim *sim, int id, const char *msg)
{
	long	now;

	pthread_mutex_lock(&sim->print_mutex);
	pthread_mutex_lock(&sim->death_mutex);
	if (!sim->finished)
	{
		now = current_time_ms() - sim->start_time;
		printf("%ld %d %s\n", now, id, msg);
	}
	pthread_mutex_unlock(&sim->death_mutex);
	pthread_mutex_unlock(&sim->print_mutex);
}

void	free_sim(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->n)
	{
		pthread_mutex_destroy(&sim->forks[i]);
		pthread_mutex_destroy(&sim->philos[i].meal_mutex);
		i++;
	}
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->death_mutex);
	free(sim->forks);
	free(sim->philos);
}
