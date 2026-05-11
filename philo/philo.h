/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 11:39:50 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/11 21:03:28 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <limits.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_sim
{
	int				n;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				max_meals;
	long			start_time;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	death_mutex;
	pthread_t		monitor;
	struct s_philo	*philos;
	int				finished;
}					t_sim;

typedef struct s_philo
{
	int				id;
	int				left_fork;
	int				right_fork;
	long			last_meal_time;
	int				meals_eaten;
	pthread_mutex_t	meal_mutex;
	pthread_t		thread;
	t_sim			*sim;
}					t_philo;

int					ft_atoi(const char *s);
long				current_time_ms(void);
void				log_event(t_sim *sim, int id, const char *msg);

int					parse_args(t_sim *sim, int argc, char **argv);
int					init_sim(t_sim *sim, int argc, char **argv);
void				free_sim(t_sim *sim);

void				*philo_routine(void *arg);

void				*monitor_routine(void *arg);
int					is_finished(t_sim *sim);

#endif
