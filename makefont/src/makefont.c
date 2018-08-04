#include <asuka.h>
#include <kvec.h>
#include <wchar.h>
#include <locale.h>
#include <png.h>
#include "makefont.h"
#include "easyfont.h"
#include "codeconvert.h"

#define CODE_PAIR_VEC kvec_t(CODE_PAIR)

static int g_font_size = FONT_SIZE;
static int g_font_left = 0;
static int g_font_top = 0;
static int g_font_bold = 0;

#define COLORS_IN_PALETTE	256

int write_png_file( const char* png_name, uint32_t png_width, uint32_t png_height, const uint8_t* buffer)
{
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp palette;
    png_bytep trns_alpha;
    uint32_t i;


    fp = fopen(png_name, "wb");
    if (fp == NULL)
        return -1;


    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                      NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(fp);
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  NULL);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -1;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr, info_ptr, png_width, png_height, 8, PNG_COLOR_TYPE_PALETTE,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH
                                              * sizeof(png_color));
    trns_alpha = (png_bytep)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH
                                                * sizeof(png_byte));
    for(i = 0; i < COLORS_IN_PALETTE; ++i)
    {
        palette[i].red = 0xFF;
        palette[i].green = 0;
        palette[i].blue = 0;
        trns_alpha[i] = i; //(png_byte)(i * 0x55);
    }

    png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
    png_set_tRNS(png_ptr, info_ptr, trns_alpha, PNG_MAX_PALETTE_LENGTH, NULL);

    png_write_info(png_ptr, info_ptr);

    const uint8_t* ptrRow = buffer;
    for(png_uint_32 y = 0;y < png_height; ++y)
    {
        png_write_row(png_ptr, ptrRow);
        ptrRow += png_width;
    }


    png_free(png_ptr, trns_alpha);
    png_free(png_ptr, palette);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    return 0;
}


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

#define TEMP_CODE_TABLE_FN "./temp_ct.txt"

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

    uint32_t utf8_size = wcslen(file_buffer) * 4;
    uint8_t * utf8_buffer = (uint8_t*)malloc(utf8_size);
    memset(utf8_buffer, 0, utf8_size);

    //as fuck linux do not support process UTF16 directly, we must take a detour
    utf8_size = cc_utf16_to_utf8(&file_buffer[2], file_size-2, utf8_buffer, utf8_size);
    write_new_file(TEMP_CODE_TABLE_FN, utf8_buffer, utf8_size);

    setlocale(LC_CTYPE, "en_US.utf8");  //iconv will clear locale at open, we have to set it back
    FILE* f_ct = fopen(TEMP_CODE_TABLE_FN, "r");
    if (!f_ct)
    {
        goto FUNC_EXIT;
    }

    uint32_t code = 0;
    wchar_t wch = 0;
    int ret = 0;
    while(1)
    {
        ret = fscanf(f_ct, "%04X=%lc\r\n", &code, &wch);
        if (ret == EOF)
        {
            break;
        }

        //printf("%04X=%lc\n", code, wch);

        CODE_PAIR cinfo = {code, wch};
        kv_push(CODE_PAIR, *table, cinfo);
    }


FUNC_EXIT:
    if (f_ct)
    {
        fclose(f_ct);
    }
    remove(TEMP_CODE_TABLE_FN);
    if (file_buffer)
    {
        free_file(file_buffer);
    }
    if (utf8_buffer)
    {
        free(utf8_buffer);
    }

    return kv_size(*table);
}


uint32_t get_char_index(uint16_t code, FONT_RANGE* ranges)
{
    for (uint32_t i = 0; i != RANGE_NUM; ++i)
    {
        if(code >= ranges[i].begin && code <= ranges[i].end)
        {
            return code - ranges[i].begin + ranges[i].plus;
        }
    }

    return INVILID_CHAR_INDEX;
}


int update_char_info(CHAR_INFO* char_info, EF_CONTEXT* ef_ctx, wchar_t ucs)
{
    static uint8_t glyph_slot[FONT_SIZE * FONT_SIZE] = {0};
    memset(glyph_slot, 0, FONT_SIZE * FONT_SIZE);


    EF_FONT_INFO glyph_info = {
            ucs,
            g_font_left,
            g_font_top,
            FONT_SIZE,
            FONT_SIZE,
            glyph_slot,
            g_font_bold,
            0,0,0
    };

    if (ef_get_font_glyph(ef_ctx, &glyph_info) != 0)
    {
        return -1;
    }

    //uint32_t width = glyph_info.w;
    uint32_t width = glyph_info.real_width;
    char_info->width = width;

    uint8_t *vt_buff = char_info->buffer;  //ViTa
    memset(vt_buff, 0, sizeof(char_info->buffer));

    uint8_t *ft_buff = glyph_info.buffer;  //FreeType

    //Debug
    //write_png_file("test.png", 32,32, glyph_slot);

    uint32_t offset = 0;
    for (uint32_t x = 0; x != width; ++x)
    {
        for (uint32_t y = 0; y != FONT_SIZE; y += FONT_HALF_SIZE)
        {
            uint32_t reverse_y = y + FONT_HALF_SIZE;
            uint16_t cur_short = 0;
            for (uint32_t k = 0; k != FONT_HALF_SIZE; ++k)
            {
                if (k == FONT_HALF_SIZE / 2)
                {
                    *(uint16_t*)&vt_buff[offset] = cur_short;
                    offset += 2;
                    cur_short = 0;
                }
                uint8_t ft_pixel = ft_buff[(reverse_y - k - 1) * FONT_SIZE + x];
                uint8_t vt_pixel = (uint8_t) (ft_pixel / 0x40);  //FreeType generate 256 gray levels image, we need to convert it to 4 gray levels
                uint32_t shift = 2 * (k % (FONT_HALF_SIZE / 2));
                cur_short |= ((vt_pixel & 3) << shift);
            }
            *(uint16_t*)&vt_buff[offset] = cur_short;
            offset += 2;
        }
    }

    return 0;
}


int update_font(CHAR_INFO* char_infos, uint32_t char_count,
                CODE_PAIR_VEC* code_table, FONT_RANGE* ranges,
                const char* fn_ttf)
{
    EF_CONTEXT ef_ctx = {0};
    if (ef_init(&ef_ctx, fn_ttf, g_font_size) != 0)
    {
        return -1;
    }

    uint32_t new_count = kv_size(*code_table);

    for (uint32_t i = 0; i != new_count; ++i)
    {
        CODE_PAIR cur_pair = kv_A(*code_table, i);
        uint32_t index = get_char_index(cur_pair.code, ranges);
        if (index == INVILID_CHAR_INDEX)
        {
            printf("get_char_index failed: 0x%04X\n", cur_pair.code);
            continue;
        }

        CHAR_INFO* cur_info = &char_infos[index];
        if (update_char_info(cur_info, &ef_ctx, cur_pair.wch) != 0)
        {
            printf("update_char_info failed: 0x%04X\n", cur_pair.code);
            continue;
        }
    }

    ef_unit(&ef_ctx);
}


int make_font(const char* fn_code_table, const char* fn_old_font, const char* fn_new_font, const char* fn_ttf)
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
    update_font(char_infos, char_count,
                &vt_code_table, font_ranges, fn_ttf);


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

    if ( argc != 9 )
    {
        fprintf ( stderr, "usage: %s code-table.txt old-font.fnt new-font.fnt font.ttf size left top bold\n", argv[0] );
        exit(1);
    }

    cc_init();

    char* fn_code_table = argv[1];
    char* fn_old_font = argv[2];
    char* fn_new_font = argv[3];
    char* fn_ttf_name = argv[4];
    g_font_size = atoi(argv[5]);
    g_font_left = atoi(argv[6]);
    g_font_top = atoi(argv[7]);
    g_font_bold = atoi(argv[8]);


    int ret = make_font(fn_code_table, fn_old_font, fn_new_font, fn_ttf_name);
    if (!ret)
    {
        printf("make font success: %s\n", fn_new_font);
    }
    else
    {
        printf("make font failed.\n");
    }

    cc_unit();

    return ret;
}