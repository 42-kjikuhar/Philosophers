/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   correct_test.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 18:53:14 by kjikuhar          #+#    #+#             */
/*   Updated: 2025/11/24 22:18:50 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>

int				count = 0;
pthread_mutex_t	mutex;
void			*pointer = (void *)0x10;

void	*routine(void *p)
{
	int	i;

	i = 0;
	while (i < 10000000)
	{
		pthread_mutex_lock(&mutex);
		count++;
		pthread_mutex_unlock(&mutex);
		i++;
	}
	return (pointer++);
}

int main(int argc, char const *argv[])
{
	(void)argv, (void)argv;
	pthread_t	p1, p2;
	void		*rtv_pointer;

	pthread_mutex_init(&mutex, NULL);
	pthread_create(&p1, NULL, *routine, NULL);
	pthread_create(&p2, NULL, *routine, NULL);

	rtv_pointer = NULL;
	pthread_join(p1, &rtv_pointer);
	printf("rtv_pointer = %p\n", rtv_pointer);
	pthread_join(p2, &rtv_pointer);
	printf("rtv_pointer = %p\n", rtv_pointer);

	pthread_mutex_destroy(&mutex);
	printf("count = %d\n", count);
	return 0;
}
