/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_sandbox.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/23 03:17:46 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/04/23 03:17:47 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>

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
	pthread_mutex_t	fork;

	pthread_mutex_init(&fork, NULL);
	pthread_create(&socrates, NULL, socrates_routine, &fork);
	pthread_join(socrates, NULL);
	pthread_mutex_destroy(&fork);
	return (0);
}
