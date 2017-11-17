#ifndef VITA_TRANSLATOR_H
#define VITA_TRANSLATOR_H

#include <psp2/types.h>
#include <stdint.h>
#include "khash.h"

KHASH_MAP_INIT_INT(INT_STR_TYPE, char*)
#define INT_STR_MAP khash_t(INT_STR_TYPE)

typedef struct TL_CONTEX_S
{
	SceUID file_id;
	INT_STR_MAP* map;
} TL_CONTEX;


int tl_init(TL_CONTEX* ctx, const char* fn_map);

void tl_unit(TL_CONTEX* ctx);

int tl_translate(TL_CONTEX* ctx, const char* jp_str, uint32_t jp_len, 
				 char* cn_str, uint32_t* cn_len);



#endif //VITA_TRANSLATOR_H