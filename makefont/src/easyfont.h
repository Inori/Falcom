#ifndef EASY_FONT_H
#define EASY_FONT_H

#include <asuka.h>

//
struct EF_CONTEXT_S;
typedef struct EF_CONTEXT_S EF_CONTEXT;

struct EF_FONT_INFO_S
{
    wchar_t wc;
    uint32_t x;  //start x
    uint32_t y;  //start y
    uint32_t w;  //buffer width
    uint32_t h;  //buffer height
    uint8_t* buffer;
};
typedef struct EF_FONT_INFO_S EF_FONT_INFO;

//
int ef_init(EF_CONTEXT *ef_ctx, const char* fontname, uint32_t size);

void ef_unit(EF_CONTEXT *ef_ctx);

int ef_get_font_glyph(EF_CONTEXT *ef_ctx, IN OUT EF_FONT_INFO *info);

#endif //EASY_FONT_H