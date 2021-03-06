#include "SkBitmapProcState.h"
#include "SkPerspIter.h"
#include "SkShader.h"

void decal_nofilter_scale(uint32_t dst[], SkFixed fx, SkFixed dx, int count);
void decal_filter_scale(uint32_t dst[], SkFixed fx, SkFixed dx, int count);

#ifdef SK_CPU_BENDIAN
    #define PACK_TWO_SHORTS(pri, sec) ((pri) << 16 | (sec))
#else
    #define PACK_TWO_SHORTS(pri, sec) ((pri) | ((sec) << 16))
#endif

#ifdef SK_DEBUG
    static uint32_t pack_two_shorts(U16CPU pri, U16CPU sec)
    {
        SkASSERT((uint16_t)pri == pri);
        SkASSERT((uint16_t)sec == sec);
        return PACK_TWO_SHORTS(pri, sec);
    }
#else
    #define pack_two_shorts(pri, sec)   PACK_TWO_SHORTS(pri, sec)
#endif

#define MAKENAME(suffix)        ClampX_ClampY ## suffix
#define TILEX_PROCF(fx, max)    SkClampMax((fx) >> 16, max)
#define TILEY_PROCF(fy, max)    SkClampMax((fy) >> 16, max)
#define TILEX_LOW_BITS(fx, max) (((fx) >> 12) & 0xF)
#define TILEY_LOW_BITS(fy, max) (((fy) >> 12) & 0xF)
#define CHECK_FOR_DECAL
#define TILEX_TRANS(x, max)     SkClampMax(x, max)
#define TILEY_TRANS(y, max)     SkClampMax(y, max)
#include "SkBitmapProcState_matrix.h"

#define MAKENAME(suffix)        RepeatX_RepeatY ## suffix
#define TILEX_PROCF(fx, max)    (((fx) & 0xFFFF) * ((max) + 1) >> 16)
#define TILEY_PROCF(fy, max)    (((fy) & 0xFFFF) * ((max) + 1) >> 16)
#define TILEX_LOW_BITS(fx, max) ((((fx) & 0xFFFF) * ((max) + 1) >> 12) & 0xF)
#define TILEY_LOW_BITS(fy, max) ((((fy) & 0xFFFF) * ((max) + 1) >> 12) & 0xF)
#define REAL_MOD(val, modulus)  (((val)%(modulus)) + (modulus)*( (val)<0 ))
#define TILEX_TRANS(x, max)     (REAL_MOD((x), ((max) + 1)))
#define TILEY_TRANS(y, max)     (REAL_MOD((y), ((max) + 1)))
#include "SkBitmapProcState_matrix.h"

#define MAKENAME(suffix)        GeneralXY ## suffix
#define PREAMBLE(state)         SkBitmapProcState::FixedTileProc tileProcX = (state).fTileProcX; \
                                SkBitmapProcState::FixedTileProc tileProcY = (state).fTileProcY
#define PREAMBLE_PARAM_X        , SkBitmapProcState::FixedTileProc tileProcX
#define PREAMBLE_PARAM_Y        , SkBitmapProcState::FixedTileProc tileProcY
#define PREAMBLE_ARG_X          , tileProcX
#define PREAMBLE_ARG_Y          , tileProcY
#define TILEX_PROCF(fx, max)    (tileProcX(fx, max) >> 16)
#define TILEY_PROCF(fy, max)    (tileProcY(fy, max) >> 16)
#define TILEX_LOW_BITS(fx, max) ((tileProcX(fx, max) >> 14) & 0x3)
#define TILEY_LOW_BITS(fy, max) ((tileProcY(fy, max) >> 14) & 0x3)
#define PREAMBLE_TRANS(state)   SkBitmapProcState::IntTileProc tileProcX = (state).iTileProcX; \
                                SkBitmapProcState::IntTileProc tileProcY = (state).iTileProcY
#define TILEX_TRANS(x, max)     tileProcX(x, max)
#define TILEY_TRANS(y, max)     tileProcY(y, max)
#include "SkBitmapProcState_matrix.h"

static inline SkFixed fixed_clamp(SkFixed x, int max)
{
#ifdef SK_CPU_HAS_CONDITIONAL_INSTR
    if (x >> 16)
        x = 0xFFFF;
    if (x < 0)
        x = 0;
#else
    if (x >> 16)
    {
        if (x < 0)
            x = 0;
        else
            x = 0xFFFF;
    }
#endif
    return x * (max + 1);
}

static inline SkFixed fixed_repeat(SkFixed x, int max)
{
    return (x & 0xFFFF) * (max + 1);
}

static inline SkFixed fixed_mirror(SkFixed x, int max)
{
    SkFixed s = x << 15 >> 31;
    // s is FFFFFFFF if we're on an odd interval, or 0 if an even interval
    x = ((x ^ s) & 0xFFFF) * (max + 1);
    return s ? (x ^ 0xFFFF) : x;
}

static SkBitmapProcState::FixedTileProc choose_tile_proc(unsigned m)
{
    if (SkShader::kClamp_TileMode == m)
        return fixed_clamp;
    if (SkShader::kRepeat_TileMode == m)
        return fixed_repeat;
    SkASSERT(SkShader::kMirror_TileMode == m);
    return fixed_mirror;
}
 
static inline int int_clamp(int x, int max)
{
    SkASSERT(max >= 0);
    
    return SkClampMax(x, max);
}

static inline int int_repeat(int x, int max)
{
    SkASSERT(max >= 0);

    return x % (max + 1);
}

static inline int int_mirror(int x, int max)
{
    SkASSERT(max >= 0);

    int dx = x % (max + 1);
    if (dx < 0)
        dx = -dx - 1;
    
    return (x / (max + 1) % 2) ? max - dx : dx;
}

static SkBitmapProcState::IntTileProc choose_int_tile_proc(unsigned m)
{
    if (SkShader::kClamp_TileMode == m)
        return int_clamp;
    if (SkShader::kRepeat_TileMode == m)
        return int_repeat;
    SkASSERT(SkShader::kMirror_TileMode == m);
    return int_mirror;
}

SkBitmapProcState::MatrixProc SkBitmapProcState::chooseMatrixProc()
{
    int index = 0;
    if (fDoFilter)
        index = 1;
    if (fInvType & SkMatrix::kPerspective_Mask)
        index |= 6;
    else if (fInvType & SkMatrix::kAffine_Mask)
        index |= 4;
    else if (fInvType & SkMatrix::kScale_Mask)
        index |= 2;

    if (SkShader::kClamp_TileMode == fTileModeX &&
        SkShader::kClamp_TileMode == fTileModeY)
    {
        // clamp gets special version of filterOne
        fFilterOneX = SK_Fixed1;
        fFilterOneY = SK_Fixed1;
        return ClampX_ClampY_Procs[index];
    }
    
    // all remaining procs use this form for filterOne
    fFilterOneX = SK_Fixed1 / fBitmap->width();
    fFilterOneY = SK_Fixed1 / fBitmap->height();

    if (SkShader::kRepeat_TileMode == fTileModeX &&
        SkShader::kRepeat_TileMode == fTileModeY)
    {
        return RepeatX_RepeatY_Procs[index];
    }

    // only general needs these procs
    fTileProcX = choose_tile_proc(fTileModeX);
    fTileProcY = choose_tile_proc(fTileModeY);
    iTileProcX = choose_int_tile_proc(fTileModeX);
    iTileProcY = choose_int_tile_proc(fTileModeY);
    return GeneralXY_Procs[index];
}

//////////////////////////////////////////////////////////////////////////////

void decal_nofilter_scale(uint32_t dst[], SkFixed fx, SkFixed dx, int count)
{
    int i;

    for (i = (count >> 2); i > 0; --i)
    {
        *dst++ = pack_two_shorts(fx >> 16, (fx + dx) >> 16);
        fx += dx+dx;
        *dst++ = pack_two_shorts(fx >> 16, (fx + dx) >> 16);
        fx += dx+dx;
    }
    uint16_t* xx = (uint16_t*)dst;

    for (i = (count & 3); i > 0; --i)
    {
        *xx++ = SkToU16(fx >> 16); fx += dx;
    }
}

void decal_filter_scale(uint32_t dst[], SkFixed fx, SkFixed dx, int count)
{
    if (count & 1)
    {
        SkASSERT((fx >> (16 + 14)) == 0);
        *dst++ = (fx >> 12 << 14) | ((fx >> 16) + 1);
        fx += dx;
    }
    while ((count -= 2) >= 0)
    {
        SkASSERT((fx >> (16 + 14)) == 0);
        *dst++ = (fx >> 12 << 14) | ((fx >> 16) + 1);
        fx += dx;

        *dst++ = (fx >> 12 << 14) | ((fx >> 16) + 1);
        fx += dx;
    }
}

///////////////////////////////////

void repeat_nofilter_identity(uint32_t dst[], int x, int width, int count)
{
    if (x >= width)
        x %= width;

    int i;
    uint16_t* xx = (uint16_t*)dst;

    // do the first partial run
    int n = width - x;
    if (n > count)
        n = count;
    
    count -= n;
    n += x;
    for (i = x; i < n; i++)
        *xx++ = SkToU16(i);

    // do all the full-width runs
    while ((count -= width) >= 0)
        for (i = 0; i < width; i++)
            *xx++ = SkToU16(i);

    // final cleanup run
    count += width;
    for (i = 0; i < count; i++)
        *xx++ = SkToU16(i);
}

