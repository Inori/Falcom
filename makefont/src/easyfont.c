#include "easyfont.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct EF_CONTEXT_S
{
    uint32_t left;
    uint32_t top;

    FT_Library    library;
    FT_Face       face;
};


int ef_init(EF_CONTEXT *ef_ctx, uint32_t left, uint32_t top, uint32_t size)
{
    return 0;
}

void ef_unit(EF_CONTEXT *ef_ctx)
{

}

int ef_get_font_glyph(EF_CONTEXT *ef_ctx, IN OUT EF_FONT_INFO *info)
{
    return 0;
}
