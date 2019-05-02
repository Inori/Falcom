#include <string.h>
#include <psp2/types.h>
#include <psp2/kernel/modulemgr.h>

#include "log.h"
#include "tools.h"
#include "hook.h"

#define TL_MAP_FILENAME "app0:string_map.acr"

int _start(SceSize args, void *argp) 
{
	int ret = -1;
	char title_id[VITA_TITLE_ID_LEN] = {0};

	get_title_id(title_id);	

	DEBUG_PRINT("module start - TITLE ID: %s\n", title_id);

	ret = tl_init(&g_tl_context, TL_MAP_FILENAME);

	ret = init_hooks();

	return ret;
}
