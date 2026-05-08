/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:03:30 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 04:42:32 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	can_take_forks(t_sim *sim, t_philo *p)
{
	if (p->left_fork == p->right_fork)
		return (0);
	return (sim->fork_used[p->left_fork] == 0
		&& sim->fork_used[p->right_fork] == 0);
}

void	take_forks(t_sim *sim, t_philo *p)
{
	sim->fork_used[p->left_fork] = 1;
	sim->fork_used[p->right_fork] = 1;
}

void	put_forks(t_sim *sim, t_philo *p)
{
	sim->fork_used[p->left_fork] = 0;
	sim->fork_used[p->right_fork] = 0;
}
