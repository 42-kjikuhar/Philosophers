/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:52:32 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 06:10:45 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	take_forks(t_philo *p)
{
	int	first;
	int	second;

	if (p->id % 2 == 0)
	{
		first = p->right_fork;
		second = p->left_fork;
	}
	else
	{
		first = p->left_fork;
		second = p->right_fork;
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
	long	slack;

	log_event(p->sim, p->id, "is sleeping");
	usleep(p->sim->time_to_sleep * 1000);
	log_event(p->sim, p->id, "is thinking");
	slack = p->sim->time_to_die - p->sim->time_to_eat - p->sim->time_to_sleep;
	if (slack > 100)
		usleep(slack * 500);
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
	if (p->id % 2 == 0)
		usleep(p->sim->time_to_eat * 1000);
	while (!is_finished(p->sim))
	{
		take_forks(p);
		do_eat(p);
		release_forks(p);
		do_sleep_think(p);
	}
	return (NULL);
}
