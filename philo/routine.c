/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:52:32 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 05:34:37 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	take_forks(t_philo *p)
{
	int	first;
	int	second;

	if (p->left_fork < p->right_fork)
	{
		first = p->left_fork;
		second = p->right_fork;
	}
	else
	{
		first = p->right_fork;
		second = p->left_fork;
	}
	pthread_mutex_lock(&p->sim->forks[first]);
	log_event(p->sim, p->id, "has taken a fork");
	pthread_mutex_lock(&p->sim->forks[second]);
	log_event(p->sim, p->id, "has taken a fork");
}

static void	release_forks(t_philo *p)
{
	pthread_mutex_unlock(&p->sim->forks[p->right_fork]);
	pthread_mutex_unlock(&p->sim->forks[p->left_fork]);
}

static void	do_eat(t_philo *p)
{
	pthread_mutex_lock(&p->meal_mutex);
	p->last_meal_time = current_time_ms() - p->sim->start_time;
	p->meals_eaten++;
	pthread_mutex_unlock(&p->meal_mutex);
	log_event(p->sim, p->id, "is eating");
	usleep(p->sim->time_to_eat * 1000);
}

static void	do_sleep_think(t_philo *p)
{
	log_event(p->sim, p->id, "is sleeping");
	usleep(p->sim->time_to_sleep * 1000);
	log_event(p->sim, p->id, "is thinking");
}

void	*philo_routine(void *arg)
{
	t_philo	*p;

	p = (t_philo *)arg;
	if (p->sim->n == 1)
	{
		log_event(p->sim, p->id, "has taken a fork");
		usleep(p->sim->time_to_die * 1000 + 1000);
		return (NULL);
	}
	while (!is_finished(p->sim))
	{
		take_forks(p);
		do_eat(p);
		release_forks(p);
		do_sleep_think(p);
	}
	return (NULL);
}
