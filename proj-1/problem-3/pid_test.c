/*

    This program is an example of IPC, which prints different information
    from parent and child process.

*/

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>


int main() {
    pid_t parent_pid = getpid();
    pid_t _pid = fork();
    if(_pid<0){
        printf("fork error");
    }
    else if (_pid > 0) {
        // print parent process ID
        printf("518030910059 Parent-PID = %d\n", parent_pid); 
        // wait for the child process 
        wait(NULL);
        printf("Child Complete\n");
    }   else if (_pid == 0) // if it is child process
    {
        pid_t child_pid=getpid();
        printf("518030910059 Child-PID = %d\n", child_pid);
        printf("Result of executing ptree: \n");
        execl("./testARM", "testARM", NULL); // execuate ptree
        _exit(0);
    }
    return 0;
}