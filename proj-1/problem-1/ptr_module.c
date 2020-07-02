#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/unistd.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_LICENSE("Dual BSD/GPL");
#define __NR_hellocall 356
#define NUMBER 4000  // the posible max ammount of the process which can be changed 
static int (*oldcall)(void);

struct prinfo {
    pid_t parent_pid;
    pid_t pid;
    pid_t first_child_pid;
    pid_t next_sibling_pid;
    long state;
    long uid;
    char comm[16];
};


// this function helps to move the information from the old task_struct  to new prinfo
void move (struct task_struct * from_old, struct prinfo *to_new   )
{

    to_new->pid = from_old->pid;

    //avoid the null_ptr
    to_new->parent_pid =  (from_old->parent) ?  from_old->parent->pid: 0 ;

    // we should judge whether the children list is empty, and the children.next always points to the sibling member
    to_new->first_child_pid = list_empty(&(from_old->children)) ? 0 : list_entry(from_old->children.next, struct task_struct, sibling)->pid;

    // the same as child_pid
    to_new->next_sibling_pid = list_empty(&(from_old->sibling)) ? 0 : list_entry(from_old->sibling.next, struct task_struct, sibling)->pid;

    to_new->state = from_old->state;

    //which is defined in cred.h
    to_new->uid = from_old->cred->uid;

    //which is definee in exec.c
    get_task_comm(to_new->comm, from_old);

}



// the function used to implement the DFS
void traverse(struct task_struct *start, struct prinfo *buf, int *nr )
{
   
    struct list_head *pos; // iterator which points to the position of child-list
    struct task_struct *task;
     
    move (start, &buf[*nr]); // copy to new space
    ++(*nr);// increase the total number

    list_for_each(pos, &(start->children)) // iterate over children processes
    {
        task= list_entry(pos, struct task_struct, sibling);//get the task through list-head
        traverse(task, buf, nr);
    }
}


static int ptree(struct prinfo *buf, int *nr) {
    
   
    struct prinfo * kernel_buf = (struct prinfo*) kmalloc(sizeof(struct prinfo)*NUMBER,GFP_KERNEL) ;
    int kernel_nr=0;
    printk("The 356 syscall is called\n");

    //handle the exception
    if(!kernel_buf){
        printk("memory allocated error!!\n");
        return 0 ;
    }
    
    


    //set a lock for protection 
    read_lock(&tasklist_lock);

    traverse(&init_task,kernel_buf,&kernel_nr);

    read_unlock(&tasklist_lock);
    
    //printk("travers successfully!!");


    //copy the information to user space
    if (copy_to_user(buf, kernel_buf, sizeof(struct prinfo) * NUMBER)||copy_to_user(nr, &kernel_nr, sizeof(int)))
    {
        printk("Error when copying\n");
        return 0;
    }


    // release the memory in kernel space
    kfree(kernel_buf);
    return 1;
}




static int addsyscall_init(void) {
    long *syscall = (long *) 0xc000d8c4;
    oldcall = (int (*)(void)) (syscall[__NR_hellocall]);
    syscall[__NR_hellocall] = (unsigned long) ptree;
    printk(KERN_INFO
    "module load!\n");
    return 0;

}

static void addsyscall_exit(void) {
    long *syscall = (long *) 0xc000d8c4;
    syscall[__NR_hellocall] = (unsigned long) oldcall;
    printk(KERN_INFO
    "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);