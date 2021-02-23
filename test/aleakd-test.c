#include <stdio.h>

//#include <pthread.h>
#include <malloc.h>

int main()
{
	{
		//pthread_t thread = pthread_self();

		char* szTxt = malloc(10);
		free(szTxt);

		printf("Another Leak Detection Library\n");
		//printf("-- Thread: %lu\n", thread);
		//fprintf(stdout, "Another Leak Detection Library\n");
	}
	return 0;
}
