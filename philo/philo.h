/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 11:39:50 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 03:33:29 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_philo_info
{
	int	philo_num;
	int	time_to_die_ms;
	int	time_to_eat_ms;
	int	time_to_sleep_ms;
	int	number_of_times_each_philosopher_must_eat;
}		t_philo_info;

int		free_all(int philo_num, pthread_t *philos, pthread_mutex_t *forks);
int		prepare_forks(int philo_num, pthread_mutex_t *forks);
int		gather_philos(int philo_num, pthread_t *philos);
void	*philo_routine(void *arg);

#endif
