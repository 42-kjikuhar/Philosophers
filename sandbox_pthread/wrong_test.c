/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wrong_test.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 18:53:14 by kjikuhar          #+#    #+#             */
/*   Updated: 2025/11/24 18:59:59 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>

int		count = 0;

void	*routine(void *p)
{
	int	i;

	i = 0;
	while (i < 10000000)
	{
		count++;
		i++;
	}
	return (NULL);
}

int main(int argc, char const *argv[])
{
	(void)argv, (void)argv;

	pthread_t	p1, p2;

	pthread_create(&p1, NULL, *routine, NULL);
	pthread_create(&p2, NULL, *routine, NULL);

	pthread_join(p1, NULL);
	pthread_join(p2, NULL);

	printf("count = %d\n", count);
	return 0;
}
