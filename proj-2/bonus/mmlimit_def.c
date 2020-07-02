#include <linux/mmlimit_struct.h>
#include <linux/types.h>
#include <linux/list.h>
#include<linux/init.h>
#include<linux/module.h>

struct MmlimitStruct mmlimit_struct={
	.uid = 0,
	.mm_max = 0,
	.time =0 ,
	.mask = 0,
	.flag = 0,
	.list = LIST_HEAD_INIT(mmlimit_struct.list), 
};


/*struct list_head mmlimit_head={
	&mmlimit_head,
	&mmlimit_head};

EXPORT_SYMBOL(mmlimit_head);
//list_add(&mmlimit_struct.list,&mmlimit_head);
*/