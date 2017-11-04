#include "hook.h"
#include "log.h"
#include "tools.h"
#include <stdint.h>
#include <string.h>

#define DECL_INLINE_ASM_ARM __attribute__((naked, target("arm")))
#define DECL_INLINE_ASM_THUMB __attribute__((naked, target("thumb")))

#define ASM_PUSHAD "stmfd sp!, {r0-r12}	\n\t"
#define ASM_POPAD "ldmfd sp!, {r0-r12}	\n\t"
#define ASM_PUSHFD(r) \
	"mrs " #r ", cpsr	\n\t" \
	"str " #r ", [sp, #-4]!		\n\t"
#define ASM_POPFD(r) \
	"ldr " #r ", [sp], #4		\n\t" \
	"msr cpsr," #r "			\n\t"


///////////////////////////////////////////////////////////
typedef struct SUBSTR_ITEM_S
{
	char* old_text;
	uint32_t old_size;

	char* new_text;
	uint32_t new_size;
} SUBSTR_ITEM;


int has_sharp(const char* string, uint32_t len)
{
	if (!string)
	{
		return 0;
	}

	for (uint32_t i = 0; i != len; ++i)
	{
		if (string[i] == '#')
		{
			return 1;
		}
	}

	return 0;
}


SUBSTR_ITEM* find_item(const char* parent_str, uint32_t position,
					   SUBSTR_ITEM items[], uint32_t item_count)
{
	for (uint32_t i = 0; i != item_count; ++i)
	{
		uint32_t cur_pointer = (uint32_t)parent_str + position;
		if (cur_pointer >= (uint32_t)items[i].old_text &&
			cur_pointer < (uint32_t)items[i].old_text + items[i].old_size)
		{
			return &items[i];
		}
	}
	return NULL;
}

void split_string(const char* string, uint32_t str_len, 
				  SUBSTR_ITEM items[], uint32_t* item_count)
{
	if (!string || !item_count)
	{
		return;
	}

	//split string by non visual char
	uint32_t count = *item_count;
	uint32_t n = 0;
	uint32_t cur_size = 0;
	items[n].old_text = (char*)string;
	for (uint32_t i = 0; i != str_len; ++i)
	{
		if ((uint8_t)string[i] >= 0x20)
		{
			cur_size++;
			continue;
		}
		else
		{
			if (cur_size == 0)
			{
				items[n].old_text++;
				continue;
			}
		}

		items[n].old_size = cur_size;
		n++;
		cur_size = 0;

		if (n >= count)
		{
			break;
		}

		uint32_t j = i + 1;
		for (; j != str_len; ++j)
		{
			if ((uint8_t)string[j] < 0x20)
			{
				continue;
			}

			items[n].old_text = (char*)&string[j];
			break;
		}
		i = j - 1;
	}

	if (cur_size != 0)
	{
		items[n].old_size = cur_size;
		n++;
	}

	*item_count = n;
	for (uint32_t k = 0; k != n; ++k)
	{
		char* str = items[k].old_text;
		uint32_t len = items[k].old_size;

		if (!has_sharp(str, len))
		{
			continue;
		}

		//if string contains "#", we skip over the opcodes.
		uint32_t m = 0;
		for (; m != len; ++m)
		{
			if (!is_acsii_char(str[m]))
			{
				break;
			}
		}

		items[k].old_text = str + m;
		items[k].old_size = len - m;
	}
}


#define SUBSTR_ITEM_MAX 32
char debug_buffer[4096] = {0};

char* new_scp_process_scena(char* text)
{
	SUBSTR_ITEM str_items[SUBSTR_ITEM_MAX] = {0};

	if (!text)
	{
		return text;
	}

	uint32_t item_count = SUBSTR_ITEM_MAX;  //will be real count after call

	uint32_t str_len = strlen(text);
	split_string(text, str_len, str_items, &item_count);

	DEBUG_PRINT("strlen: %d\n", str_len);

	dump_mem("STR:", text, str_len);
	for (uint32_t i = 0; i != item_count; ++i)
	{
		DEBUG_PRINT("%d: start: %d size:%d\n", i,
			(uint32_t)(str_items[i].old_text - text), str_items[i].old_size);
	}


	return text;
}


//Note: r6 saves the old function address

DECL_INLINE_ASM_THUMB void new_scp_process_scena_stub()
{
	asm volatile (
		"sub sp, #4 \n\t"
		ASM_PUSHAD
		ASM_PUSHFD(r7)
		"mov r0, r1 \n\t"
		"blx new_scp_process_scena \n\t"
		"str r0, [sp, #0x38] \n\t"
		ASM_POPFD(r7)
		ASM_POPAD
		"ldr r1, [sp], #4 \n\t"
		"orr r6, r6, #1 \n\t"
		"bx r6 \n\t"
	);
}



///////////////////////////////////////////////////////////
uint32_t* p_entry_scp_process_scena = (uint32_t*)0x8127C9A8;


int init_hooks()
{

	*p_entry_scp_process_scena = (uint32_t)ADDR_THUMB(new_scp_process_scena_stub);
	DEBUG_PRINT("Hook Addr: 0x%08X Value: 0x%08X\n", p_entry_scp_process_scena, *p_entry_scp_process_scena);

	return 0;
}