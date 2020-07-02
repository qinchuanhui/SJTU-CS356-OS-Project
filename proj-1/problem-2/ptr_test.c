/*

    This program is used to test the correctness of ptr_module and print the result
    by calling the 356 syscall.

*/

#include <stdio.h>
#include <sys/types.h>
#include <memory.h>

#define NUMBER 4000
#define NUMBER1 4093 //we choose this number to form a hash-liked map

struct prinfo {
    pid_t parent_pid;
    pid_t pid;
    pid_t first_child_pid;
    pid_t next_sibling_pid;
    long state;
    long uid;
    char comm[16];
};

//used to store the result
struct prinfo buf [NUMBER];

//this two sequence form a hash-like map, which maps the pid to its depth of ptree.
int depth [NUMBER1];
int pid_sequence[NUMBER1];

//init the hash-like map
void init(){
    int i=0;
    for (i=0;i<NUMBER1;++i){
        pid_sequence[i]=-1;
    }
    pid_sequence[0]=0;
    depth[0]=0;
}


//the map is organized like a linear hash map
//set the depth of the corresponding pid (process)
void set_depth(int pid, int dep){
    int find=pid%NUMBER1;
    while (pid_sequence[find]!=-1){
        find++;
        find=find%NUMBER1;
    }
    pid_sequence[find]=pid;
    depth[find]=dep;

}

//get the depth of the corresponding pid (process)
int get_depth(int pid){
    int find=pid%NUMBER1;
    while (pid_sequence[find]!=pid){
        find++;
        find=find%NUMBER1;
    }
    return depth[find];
}

//deal with the special process separately
void output_swapper(){
    struct prinfo* p=buf;
    printf("%s,%d,%ld,%d,%d,%d,%ld\n", p->comm, p->pid, p->state, p->parent_pid, p->first_child_pid,
           p->next_sibling_pid, p->uid);
}


//the major function to meet the output requirement
void  output_deeply (struct prinfo* start, int *counter,int nr){

    struct prinfo* p=start;

    //if all of the process is printed, return.
    if (*counter==nr)
        return;

    //when start a new branch, first get the parent depth
    int dep = get_depth(p->parent_pid);

    //traverse all node in this branch
    do {
        (*counter)++;
        dep++;
        set_depth(p->pid, dep);

        //format print
        int i=1;
        for ( i = 1; i <= dep; ++i) {
            printf("\t");
        }
        printf("%s,%d,%ld,%d,%d,%d,%ld\n", p->comm,p->pid, p->state, p->parent_pid, p->first_child_pid,
               p->next_sibling_pid, p->uid);

        p++;
    }while ((p-1)->first_child_pid);//when there is no child, it means a branch has been finished

    //turn to a new branch
    output_deeply(p,counter,nr);

}


int main(){

    //use the syscall
    int nr;
    int res=syscall(356,buf,&nr);
    if (!res) {
        printf("syscall error");
        return 0;
    }

    //the data for debug is as follows
    /*buf[0].pid=0; buf[0].first_child_pid=3;buf[0].parent_pid=0;
    buf[1].pid=3;buf[1].first_child_pid=12;buf[1].parent_pid=0;
    buf[2].pid=12;buf[2].first_child_pid=19;buf[2].parent_pid=3;
    buf[3].pid=19;buf[3].first_child_pid=0;buf[3].parent_pid=12;
    buf[4].pid=4098;buf[4].first_child_pid=0;buf[4].parent_pid=12;
    buf[5].pid=17;buf[5].first_child_pid=0;buf[5].parent_pid=3;
    buf[6].pid=5;buf[6].first_child_pid=0;buf[6].parent_pid=0;
    */
    init();

    //deal with the special process
    output_swapper();
    
    //do other output
    int counter=1;
    struct prinfo* it =&buf[1];
    output_deeply(it,&counter,nr);
    
    return 0;

}