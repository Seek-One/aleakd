#include <stdio.h>

//#include <pthread.h>
#include <malloc.h>

#include "../src/aleakd.h"

int main()
{
	aleakd_set_current_thread_name("main");

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
