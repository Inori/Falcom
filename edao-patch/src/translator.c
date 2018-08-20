#include "translator.h"
#include "log.h"
#include "tools.h"
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>



int tl_init(TL_CONTEXT* ctx, const char* fn_map)
{
	int ret = -1;
	SceUID file_id = 0;
	acr_header header = {0};
	do
	{
		if (!ctx || !fn_map)
		{
			break;
		}

		file_id = sceIoOpen(fn_map, SCE_O_RDONLY, 0777);
		if (file_id < 0)
		{
			break;
		}

		int read = sceIoRead(file_id, &header, sizeof(header));
		if (read != sizeof(header))
		{
			break;
		}

		if (header.compress_flag)
		{
			break;
		}

		ctx->str_count = header.index_count;

		uint32_t buffer_size = header.orgsize;
		uint8_t* map_buffer = vita_malloc(buffer_size);
		if (!map_buffer)
		{
			break;
		}

		ctx->map_buffer = map_buffer;

		read = sceIoRead(file_id, map_buffer, buffer_size);
		if (read != buffer_size)
		{
			break;
		}

		ret = tl_init_hash_map(ctx, map_buffer, buffer_size);
		if (ret < 0)
		{
			break;
		}

		ret = 0;
	} while(0);

	if (file_id > 0)
	{
		sceIoClose(file_id);
	}
	return ret;
}

void tl_unit(TL_CONTEXT* ctx)
{
	if (!ctx)
	{
		return;
	}
	if (ctx->map_buffer)
	{
		vita_free(ctx->map_buffer);
	}
	if (ctx->map)
	{
		kh_destroy(INT_ACR_NAME, ctx->map);
	}
	memset(ctx, 0, sizeof(TL_CONTEXT));
}

/*
def BKDRHash(bytes):
    hash = 0
    seed = 131
    for c in bytes:
        if c != 0:
            hash = hash * seed + c
    return (hash & 0x7FFFFFFF)
*/

inline uint32_t bkdr_hash(const uint8_t* string, uint32_t len)
{
	const uint32_t seed = 131;
	register uint32_t hash = 0;

	for (uint32_t i = 0; i != len; ++i)
	{
		if (string[i] != 0)
		{
			hash = hash * seed + string[i];
		}
	}
	return (hash & 0x7FFFFFFF);
}

int tl_init_hash_map(TL_CONTEXT* ctx, uint8_t* map_buffer, uint32_t map_size)
{
	int ret = -1;
	do
	{
		if (!ctx || !map_buffer || !map_size)
		{
			break;
		}

		INT_ACR_MAP* map = kh_init(INT_ACR_NAME);
		if (!map)
		{
			break;
		}

		uint32_t str_count = ctx->str_count;
		acr_index* str_index = (acr_index*)map_buffer;
		khiter_t iter = 0;
		khint_t kret = -1;

		for (uint32_t i = 0; i != str_count; ++i)
		{
			iter = kh_put(INT_ACR_NAME, map, str_index[i].hash, &kret);
			if (kret == -1)
			{
				continue;
			}
			kh_val(map, iter) = &str_index[i];
		}

		ctx->map = map;

		ret = 0;
	} while (0);

	return ret;
}


int tl_translate(TL_CONTEXT* ctx, const char* jp_str, uint32_t jp_len, 
				 char* cn_str, uint32_t* cn_len)
{
	int translated = 0;
	do
	{
		if (!ctx)
		{
			break;
		}

		INT_ACR_MAP* map = ctx->map;

		uint32_t hash = bkdr_hash((uint8_t*)jp_str, jp_len);

		//dump_mem("jpstr", jp_str, jp_len);
		//DEBUG_PRINT("jp hash: 0x%08X\n", hash);

		khiter_t iter = kh_get(INT_ACR_NAME, map, hash);
		if (iter == kh_end(map))
		{
			break;
		}

		//DEBUG_PRINT("find key sucess\n");

		acr_index* index = kh_val(map, iter);
		if (!index)
		{
			break;
		}

		char* new_str = (char*)(ctx->map_buffer - sizeof(acr_header) + index->new_str_off);
		uint32_t new_len = index->new_str_len;
		if (!new_str || !new_len)
		{
			break;
		}


		uint32_t copy_len = new_len < *cn_len ? new_len : *cn_len;
		*cn_len = copy_len;

		//seems memcpy will use NEON register without protect
		//cause some float values in game corrupt
		//memcpy(cn_str, new_str, copy_len);

		for (int i = 0; i != copy_len; ++i)
        {
		    cn_str[i] = new_str[i];
        }

		//cn_str[copy_len] = 0;

		translated = 1;
	} while (0);
	return translated;
}