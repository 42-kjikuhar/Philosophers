/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   update.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:03:30 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 04:15:07 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	enter_eating(t_sim *sim, t_philo *p, long now)
{
	take_forks(sim, p);
	print_log(sim, p->id, "has taken a fork", now);
	print_log(sim, p->id, "has taken a fork", now);
	p->state = EATING;
	p->last_meal_time = now;
	p->state_until = now + sim->time_to_eat;
	p->meals_eaten++;
	print_log(sim, p->id, "is eating", now);
}

void	update_philo(t_sim *sim, t_philo *p, long now)
{
	if (p->state == THINKING)
		p->state = HUNGRY;
	else if (p->state == HUNGRY && can_take_forks(sim, p))
		enter_eating(sim, p, now);
	else if (p->state == EATING && now >= p->state_until)
	{
		put_forks(sim, p);
		p->state = SLEEPING;
		p->state_until = now + sim->time_to_sleep;
		print_log(sim, p->id, "is sleeping", now);
	}
	else if (p->state == SLEEPING && now >= p->state_until)
	{
		p->state = THINKING;
		print_log(sim, p->id, "is thinking", now);
	}
}

void	check_death(t_sim *sim, long now)
{
	int	i;

	i = 0;
	while (i < sim->n)
	{
		if (sim->philos[i].state != DEAD && now
			- sim->philos[i].last_meal_time > sim->time_to_die)
		{
			sim->philos[i].state = DEAD;
			print_log(sim, sim->philos[i].id, "died", now);
			sim->finished = 1;
			return ;
		}
		i++;
	}
}
