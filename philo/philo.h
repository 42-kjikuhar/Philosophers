/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 11:39:50 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/04/23 02:03:24 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <pthread.h>
# include <unistd.h>

typedef struct s_philo_info
{
	int	philo_num;
	int	time_to_die_ms;
	int	time_to_eat_ms;
	int	time_to_sleep_ms;
	int	number_of_times_each_philosopher_must_eat;
}	t_philo_info;

#endif
