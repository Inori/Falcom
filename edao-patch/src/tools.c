#include "tools.h"

#include <psp2/kernel/processmgr.h>
#include <psp2/appmgr.h>

#define SCE_APPMGR_APP_PARAM_TITLE_ID 12

int get_title_id(char title_id[VITA_TITLE_ID_LEN])
{
	if (!title_id)
	{
		return -1;
	}

	SceUID pid = sceKernelGetProcessId();
	if (pid < 0)
	{
		return -1;
	}

	int ret = sceAppMgrAppParamGetString(pid, SCE_APPMGR_APP_PARAM_TITLE_ID, title_id, VITA_TITLE_ID_LEN);
	if (ret < 0)
	{
		return -1;
	}
	
	return 0;
}