#include <asuka.h>
#include <kvec.h>
#include <wchar.h>
#include <locale.h>
#include <iconv.h>
#include "makefont.h"
#include "easyfont.h"

#define CODE_PAIR_VEC kvec_t(CODE_PAIR)


int read_font(const char *font_name,
              FONT_HEADER *header, FONT_RANGE *ranges,
              CHAR_INFO **char_infos, uint32_t *char_count)
{
    if (!font_name)
    {
        return -1;
    }

    uint8_t *buffer = load_file(font_name, NULL);
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

    if (buffer)
    {
        free_file(buffer);
    }

    return 0;
}


int write_font(const char* font_name,
               FONT_HEADER* header, FONT_RANGE* ranges,
               CHAR_INFO* char_infos, uint32_t char_count)
{
    if (!font_name)
    {
        return -1;
    }

    uint32_t index_start = sizeof(FONT_HEADER) + RANGE_BLOCK_SIZE * 3;
    uint32_t glyph_data_start = index_start +
                                sizeof(uint32_t) + sizeof(uint32_t) * char_count +
                                sizeof(uint32_t);

    uint32_t glyph_data_size = 0;

    FILE* fdst = fopen(font_name, "wb");
    if (!fdst)
    {
        return -1;
    }

    //
    fseek(fdst, glyph_data_start, SEEK_SET);
    for (uint32_t i = 0; i != char_count; ++i)
    {
        uint32_t size = char_infos[i].width * FONT_SIZE_FATOR;
        uint32_t offset = ftell(fdst) - glyph_data_start;
        char_infos[i].offset = offset;
        glyph_data_size += size;

        fwrite(char_infos[i].buffer, size, 1, fdst);
    }

    //
    header->glyph_data_size = glyph_data_size;
    header->shift_data_size = header->glyph_data_size >> 3;

    fseek(fdst, 0, SEEK_SET);
    fwrite(header, sizeof(FONT_HEADER), 1, fdst);

    //
    for (uint32_t j = 0; j != RANGE_NUM; ++j)
    {
        fseek(fdst, sizeof(FONT_HEADER) + j * sizeof(uint32_t), SEEK_SET);
        fwrite(&ranges[j].begin, sizeof(uint32_t), 1, fdst);

        fseek(fdst, RANGE_BLOCK_SIZE - sizeof(uint32_t), SEEK_CUR);
        fwrite(&ranges[j].end, sizeof(uint32_t), 1, fdst);

        fseek(fdst, RANGE_BLOCK_SIZE - sizeof(uint32_t), SEEK_CUR);
        fwrite(&ranges[j].plus, sizeof(uint32_t), 1, fdst);
    }

    //
    fseek(fdst, index_start, SEEK_SET);
    fwrite(&char_count, sizeof(uint32_t), 1, fdst);
    for (uint32_t k = 0; k != char_count; ++k)
    {
        uint32_t val = (char_infos[k].width << 24) | char_infos[k].offset;
        fwrite(&val, sizeof(uint32_t), 1, fdst);
    }
    fwrite(&glyph_data_size, sizeof(uint32_t), 1, fdst);


    if (fdst)
    {
        fclose(fdst);
    }
    return 0;
}


int read_code_table(const char* table_name, CODE_PAIR_VEC* table)
{
    if (!table_name)
    {
        return 0;
    }

    uint32_t file_size = 0;
    uint8_t * file_buffer = load_file(table_name, &file_size);
    if (!file_buffer)
    {
        return 0;
    }

    iconv_t ic = iconv_open("UTF-8", "WCHAR_T");

    uint8_t *utf16_buffer = &file_buffer[2];
    uint32_t utf16_size = file_size - 2;
    uint32_t utf8_size = utf16_size / 2 * 5;
    uint8_t * utf8_buffer = (uint8_t*)malloc(utf8_size);

    size_t ret = iconv(ic, &utf16_buffer, &utf16_size, &utf8_buffer, &utf8_size);

    printf("%s", utf8_buffer);

    uint32_t code = 0;
    wchar_t wch = 0;
    int scaned = sscanf(utf8_buffer, "%02X=%c", &code, &wch);



    free_file(file_buffer);
    iconv_close(ic);
    free(utf8_buffer);

    return kv_size(*table);
}


int make_font(const char* fn_code_table, const char* fn_old_font, const char* fn_new_font)
{
    if (!fn_code_table || !fn_old_font || !fn_new_font)
    {
        return -1;
    }

    CODE_PAIR_VEC vt_code_table;
    kv_init(vt_code_table);
    //
    if (!read_code_table(fn_code_table, &vt_code_table))
    {
        goto FUNC_EXIT;
    }

    //
    FONT_HEADER header = {0};
    FONT_RANGE font_ranges[RANGE_NUM] = {0};
    CHAR_INFO* char_infos = NULL;
    uint32_t char_count = 0;

    read_font(fn_old_font,
              &header, font_ranges,
              &char_infos, &char_count);

    //


    //
    write_font(fn_new_font,
               &header, font_ranges,
               char_infos, char_count);


FUNC_EXIT:
    if (char_infos)
    {
        free(char_infos);
    }

    kv_destroy(vt_code_table);
    return 0;
}


int main(int argc, char** argv)
{

    if ( argc != 4 )
    {
        fprintf ( stderr, "usage: %s code-table.txt old-font.fnt new-font.fnt\n", argv[0] );
        exit(1);
    }

    //setlocale(LC_ALL, "");
    //setlocale(LC_CTYPE, "en_US.utf8");

    char* fn_code_table = argv[1];
    char* fn_old_font = argv[2];
    char* fn_new_font = argv[3];


    int ret = make_font(fn_code_table, fn_old_font, fn_new_font);

    return ret;
}