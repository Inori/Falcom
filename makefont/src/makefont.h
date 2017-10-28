#ifndef MAKE_FONT_H
#define MAKE_FONT_H

#define FONT_SIZE 0x20
#define FONT_HALF_SIZE (FONT_SIZE/2)
#define RANGE_NUM 4
#define RANGE_BLOCK_SIZE 0x40

#define BITS_EACH_PIXEL 2  //vitafont2bi.fnt use 2 bits to represent 1 pixel
                           //which means 4 gray levels per pixle
                           //compared to 256 gray levels of FreeType font
                        
#define FONT_SIZE_FATOR (FONT_SIZE * BITS_EACH_PIXEL / 8)

#define INVILID_CHAR_INDEX 0xFFFFFFFF


#pragma pack(1)


typedef struct FONT_HEADER_S
{
    uint8_t magic[4]; //VFNT
    uint32_t version; // not sure
    uint32_t bit_width;
    uint32_t glyph_data_size; //header + range * 3 + index + glyph_data_size = filesize
    uint32_t shift_data_size; // shift_data_size = glyph_data_size >> 3
    uint32_t unknown;
    uint32_t pad[10];
} FONT_HEADER;

typedef struct FONT_RANGE_S
{
    uint32_t begin;
    uint32_t end;
    uint32_t plus;
} FONT_RANGE;

typedef struct FONT_INDEX_S
{
    uint32_t count;
    uint32_t val[1];
} FONT_INDEX;


#pragma pack()

typedef struct CHAR_INFO_S
{
    uint32_t offset;
    uint32_t width;
    uint8_t buffer[FONT_SIZE * FONT_SIZE / 4];
} CHAR_INFO;

typedef struct CODE_PAIR_S
{
    uint16_t code;
    wchar_t wch;
} CODE_PAIR;

#endif //MAKE_FONT_H