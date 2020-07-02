#ifndef _LINUX_MMLIMIT_STRUCT_H
#define _LINUX_MMLIMIT_STRUCT_H

#include <linux/types.h> 
#include <linux/list.h>

struct MmlimitStruct{
	uid_t uid;
	unsigned long mm_max;
	struct list_head list;
};

extern struct MmlimitStruct mmlimit_struct;
//extern struct list_head mmlimit_head;


#endif
