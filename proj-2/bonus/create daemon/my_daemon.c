#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

int main(int argc, char *argv[])
{	
	// return 0 for success
	if(daemon(0, 0))
	{
		printf("daemon error\n");
		exit(1);
	}

	syscall(386, 100);
		
	return 0;
}
