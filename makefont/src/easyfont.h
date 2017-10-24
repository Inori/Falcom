#ifndef EASY_FONT_H
#define EASY_FONT_H

#include <asuka.h>
#include "makefont.h"

//
struct EF_CONTEXT_S;
typedef struct EF_CONTEXT EF_CONTEXT;

struct EF_FONT_INFO_S
{
    wchar_t wc;
    uint32_t width;
    uint32_t height;
    uint8_t *buffer;
};
typedef struct EF_FONT_INFO_S EF_FONT_INFO;

//
int ef_init(EF_CONTEXT *ef_ctx, uint32_t left, uint32_t top, uint32_t size);

void ef_unit(EF_CONTEXT *ef_ctx);

int ef_get_font_glyph(EF_CONTEXT *ef_ctx, IN OUT EF_FONT_INFO *info);

#endif //EASY_FONT_H