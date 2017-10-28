//
// Created by asuka on 10/29/17.
//
#include "codeconvert.h"
#include <iconv.h>


static iconv_t g_u16_to_u8_ic = (iconv_t)(-1);
static iconv_t g_u8_to_u16_ic = (iconv_t)(-1);



int cc_init()
{
    g_u16_to_u8_ic = iconv_open("UTF-8", "UTF-16LE");
    if (g_u16_to_u8_ic == (iconv_t)-1)
    {
        return -1;
    }

    g_u8_to_u16_ic = iconv_open("UTF-16", "UTF-8");
    if (g_u8_to_u16_ic == (iconv_t)-1)
    {
        return -1;
    }
    return 0;
}


int cc_utf16_to_utf8(uint16_t* u16_buff, uint32_t u16_size,
                     uint8_t* u8_buff, uint32_t u8_size)
{
    uint8_t * in_buff = u16_buff;
    size_t in_size = u16_size;
    uint8_t * out_buff = u8_buff;
    size_t out_size = u8_size;

    size_t ret = iconv(g_u16_to_u8_ic, &in_buff, &in_size, &out_buff, &out_size);
    if (ret == -1)
    {
        return 0;
    }
    return (u8_size - out_size);
}


int cc_utf8_to_utf16(uint8_t* u8_buff, uint32_t u8_size,
                     uint16_t* u16_buff, uint32_t u16_size)
{
    uint8_t * in_buff = u8_buff;
    size_t in_size = u8_size;
    uint8_t * out_buff = u16_buff;
    size_t out_size = u16_size;

    size_t ret = iconv(g_u8_to_u16_ic, &in_buff, &in_size, &out_buff, &out_size);
    if (ret == -1)
    {
        return 0;
    }
    return (u16_size - out_size);
}


void cc_unit()
{
    if (g_u16_to_u8_ic != (iconv_t)-1)
    {
        iconv_close(g_u16_to_u8_ic);
    }

    if (g_u8_to_u16_ic != (iconv_t)-1)
    {
        iconv_close(g_u8_to_u16_ic);
    }
}