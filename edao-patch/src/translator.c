#include "translator.h"
#include "log.h"


int tl_init(TL_CONTEX* ctx, const char* fn_map)
{
	return 0;
}

void tl_unit(TL_CONTEX* ctx)
{

}


char* test_string = "\x8F\x71\x87\x66\x8F\xB0\x94\xF8";
char test_buffer[1024] = {0};
int tl_translate(TL_CONTEX* ctx, const char* jp_str, uint32_t jp_len, 
				 char* cn_str, uint32_t* cn_len)
{
	// strncpy(test_buffer, jp_str, jp_len);
	// test_buffer[jp_len] = 0;
	// DEBUG_PRINT("%s\n", test_buffer);

	strcpy(cn_str, test_string);
	*cn_len = strlen(test_string);
	return 1;
}