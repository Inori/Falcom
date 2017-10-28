#ifndef EASY_FONT_H
#define EASY_FONT_H

#include <asuka.h>
#include <ft2build.h>
#include FT_FREETYPE_H

//
struct EF_CONTEXT_S
{
    FT_Library    library;
    FT_Face       face;
};
typedef struct EF_CONTEXT_S EF_CONTEXT;

struct EF_FONT_INFO_S
{
    wchar_t wc;
    int x;  //start x
    int y;  //start y
    int w;  //buffer width
    int h;  //buffer height
    uint8_t* buffer;
    int bold;
    int italic;
    int real_width;  //real glyph width; return
    int real_height;  //real glyph height; return
};
typedef struct EF_FONT_INFO_S EF_FONT_INFO;

//
int ef_init(EF_CONTEXT *ef_ctx, const char* fontname, uint32_t size);

void ef_unit(EF_CONTEXT *ef_ctx);

int ef_get_font_glyph(EF_CONTEXT *ef_ctx, IN OUT EF_FONT_INFO *info);

#endif //EASY_FONT_H