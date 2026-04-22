#include <stdio.h>
#include <pthread.h>

void	*socrates_routine(void *arg)
{
	(void)arg;
	printf("Socrates is thinking.\n");
	return (NULL);
}

int	main(void)
{
	pthread_t	socrates;

	pthread_create(&socrates, NULL, socrates_routine, NULL);
	pthread_join(socrates, NULL);
	return (0);
}
