/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pointer_and_array.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kjikuhar <kjikuhar@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 20:06:01 by kjikuhar          #+#    #+#             */
/*   Updated: 2025/11/24 22:02:00 by kjikuhar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdbool.h>

void *routine(void *ptr)
{
	printf("aaa\n");
	return (NULL);
}

int main(int argc, char const *argv[])
{
	int a[5] = {1,2,3,4,5};
	int	*p = a;

	printf("sizeof(a) = %ld\n", sizeof(a));
	printf("sizeof(&routine) = %ld\n", sizeof(&routine));
	printf("sizeof(bool) = %ld\n", sizeof(bool));
	printf("sizeof(&a) = %ld\n", sizeof(&a));
	printf("sizeof(p) = %ld\n", sizeof(p));
	return 0;
}
