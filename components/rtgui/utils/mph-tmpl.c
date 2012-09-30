/* adapted from utils/perfect_hash/example1-C/states-tmpl.c */

#include <string.h>

#include "mph-code.h"

static uint8_t T1[] = { $S1 };
static uint8_t T2[] = { $S2 };
static uint8_t G[] = { $G };

static int hash_g(const rt_uint16_t key, const int *T)
{
    // NOTE: little endian
    int sum = (T[0] * (key>>8) + T[1] * (key&0xFF)) % $NG;
    return G[sum % $NG];
}

static int perfect_hash(const rt_uint16_t key)
{
    return (hash_g(key, T1) + hash_g(key, T2)) % $NG;
}

int rtgui_font_mph(const rt_uint16_t key)
{
    int hash_value = perfect_hash(key);

    /* NOTE: we trust you will not feed invalid data into me. So there is no
     * more sanity check which will consume more flash and memory. */
    if (hash_value < $NK)
        return hash_value;
    return -1;
}

static const unsigned char compact_hz[] = { $font_data };

static const struct rtgui_font_bitmap compact_bitmap=
{
	compact_hz, /* bmp */
	RT_NULL,    /* each character width, NULL for fixed font */
	RT_NULL,    /* offset for each character */
	$width,     /* width */
	$height,    /* height */
	0,          /* first char */
	255         /* last char */
};

extern struct rtgui_font_engine hz_bmp_font_engine;
struct rtgui_font rtgui_font_hzcmp${height} =
{
    "hz",                    /* family */
    $height,                /* height */
    1,                       /* refer count */
    &hz_bmp_font_engine,     /* font engine */
    (void *)&compact_bitmap, /* font private data */
};

