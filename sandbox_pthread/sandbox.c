/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandbox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kei2003730 <kei2003730@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 12:24:45 by kei2003730        #+#    #+#             */
/*   Updated: 2025/07/11 12:48:39 by kei2003730       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void *test(void *arg)
{
   printf("test\n");
   sleep(1);
}

int main(void)
{
   pthread_t thread;
   void *retval;

   if (pthread_create(&thread, NULL, test, NULL) != 0)
       return (0);
   pthread_join(thread, &retval);
   pthread_detach(thread);
}
