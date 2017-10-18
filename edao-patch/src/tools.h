#ifndef VITA_TOOLS_H
#define VITA_TOOLS_H


#define VITA_TITLE_ID_LEN 12

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif

#define ALIGN(x, align) (((x) + ((align) - 1)) & ~((align) - 1))


int get_title_id(char title_id[VITA_TITLE_ID_LEN]);

void hex_to_string(unsigned char* data, int data_len, char* string, int str_len);

void dump_mem(char* tag, unsigned char* data, int len);

void* vita_malloc(unsigned int size);

void vita_free(void* mem);


//string tools


char* rip_string(char* old_str);

#endif