#ifndef _BITMAPH
#define _BITMAPH
#include "memlib.h"

typedef unsigned char* k_bitmap;

//Allocates heap space for a bitmap of size
#define create_bitmap(size) (k_bitmap) kmalloc(size / 8)

//These manage the bits from most-significant to least-significant so it would fill like:
//10000000 00000000
//11000000 00000000
//11100000 00000000
//       ...
//11111111 00000000
//11111111 10000000
//11111111 11000000
//       ...
//11111111 11111111
//This is the system default way of storing bits
#define bitmap_set_bit(bitmap, bit) bitmap[bit / 8] = bitmap[bit / 8] | (0x80 >> (bit % 8))
#define bitmap_clear_bit(bitmap, bit) bitmap[bit / 8] = (((bitmap[bit / 8] & ((0x80 >> (bit % 8)))) > 0) ? (bitmap[bit / 8] ^ (0x80 >> (bit % 8))) : bitmap[bit / 8])
#define bitmap_is_bit_set(bitmap, bit) ((bitmap[bit / 8] & (0x80 >> (bit % 8))) > 0)

//These manage bits in the order of least-significant to most-significant so it would fill like:
//00000001 00000000
//00000011 00000000
//00000111 00000000
//       ...
//11111111 00000000
//11111111 00000001
//11111111 00000011
//       ...
//11111111 11111111
//This is needed for the LEAN filesystem which stores it's bitmap like this
#define bitmap_set_bit_be(bitmap, bit) bitmap[bit / 8] = bitmap[bit / 8] | (1 << (bit % 8))
#define bitmap_clear_bit_be(bitmap, bit) bitmap[bit / 8] = (((bitmap[bit / 8] & ((1 << (bit % 8)))) > 0) ? (bitmap[bit / 8] ^ (1 << (bit % 8))) : bitmap[bit / 8])
#define bitmap_is_bit_set_be(bitmap, bit) ((bitmap[bit / 8] & (1 << (bit % 8))) > 0)

#endif