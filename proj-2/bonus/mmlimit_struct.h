#ifndef _LINUX_MMLIMIT_STRUCT_H
#define _LINUX_MMLIMIT_STRUCT_H

#include <linux/types.h> 
#include <linux/list.h>
#include <linux/timer.h>

struct MmlimitStruct{
	uid_t uid;
	unsigned long mm_max;
	unsigned int time;
	struct list_head list;
	struct timer_list user_timer; // to set a timer
	bool mask; //to avoid to be killed during allowed_time
	bool flag; //to judge whether it's the second time or not.
};

extern struct MmlimitStruct mmlimit_struct;
//extern struct list_head mmlimit_head;


#endif
