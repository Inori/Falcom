#include "easyfont.h"
#include FT_OUTLINE_H




int ef_init(EF_CONTEXT *ef_ctx, const char* fontname, uint32_t size)
{
    FT_Error error = -1;

    if (!ef_ctx || !fontname)
    {
        return error;
    }

    memset(ef_ctx, 0, sizeof(EF_CONTEXT));

    error = FT_Init_FreeType(&ef_ctx->library);
    if (error)
    {
        goto FAILED;
    }

    error = FT_New_Face(ef_ctx->library, fontname, 0, &ef_ctx->face);
    if (error)
    {
        goto FAILED;
    }

    error = FT_Set_Pixel_Sizes(ef_ctx->face, 0, size);
    if (error)
    {
        goto FAILED;
    }

    return error;
FAILED:
    if (ef_ctx->library)
    {
        FT_Done_FreeType(ef_ctx->library);
    }
    if (ef_ctx->face)
    {
        FT_Done_Face(ef_ctx->face);
    }

    return error;
}

void ef_unit(EF_CONTEXT *ef_ctx)
{
    if (!ef_ctx)
    {
        return;
    }

    if (ef_ctx->face)
    {
        FT_Done_Face(ef_ctx->face);
    }

    if (ef_ctx->library)
    {
        FT_Done_FreeType(ef_ctx->library);
    }

    memset(ef_ctx, 0, sizeof(EF_CONTEXT));
}

void blt_bitmap(FT_Bitmap* bitmap, uint8_t* dst_buff,
                FT_Int x, FT_Int y, FT_Int w, FT_Int h)
{

    if (!bitmap || !dst_buff)
    {
        return;
    }

    memset(dst_buff, 0, w * h);

    uint8_t* src_buff = bitmap->buffer;
    uint32_t k = 0;
    for (FT_Int i = 0; i < bitmap->rows; ++i)
    {
        for (FT_Int j = 0; j < bitmap->width; ++j)
        {
            FT_Int cur_x = x + j;
            FT_Int cur_y = y + i;
            if (cur_x < 0 || cur_y < 0 || cur_x >= w || cur_y >= h)
            {
                continue;
            }
            dst_buff[cur_y * w + cur_x] |= src_buff[k++];
        }
    }

}

int ef_get_font_glyph(EF_CONTEXT *ef_ctx, IN OUT EF_FONT_INFO *info)
{
    FT_Error error = -1;

    if (!ef_ctx || !info)
    {
        return error;
    }

    FT_UInt glyph_index = FT_Get_Char_Index(ef_ctx->face, info->wc);
    error = FT_Load_Glyph(ef_ctx->face, glyph_index, 0);
    if (error)
    {
        goto FAILED;
    }

    if(info->bold)
    {
        error = FT_Outline_Embolden(&ef_ctx->face->glyph->outline, (FT_Pos)(info->bold));
        if (error)
        {
            goto FAILED;
        }
    }

    if(info->italic)
    {
        double lean = 1.0f * info->italic / 100;
        FT_Matrix matrix;
        matrix.xx = (FT_Fixed)(1		* 0x10000L);
        matrix.xy = (FT_Fixed)(lean		* 0x10000L);
        matrix.yx = (FT_Fixed)(0		* 0x10000L);
        matrix.yy = (FT_Fixed)(1		* 0x10000L);
        FT_Set_Transform(ef_ctx->face, &matrix, 0 );
    }

    if(ef_ctx->face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
	{
        error = FT_Render_Glyph(ef_ctx->face->glyph, FT_RENDER_MODE_NORMAL);
        if (error)
        {
            goto FAILED;
        }
	}

    FT_GlyphSlot slot = ef_ctx->face->glyph;
    FT_Bitmap* bitmap = &slot->bitmap;

    FT_Int x = info->x + slot->bitmap_left;
    FT_Int y = info->y + info->h - slot->bitmap_top;

    blt_bitmap(bitmap, info->buffer, x, y, info->w, info->h);

    info->real_width = x + bitmap->width;
    info->real_height = y + bitmap->rows;
FAILED:
    return error;
}
