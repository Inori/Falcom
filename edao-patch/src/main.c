#include <string.h>
#include <psp2/types.h>
#include <psp2/kernel/modulemgr.h>

#include "log.h"
#include "tools.h"
#include "hook.h"



int _start(SceSize args, void *argp) 
{

	char title_id[VITA_TITLE_ID_LEN] = {0};

	get_title_id(title_id);	

	DEBUG_PRINT("module start - TITLE ID: %s\n", title_id);


	dump_mem("ScpProcessText:", (unsigned char *)0x81087520, 0x10);
	dump_mem("Hooked:", (unsigned char *)0x81235560, 0x10);
	DEBUG_PRINT("init hooks begin.\n");

	int ret = init_hooks();

	DEBUG_PRINT("init hooks done:%d\n", ret);
	//while(1);

	return 0;
}
