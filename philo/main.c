/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 01:36:25 by kjikuhar          #+#    #+#             */
/*   Updated: 2026/05/09 03:46:45 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_philo_info	init_info(int argc, char const *argv[])
{
	t_philo_info	info;

	info.philo_num = ft_atoi(argv[1]);
	info.time_to_die_ms = ft_atoi(argv[2]);
	info.time_to_eat_ms = ft_atoi(argv[3]);
	info.time_to_sleep_ms = ft_atoi(argv[4]);
	if (argc == 6)
		info.number_of_times_each_philosopher_must_eat = ft_atoi(argv[5]);
	return (info);
}

int	validate_input(int argc, char const *argv[])
{
	(void)argv;
	if (argc != 5 && argc != 6)
	{
		printf("Your input is wrong.\n");
		return (1);
	}
	return (0);
}

int	main(int argc, char const *argv[])
{
	t_philo_info	info;
	pthread_t		*philos;
	pthread_mutex_t	*forks;

	if (validate_input(argc, argv) != 0)
		return (1);
	info = init_info(argc, argv);
	philos = malloc(sizeof(pthread_t) * info.philo_num);
	forks = malloc(sizeof(pthread_mutex_t) * info.philo_num);
	if (!philos || !forks)
		return (free_all(info.philo_num, philos, forks));
	prepare_forks(info.philo_num, forks);
	printf("Philosophers start eating and thinking.\n");
	gather_philos(info.philo_num, philos);
	free_all(info.philo_num, philos, forks);
	return (0);
}
