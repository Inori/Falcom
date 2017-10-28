#ifndef ASUKA_H
#define ASUKA_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//macros

#define IN
#define OUT
#define OPT

#define GET_VALUE(buff, index, type) (*(type*)((unsigned char*)buff + index))

#define ALIGN(x, align) (((x) + ((align) - 1)) & ~((align) - 1))


//tool functions
static inline
void* load_file(const char* filename, uint32_t* size)
{
    if (!filename)
    {
        return NULL;
    }

    FILE* fp_src = fopen(filename, "rb");
    if (fp_src == NULL)
    {
        return NULL;
    }

    uint32_t filesize = 0;

	fseek(fp_src, 0, SEEK_END);
	filesize = ftell(fp_src);
	fseek(fp_src, 0, SEEK_SET);

    void* buffer = malloc(filesize);
    if (!buffer)
    {
        fclose(fp_src);
        return NULL;
    }

	fread(buffer, filesize, 1, fp_src);
    fclose(fp_src);

    if (size)
    {
        *size = filesize;
    }
    return buffer;
}

static inline
void free_file(void* file_buff)
{
    if(!file_buff)
    {
        return;
    }
    free(file_buff);
}


static inline
int write_new_file(const char* filename, uint8_t* buffer, uint32_t len)
{
    if (!filename || !buffer)
    {
        return -1;
    }

    FILE* fdst = fopen(filename, "wb");
    if (!fdst)
    {
        return -1;
    }
    fwrite(buffer, len, 1, fdst);
    fclose(fdst);
}



#endif //ASUKA_H