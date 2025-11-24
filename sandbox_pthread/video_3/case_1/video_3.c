/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   video_3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 14:42:50 by kei2003730        #+#    #+#             */
/*   Updated: 2025/11/24 22:26:44 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int		mails = 0;
void	*routine(void *ptr)
{
	(void)ptr;
	for (int i = 0; i < 10000000; i++)
	{
		mails++;
		// read mails
		// increment
		// write mails
	}
	return (NULL);
}

int	main(void)
{
	pthread_t t1, t2;
	if (pthread_create(&t1, NULL, &routine, NULL) != 0)
		return (1);
	if (pthread_create(&t2, NULL, &routine, NULL) != 0)
		return (2);
	if (pthread_join(t1, NULL) != 0)
		return (3);
	if (pthread_join(t2, NULL) != 0)
		return (4);
	printf("Number of mails: %d", mails);
	return (0);
}
