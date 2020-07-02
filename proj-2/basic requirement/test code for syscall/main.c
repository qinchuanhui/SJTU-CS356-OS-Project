#include <stdio.h>
#include <unistd.h>

int main() {
    printf("this is a test:\n\n");
    int i=syscall(385,10070,1000);
	i=syscall(385,10080,9000);
	i=syscall(385,10010,10050);
	i=syscall(385,10070,10004);
    printf("Answer is %d!\n",i);
    printf("Test End\n");
    return 0;

}
