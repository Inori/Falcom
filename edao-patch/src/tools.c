#include "tools.h"

#include <stdio.h>
#include <string.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/appmgr.h>
#include <psp2/kernel/sysmem.h>

#include "log.h"

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

void hex_to_string(unsigned char* data, int data_len, char* string, int str_len)
{
	if (!data || !string || !data_len || !str_len)
	{
		return;
	}

	if (data_len * 2  > str_len - 1)
	{
		return;
	}

	int i = 0;
	for (; i != data_len; ++i)
	{
		snprintf(&string[i * 2], str_len - i * 2, "%02X", data[i]);
	}
	string[str_len - 1] = 0;
}


void dump_mem(char* tag, unsigned char* data, int len)
{
	if (!data || !len)
	{
		return;
	}

	char* dbg_string = (char*)vita_malloc(len * 2 + 1);

	hex_to_string(data, len, dbg_string, len * 2 + 1);

	DEBUG_PRINT("[%s]: %s\n", tag, dbg_string);

	vita_free(dbg_string);
}


void* vita_malloc(unsigned int size)
{
	if (!size)
	{
		return NULL;
	}

	unsigned int inner_size = size + sizeof(SceUID);
	SceUID block = sceKernelAllocMemBlock("asuka", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW, ALIGN(inner_size, PAGE_SIZE), NULL);
	if (block <= 0)
	{
		return NULL;
	}

	void* base = NULL;
	sceKernelGetMemBlockBase(block, &base);
	if (!base)
	{
		sceKernelFreeMemBlock(block);
		return NULL;
	}

	memcpy(base, &block, sizeof(SceUID));
	void* user_base = (void*)((unsigned char*)base + sizeof(SceUID));

	return user_base;
}

void vita_free(void* mem)
{
	if (!mem)
	{
		return;
	}

	unsigned char* inner_mem = (unsigned char*)mem - sizeof(SceUID);
	SceUID block = 0;
	memcpy(&block, inner_mem, sizeof(SceUID));
	if (block <= 0)
	{
		return;
	}
	sceKernelFreeMemBlock(block);
}


//string tools

#define EDAO_STR_BUFF_LEN 1024


int is_acsii_char(unsigned char c)
{
	if (c >= 0x20 && c <= 0x7E)
	{
		return 1;
	}
	if (c == '\t' || c == '\n' || c == '\r')
	{
		return 1;
	}
	return 0;
}

int is_sjis_char(unsigned char c)
{
	//half-width sjis char
	if (c >= 0xA1 && c <= 0xDF)
	{
		return 1;
	}

	//sjis first char
	if ((c >= 0x81 && c <= 0x9F) ||
		(c >= 0xE0 && c <= 0xEF))
	{
		return 1;
	}

	//sjis second char
	if ((c >= 0x40 && c <= 0x7E) ||
		(c >= 0x80 && c <= 0xFC))
	{
		return 1;
	}
	return 0;
}

int is_opcode(unsigned char c)
{
	return (!is_acsii_char(c) && !is_sjis_char(c));
}

char* rip_string(char* old_str)
{
	static char string_buff[EDAO_STR_BUFF_LEN] = {0};

	if (!old_str)
	{
		return NULL;
	}

	int len = strlen(old_str);
	int j = 0;
	for(int i = 0; i != len; ++i)
	{
		if (!is_opcode(old_str[i]))
		{
			string_buff[j++] = old_str[i];
		}
	}
	string_buff[j] = 0;
	return string_buff;
}