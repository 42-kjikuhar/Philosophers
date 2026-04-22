/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_sandbox.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 03:22:38 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/04/23 03:22:39 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>

void	*plato_routine(void *fork)
{
	printf("Plato is thinking.\n");
	pthread_mutex_lock(fork);
	printf("Plato gets a fork.\n");
	pthread_mutex_unlock(fork);
	printf("Plato leaves a fork.\n");
	return (NULL);
}
void	*socrates_routine(void *fork)
{
	printf("Socrates is thinking.\n");
	pthread_mutex_lock(fork);
	printf("Socrates gets a fork.\n");
	pthread_mutex_unlock(fork);
	printf("Socrates leaves a fork.\n");
	return (NULL);
}

int	main(void)
{
	pthread_t		socrates;
	pthread_t		plato;
	pthread_mutex_t	fork;

	pthread_mutex_init(&fork, NULL);
	pthread_create(&socrates, NULL, socrates_routine, &fork);
	pthread_create(&plato, NULL, plato_routine, &fork);
	pthread_join(socrates, NULL);
	pthread_join(plato, NULL);
	pthread_mutex_destroy(&fork);
	return (0);
}
