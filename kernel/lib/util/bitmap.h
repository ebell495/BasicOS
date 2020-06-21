#ifndef _BITMAPH
#define _BITMAPH
#include "memlib.h"

typedef unsigned char* k_bitmap;

#define create_bitmap(size) (k_bitmap) kmalloc(size / 8)
#define bitmap_set_bit(bitmap, bit) bitmap[bit / 8] = bitmap[bit / 8] | (0x80 >> (bit % 8))
#define bitmap_clear_bit(bitmap, bit) bitmap[bit / 8] = (((bitmap[bit / 8] & ((0x80 >> (bit % 8)))) > 0) ? (bitmap[bit / 8] ^ (0x80 >> (bit % 8))) : bitmap[bit / 8])
#define bitmap_is_bit_set(bitmap, bit) ((bitmap[bit / 8] & (0x80 >> (bit % 8))) > 0)

#define bitmap_set_bit_be(bitmap, bit) bitmap[bit / 8] = bitmap[bit / 8] | (1 << (bit % 8))
#define bitmap_clear_bit_be(bitmap, bit) bitmap[bit / 8] = (((bitmap[bit / 8] & ((1 << (bit % 8)))) > 0) ? (bitmap[bit / 8] ^ (1 << (bit % 8))) : bitmap[bit / 8])
#define bitmap_is_bit_set_be(bitmap, bit) ((bitmap[bit / 8] & (1 << (bit % 8))) > 0)

#endif