/*
* This program extract font graph data in vitafont2bit.fnt file
* into a png file.
* 
* Support:
* Eiyuu Densetsu: Ao no Kiseki - Evolution
* Eiyuu Densetsu: Zero no Kiseki - Evolution (change the "#if 0" and do some modification, I'm lazy:) )
*
* Author: Asuka(Inori)
* Date: 2017/10/22
*/



#include <asuka.h>
#include "pnglibconf.h"
#include "png.h"

#if 0

//For vitafont.dat in ED ZERO
#define OFF_NUMBER	0x00C0
#define GET_VALUE(PTR, OFF, VAR) (*((decltype(VAR)*)(((unsigned char*)PTR) + OFF)));
#define SIZE_ONE_CHA	0x20
#define NUM_ONE_ROW		32
#define BITS_ONE_BYTE	8

bool font2png(const std::string& fontFileName, const std::string& pngName)
{
	std::string font_name = "C:\\Users\\Asuka\\Desktop\\FalcomToolchain\\font\\vitafont.dat";

	FILE* fpFont = fopen(font_name.c_str(), "rb");
	if (fpFont == NULL)
		return false;

	fseek(fpFont, 0, SEEK_END);
	u32 fontFileLength = ftell(fpFont);
	fseek(fpFont, 0, SEEK_SET);

	u8* buffFont = new u8[fontFileLength];
	fread(buffFont, sizeof(*buffFont), fontFileLength, fpFont);
	fclose(fpFont);

	u32 index = OFF_NUMBER;
	u32 num = GET_VALUE(buffFont, index, num); index += sizeof(num);

	u32 width = SIZE_ONE_CHA * NUM_ONE_ROW;
	u32 numOfRows = (num + NUM_ONE_ROW - 1) / NUM_ONE_ROW;
	u32 height = SIZE_ONE_CHA * numOfRows;

	u8* buffData = new u8[width * height];
	memset(buffData, 0, width * height);

	for (u32 i = 0; i < numOfRows; ++i)
	{
		unsigned y0 = i * SIZE_ONE_CHA;
		for (u32 j = 0; j < NUM_ONE_ROW; ++j)
		{
			if (index > OFF_NUMBER + num * 4)
				break;

			u32 tmp = GET_VALUE(buffFont, index, tmp); index += sizeof(tmp);
			u32 off = (tmp & 0x00FFFFFF) * 4 +
				OFF_NUMBER + 4 + 
				num * 4 + 4; 
	
			u32 size = tmp >> 24;

			u32 x0 = j * SIZE_ONE_CHA;

			for (u32 x = x0; x < x0 + size; x += 1)
			{
				for (u32 y = y0; y < y0 + SIZE_ONE_CHA; y += BITS_ONE_BYTE)
				{
					buffData[(y)* width + x] = (buffFont[off]) & 0x01;
					buffData[(y + 1) * width + x] = (buffFont[off] >> 1) & 0x01;
					buffData[(y + 2) * width + x] = (buffFont[off] >> 2) & 0x01;
					buffData[(y + 3) * width + x] = (buffFont[off] >> 3) & 0x01;
					buffData[(y + 4) * width + x] = (buffFont[off] >> 4) & 0x01;
					buffData[(y + 5) * width + x] = (buffFont[off] >> 5) & 0x01;
					buffData[(y + 6) * width + x] = (buffFont[off] >> 6) & 0x01;
					buffData[(y + 7) * width + x] = (buffFont[off] >> 7) & 0x01;

					++off;
				}
			}

			size = off;

		}
	}
	delete[] buffFont;

	bool rst = WritePNG(pngName.c_str(), width, height, buffData);

	delete[] buffData;

	return rst;
}

#endif

#define INDEX_OFFSET    0x0100
#define FONT_SIZE   0x20  //font png_width = font png_height = font size
#define FONT_HALF_SIZE (FONT_SIZE/2)
#define NUM_ONE_ROW     32
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
		trns_alpha[i] = (png_byte)(i * 0x55);
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


int font2png(const char* font_filename, const char* png_filename)
{
    if (!font_filename || !png_filename)
    {
        return -1;
    }

    uint32_t font_filesize = 0;
    uint8_t* font_buff = load_file(font_filename, &font_filesize);
    if (!font_buff)
    {
        return -1;
    }

    //

	uint32_t index = INDEX_OFFSET;
	uint32_t char_num = GET_VALUE(font_buff, index, uint32_t); 
	index += sizeof(char_num);

	uint32_t png_width = FONT_SIZE * NUM_ONE_ROW;
	uint32_t png_height = FONT_SIZE * (ALIGN(char_num, NUM_ONE_ROW) / NUM_ONE_ROW);

	uint8_t* png_buff = (uint8_t*)malloc(png_width * png_height);
    memset(png_buff, 0, png_width * png_height);
    
    for (uint32_t i = 0; i != char_num; ++i)
    {
        uint32_t x0 = (i % NUM_ONE_ROW) * FONT_SIZE;
        uint32_t y0 = (i / NUM_ONE_ROW) * FONT_SIZE;

        uint32_t val = GET_VALUE(font_buff, index, uint32_t);
        index += sizeof(val);

        //font graph data start
        uint32_t font_start = INDEX_OFFSET + sizeof(uint32_t) +
                              char_num * sizeof(uint32_t) +
                              4;
        uint32_t offset = font_start + (val & 0x00FFFFFF);
        uint32_t char_width = val >> 24;

        for (uint32_t x = x0; x < x0 + char_width; x += 1)
        {
            for (uint32_t y = y0; y < y0 + FONT_SIZE; y += FONT_HALF_SIZE)
            {
                uint32_t reverse_y = y + FONT_HALF_SIZE;
                for (uint32_t k = 0; k != FONT_HALF_SIZE; ++k)
                {
                    if (k == FONT_HALF_SIZE / 2)
                    {
                        offset += 2;
                    }
                    uint16_t cur_short = *(uint16_t*)&font_buff[offset];
                    uint32_t shift = 2 * (k % (FONT_HALF_SIZE / 2));
                    png_buff[(reverse_y - k - 1)  * png_width + x] = (cur_short >> shift) & 0x03;
                }
                offset += 2;
            }
        }
    }

    free_file(font_buff);

	int ret = write_png_file(png_filename, png_width, png_height, png_buff);

	free(png_buff);

	return ret;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("usage: %s <vitafont2bit.fnt>\n", argv[0]);
        return -1;
    }

    char* font_filename = argv[1];
    char png_filename[256] = {0};
    strcpy(png_filename, font_filename);
    strcat(png_filename, ".png");

    int ret = font2png(font_filename, png_filename);

    return ret;
}