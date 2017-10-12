#include <string.h>

#include <psp2/types.h>



#define DEBUG


#include "log.h"
#include "tools.h"


int _start(SceSize args, void *argp) {

	char title_id[VITA_TITLE_ID_LEN] = {0};

	get_title_id(title_id);	

	DEBUG_PRINT("module start - TITLE ID: %s\n", title_id);

	//while(1);

	return 0;
}