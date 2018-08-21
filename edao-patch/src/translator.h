#ifndef VITA_TRANSLATOR_H
#define VITA_TRANSLATOR_H

#include <psp2/types.h>
#include <stdint.h>
#include "khash.h"

//

#pragma pack (1)

typedef struct _acr_header
{
	uint32_t index_count; //包含索引数，即字符串数
	uint32_t compress_flag; //是否压缩。 0没有压缩
	uint32_t compsize; //如果有压缩，为压缩后大小，否则等于orgsize
	uint32_t orgsize; //如果有压缩，为压缩前【压缩部分】大小，否则为实际大小
} acr_header;

typedef struct _acr_index
{
	uint32_t hash; //oldstr的hash值，用于map查找
	uint32_t new_str_off; //newstr 地址
	uint32_t new_str_len; //newstr 长度
} acr_index;

#pragma pack ()


//

KHASH_MAP_INIT_INT(INT_ACR_NAME, acr_index*)
#define INT_ACR_MAP khash_t(INT_ACR_NAME)

typedef struct TL_CONTEXT_S
{
	INT_ACR_MAP* map;
	uint8_t* map_buffer;
	uint32_t str_count;
} TL_CONTEXT;


//

int tl_init(TL_CONTEXT* ctx, const char* fn_map);

void tl_unit(TL_CONTEXT* ctx);

int tl_translate(TL_CONTEXT* ctx, const char* jp_str, uint32_t jp_len, 
				 char* cn_str, uint32_t* cn_len);

//
int tl_init_hash_map(TL_CONTEXT* ctx, uint8_t* map_buffer, uint32_t map_size);


uint32_t bkdr_hash(const uint8_t* string, uint32_t len);

#endif //VITA_TRANSLATOR_H