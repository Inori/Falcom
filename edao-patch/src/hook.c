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

char* new_scp_process_scena(char* text)
{
	if (!text)
	{
		return text;
	}

	char* string = rip_string(text);
	if (!string)
	{
		return text;
	}

	DEBUG_PRINT("%s\n", string);

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