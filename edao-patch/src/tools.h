#ifndef VITA_TOOLS_H
#define VITA_TOOLS_H

#include <stdint.h>

#define VITA_TITLE_ID_LEN 12

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif

#define ALIGN(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

#define ADDR_THUMB(addr) ((unsigned int)(addr) | 1)
#define ADDR_ARM(addr) ((unsigned int)(addr) & ~1)

#define DECL_INLINE_ASM_ARM __attribute__((naked, target("arm")))
#define DECL_INLINE_ASM_THUMB __attribute__((naked, target("thumb")))

#define DECL_FUNCTION_ARM __attribute__((target("arm")))
#define DECL_FUNCTION_THUMB __attribute__((target("thumb")))
#define DECL_FUNCTION_NOINLINE __attribute__ ((noinline))


int get_title_id(char title_id[VITA_TITLE_ID_LEN]);

void hex_to_string(unsigned char* data, int data_len, char* string, int str_len);

void dump_mem(char* tag, unsigned char* data, int len);

void* vita_malloc(unsigned int size);

void* vita_calloc(unsigned int num, unsigned int size);

void* vita_realloc(void* ptr, unsigned int size);

void vita_free(void* mem);


//string tools

int is_acsii_char(unsigned char c);

int is_sjis_char(unsigned char c);

char* rip_string(char* old_str);


#endif