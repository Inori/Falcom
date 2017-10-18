#include "hook.h"
#include "log.h"
#include "tools.h"

int hook_scp_process_text(uint32_t arg1, char* text, uint32_t arg3, uint32_t arg4)
{
	if (!text)
	{
		return 0;
	}

	char* string = rip_string(text);
	if (!string)
	{
		return 0;
	}

	DEBUG_PRINT("%s\n", string);

	return 0;
}



///////////////////////////////////////////////////////////
uint32_t* p_sce_process_text_entry = (uint32_t*)0x8127C9A8;


int init_hooks()
{

	*p_sce_process_text_entry = (uint32_t)ADDR_THUMB(hook_scp_process_text);
	DEBUG_PRINT("Hook Addr: 0x%08X Value: 0x%08X\n", p_sce_process_text_entry, *p_sce_process_text_entry);

	return 0;
}