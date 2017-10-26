#include <asuka.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "makefont.h"
#include "easyfont.h"


int get_old_font_info(uint8_t* buffer, 
    FONT_HEADER* header, FONT_RANGE* ranges, 
    CHAR_INFO** char_infos, uint32_t* char_count)
{
    if (!buffer)
    {
        return -1;
    }

    uint8_t* cur_p = buffer;
    //header
    memcpy(header, cur_p, sizeof(FONT_HEADER));
    cur_p += sizeof(FONT_HEADER);

    //range
    for (uint32_t i = 0; i != RANGE_NUM; ++i)
    {
        ranges[i].begin = GET_VALUE(cur_p, i * sizeof(uint32_t), uint32_t);
        ranges[i].end = GET_VALUE(cur_p, i * sizeof(uint32_t) + RANGE_BLOCK_SIZE, uint32_t);
        ranges[i].plus = GET_VALUE(cur_p, i * sizeof(uint32_t) + RANGE_BLOCK_SIZE * 2, uint32_t);
    }
    cur_p += (RANGE_BLOCK_SIZE * 3);

    //index
    FONT_INDEX* entries = (FONT_INDEX*)cur_p;
    uint32_t count = entries->count;
    CHAR_INFO* infos = (CHAR_INFO*)malloc(count * sizeof(CHAR_INFO));

    uint32_t glyph_data_start = sizeof(FONT_HEADER) + RANGE_BLOCK_SIZE * 3 +
                                sizeof(uint32_t) + sizeof(uint32_t) * count +
                                sizeof(uint32_t);
    cur_p = buffer + glyph_data_start;

    //font buffer
    for (uint32_t j = 0; j != count; ++j)
    {
        uint32_t val = entries->val[j];
        uint32_t offset = val & 0x00FFFFFF;
        uint32_t width = val >> 24;
        
        infos[j].offset = offset;
        infos[j].width = width;
        memcpy(infos[j].buffer, cur_p + offset, FONT_SIZE_FATOR * width);
    }

    *char_infos = infos;
    *char_count = count;

    return 0;
}


int make_font(const char* fn_code_table, const char* fn_old_font, const char* fn_new_font)
{
    if (!fn_code_table || !fn_old_font || !fn_new_font)
    {
        return -1;
    }


    //load code table

    //
    uint32_t old_size = 0;
    uint8_t* old_font_buff = (uint8_t*)load_file(fn_old_font, &old_size);
    if (!old_font_buff)
    {
        goto FAILED;
    }

    FONT_HEADER header = {0};
    FONT_RANGE font_ranges[RANGE_NUM] = {0};
    CHAR_INFO* char_infos = NULL;
    uint32_t char_count = 0;

    get_old_font_info(old_font_buff,
                      &header, font_ranges,
                      &char_infos, &char_count);



    return 0;
FAILED:
    if (old_font_buff)
    {
        free_file(old_font_buff);
    }
    if (char_infos)
    {
        free(char_infos);
    }

    return -1;
}


int main(int argc, char** argv)
{
    if ( argc != 4 )
    {
        fprintf ( stderr, "usage: %s code-table.txt old-font.fnt new-font.fnt\n", argv[0] );
        exit(1);
    }

    char* fn_code_table = argv[1];
    char* fn_old_font = argv[2];
    char* fn_new_font = argv[3];


    int ret = make_font(fn_code_table, fn_old_font, fn_new_font);

    return ret;
}