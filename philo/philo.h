/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serial_philo.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/09 04:03:30 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 04:15:07 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef enum e_state
{
	THINKING,
	HUNGRY,
	EATING,
	SLEEPING,
	DEAD
}			t_state;

typedef struct s_philo
{
	int		id;
	int		left_fork;
	int		right_fork;
	t_state	state;
	long	last_meal_time;
	long	state_until;
	int		meals_eaten;
}			t_philo;

typedef struct s_sim
{
	int		n;
	long	time_to_die;
	long	time_to_eat;
	long	time_to_sleep;
	long	simulation_time;
	long	start_time;
	int		*fork_used;
	t_philo	*philos;
	int		finished;
}			t_sim;

int			ft_atoi(const char *s);
long		current_time_ms(void);
void		print_log(t_sim *sim, int id, const char *msg, long now);

int			can_take_forks(t_sim *sim, t_philo *p);
void		take_forks(t_sim *sim, t_philo *p);
void		put_forks(t_sim *sim, t_philo *p);

void		update_philo(t_sim *sim, t_philo *p, long now);
void		check_death(t_sim *sim, long now);

int			init_sim(t_sim *sim, int argc, char **argv);
void		free_sim(t_sim *sim);
void		run_loop(t_sim *sim);

#endif
