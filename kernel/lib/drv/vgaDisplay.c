#include "vgaDisplay.h"
#include "hwio.h"
#include "../util/memlib.h"
#include "../util/bitmap.h"

#define VGA_DISPLAY_VBE_INFO_TABLE_LOC 0x20000
#define VGA_DISPLAY_BLOCK_SIZE 32
#define VGA_SYNC_TIME 25

struct vbe_mode_info_structure* vbe_table;

unsigned char* backbuffer;
unsigned char* frontbuffer;
k_bitmap dirtyBuffer;
k_bitmap clearDirtyBuffer;
unsigned int bufferSize;
unsigned int bitmapSize;

unsigned int mapWidth;
unsigned int mapHeight;
unsigned int pxMapWidth;
unsigned int pxMapHeight;
unsigned int pxDispWidth;

unsigned char* vga_font_bitmap;
unsigned int currentBuffer = 0;

void vga_initDisplay()
{
	vbe_table = (struct vbe_mode_info_structure*) VGA_DISPLAY_VBE_INFO_TABLE_LOC;

	unsigned short width = vbe_table->width;
	unsigned short height = vbe_table->height;
	unsigned char colorDepth = vbe_table->bpp;
	unsigned short attribs = vbe_table->attributes;
	unsigned int framebuffer = vbe_table->framebuffer;

	frontbuffer = (unsigned char*) framebuffer;
	bufferSize = (width * height * (colorDepth >> 3));
	backbuffer = (unsigned char*) kmalloc(bufferSize);
	p_serial_printf("Backbuffer location %xi\n", backbuffer);

	bitmapSize = ((width*height) / VGA_DISPLAY_BLOCK_SIZE)  / 8;
	dirtyBuffer = (k_bitmap) kmalloc(bitmapSize);
	clearDirtyBuffer = (k_bitmap) kmalloc(bitmapSize);

	memset(backbuffer, 0, bufferSize);
	memcpy(frontbuffer, backbuffer, bufferSize);

	p_serial_printf("Width %i, Height %i, colorDepth %i, attributes %xs\n", width, height, colorDepth, attribs);
	p_serial_printf("Red mask %xb, Red location %xb, Blue mask %xb, Blue location %xb, Green mask %xb, Green location %xb\n", 
					vbe_table->red_mask, vbe_table->red_position,
					vbe_table->blue_mask, vbe_table->blue_position,
					vbe_table->green_mask, vbe_table->green_position);

	mapWidth = (width / VGA_DISPLAY_BLOCK_SIZE);
	mapHeight = (height / VGA_DISPLAY_BLOCK_SIZE);
	pxMapWidth =  VGA_DISPLAY_BLOCK_SIZE * (width * 3);
	pxMapHeight = VGA_DISPLAY_BLOCK_SIZE * 3;
	pxDispWidth = (width * 3);
}

void vga_swap_buffers()
{
	//Stop preemtion
	__asm__("int $82");

	unsigned int bitOffset;
	unsigned int vMemOffset;
	vga_clear_dirty_screen();
	for(int i = 0; i < mapHeight; i++)
	{
		for(int j = 0; j < mapWidth; j++)
		{
			bitOffset = j + (i * mapWidth);

			if(bitmap_is_bit_set(dirtyBuffer, bitOffset))
			{
				//p_serial_printf("WRITE BIT %i\n", bitOffset);
				vMemOffset = (j * pxMapHeight) + (i * pxMapWidth);
				//p_serial_printf("vMemOffset %xi\n", vMemOffset);
				for(int k = 0; k < VGA_DISPLAY_BLOCK_SIZE; k++)
				{
					for(int l = 0; l < VGA_DISPLAY_BLOCK_SIZE; l++)
					{
						bitOffset = vMemOffset + (k * pxDispWidth) + (l * 3);
						frontbuffer[bitOffset] = backbuffer[bitOffset];
						backbuffer[bitOffset] = 0;

						frontbuffer[bitOffset+ 1] = backbuffer[bitOffset + 1];
						backbuffer[bitOffset + 1] = 0;

						frontbuffer[bitOffset+ 2] = backbuffer[bitOffset + 2];
						backbuffer[bitOffset + 2] = 0;
					}
				}
			}
		}
	}

	//memcpy(frontbuffer, backbuffer, bufferSize);
	//memset(backbuffer, 0, bufferSize);
	memcpy(clearDirtyBuffer, dirtyBuffer, bitmapSize);
	memset(dirtyBuffer, 0, bitmapSize);

	//Reenable preemtion
	__asm__("int $81");
}

void vga_swap_buffers_noclr()
{
	unsigned int bitOffset;
	unsigned int vMemOffset;
	vga_clear_dirty_screen();
	for(int i = 0; i < mapHeight; i++)
	{
		for(int j = 0; j < mapWidth; j++)
		{
			bitOffset = j + (i * mapWidth);

			if(bitmap_is_bit_set(dirtyBuffer, bitOffset))
			{
				//p_serial_printf("WRITE BIT %i\n", bitOffset);
				vMemOffset = (j * pxMapHeight) + (i * pxMapWidth);
				//p_serial_printf("vMemOffset %xi\n", vMemOffset);
				for(int k = 0; k < VGA_DISPLAY_BLOCK_SIZE; k++)
				{
					for(int l = 0; l < VGA_DISPLAY_BLOCK_SIZE; l++)
					{
						bitOffset = vMemOffset + (k * pxDispWidth) + (l * 3);
						frontbuffer[bitOffset] = backbuffer[bitOffset];

						frontbuffer[bitOffset+ 1] = backbuffer[bitOffset + 1];

						frontbuffer[bitOffset+ 2] = backbuffer[bitOffset + 2];
					}
				}
			}
		}
	}

	//memcpy(frontbuffer, backbuffer, bufferSize);
	//memset(backbuffer, 0, bufferSize);
	//memcpy(clearDirtyBuffer, dirtyBuffer, bitmapSize);
	//memset(dirtyBuffer, 0, bitmapSize);
	__asm__("int $80");
}

void vga_sync_thread_func()
{
	unsigned long long lastTick = 0;
	while(1)
	{
		if(lastTick + VGA_SYNC_TIME < time_getsysticks())
		{
			lastTick = time_getsysticks();
			vga_swap_buffers();
			currentBuffer ^= 1;
		}
		else
		{
			//Forces the kernel to schedule tasks before the given timeslice is finished
			
		}
		__asm__("int $80");
	}
}

void vga_clear_dirty_screen()
{
	unsigned int bitOffset;
	unsigned int vMemOffset;

	for(int i = 0; i < mapHeight; i++)
	{
		for(int j = 0; j < mapWidth; j++)
		{
			bitOffset = j + (i * mapWidth);

			if(bitmap_is_bit_set(clearDirtyBuffer, bitOffset))
			{
				//p_serial_printf("CLEAR BIT %i\n", bitOffset);
				vMemOffset = (j * pxMapHeight) + (i * pxMapWidth);
				//p_serial_printf("vMemOffset %xi\n", vMemOffset);
				for(int k = 0; k < VGA_DISPLAY_BLOCK_SIZE; k++)
				{
					for(int l = 0; l < VGA_DISPLAY_BLOCK_SIZE; l++)
					{
						bitOffset = vMemOffset + (k * pxDispWidth) + (l * 3);
						frontbuffer[bitOffset] = 0;
						frontbuffer[bitOffset+1] = 0;
						frontbuffer[bitOffset+2] = 0;
					}
				}
			}
		}
	}

	memset(clearDirtyBuffer, 0, bitmapSize);
}

//This sets the already calculated pixel offset to the provided red, green, and blue values
void vga_set_raw_pixel(unsigned int pX, unsigned int pY, unsigned char red, unsigned char green, unsigned char blue)
{
	backbuffer[(pX + pY)] = red;
	backbuffer[(pX + pY) + 1] = green;
	backbuffer[(pX + pY) + 2] = blue;
}

void vga_setpixel(unsigned int x, unsigned int y, unsigned int color)
{
	unsigned int bitmapOffset = ((x/VGA_DISPLAY_BLOCK_SIZE) + (mapWidth * (y/VGA_DISPLAY_BLOCK_SIZE)));
	bitmap_set_bit(dirtyBuffer, bitmapOffset);
	//p_serial_printf("SET BIT %i\n", bitmapOffset);

	y *= (vbe_table->width * 3);
	x *= 3;

	backbuffer[(x + y)] = (color & (0x000000FF));
	backbuffer[(x + y+1)] = (color & (0x0000FF00)) >> 8;
	backbuffer[(x + y+2)] = (color & (0x00FF0000)) >> 16;
}

void vga_setpixel_rgb(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned int bitmapOffset = ((x/VGA_DISPLAY_BLOCK_SIZE) + (mapWidth * (y/VGA_DISPLAY_BLOCK_SIZE)));
	bitmap_set_bit(dirtyBuffer, bitmapOffset);
	//p_serial_printf("SET BIT %i\n", bitmapOffset);

	y *= (vbe_table->width * 3);
	x *= 3;

	backbuffer[(x + y)] = b;
	backbuffer[(x + y+1)] = g;
	backbuffer[(x + y+2)] = r;
}

void vga_drawrectagle(unsigned int x, unsigned int y, unsigned int sx, unsigned int sy, unsigned int color)
{
	unsigned int xStart = (x/VGA_DISPLAY_BLOCK_SIZE);
	unsigned int xEnd = ((x + sx)/VGA_DISPLAY_BLOCK_SIZE);
	unsigned int yStart = (mapWidth * (y/VGA_DISPLAY_BLOCK_SIZE));
	unsigned int yEnd = (mapWidth * ((y + sy)/VGA_DISPLAY_BLOCK_SIZE));

	for(int i = yStart; i <= yEnd; i += mapWidth)
	{
		for(int j = xStart; j <= xEnd; j++)
		{
			bitmap_set_bit(dirtyBuffer, (i + j));
			//p_serial_printf("SETQ BIT %i\n", i + j);
		}
	}
	
	y *= (vbe_table->width * 3);
	x *= 3;
	unsigned char red = (color & (0x00FF0000)) >> 16;
	unsigned char green = (color & (0x0000FF00)) >> 8;
	unsigned char blue = (color & (0x000000FF));

	unsigned int bitOffset;
	unsigned int vMemOffset = x + y;

	for(int i = 0; i < sy; i++)
	{
		for(int j = 0; j < sx; j++)
		{
			bitOffset = vMemOffset + (i * pxDispWidth) + (j * 3);
			backbuffer[bitOffset] = blue;
			backbuffer[bitOffset+1] = green;
			backbuffer[bitOffset+2] = red;
		}
	}
}

void vga_draw_bitmap(unsigned char* bitmap, unsigned int xSize, unsigned int ySize, unsigned int xPos, unsigned int yPos)
{
	unsigned int xStart = (xPos/VGA_DISPLAY_BLOCK_SIZE);
	unsigned int xEnd = ((xPos + xSize)/VGA_DISPLAY_BLOCK_SIZE);
	unsigned int yStart = (mapWidth * (yPos/VGA_DISPLAY_BLOCK_SIZE));
	unsigned int yEnd = (mapWidth * ((yPos + ySize)/VGA_DISPLAY_BLOCK_SIZE));

	for(int i = yStart; i <= yEnd; i += mapWidth)
	{
		for(int j = xStart; j <= xEnd; j++)
		{
			bitmap_set_bit(dirtyBuffer, (i + j));
			//p_serial_printf("SETQ BIT %i\n", i + j);
		}
	}

  unsigned int xOffset = 0;
  unsigned int nextRowOffset = xSize * 3;

  unsigned int bitOffset;
  unsigned int vMemOffset = (xPos * 3) + (yPos * 3 * vbe_table->width);

  for(int i = 0; i < ySize; i++)
  {
    bitOffset = vMemOffset + (i * pxDispWidth);
    for(int j = 0; j < nextRowOffset; j++)
    {
      //bitOffset = vMemOffset + (i * pxDispWidth) + (j * 3);
      backbuffer[bitOffset + j] = bitmap[xOffset + j];
      // backbuffer[bitOffset+1] = bitmap[xOffset + 1];
      // backbuffer[bitOffset+2] = bitmap[xOffset + 2];

      // xOffset += 3;
    }

    xOffset += (nextRowOffset);
  }
}

void vga_draw_char(unsigned char num, unsigned int xPos, unsigned int yPos)
{
	unsigned int xStart = (xPos/VGA_DISPLAY_BLOCK_SIZE);
	unsigned int xEnd = ((xPos + VGA_FONT_CHAR_XSIZE)/VGA_DISPLAY_BLOCK_SIZE);
	unsigned int yStart = (mapWidth * (yPos/VGA_DISPLAY_BLOCK_SIZE));
	unsigned int yEnd = (mapWidth * ((yPos + VGA_FONT_CHAR_YSIZE)/VGA_DISPLAY_BLOCK_SIZE));

	for(int i = yStart; i <= yEnd; i += mapWidth)
	{
		for(int j = xStart; j <= xEnd; j++)
		{
			bitmap_set_bit(dirtyBuffer, (i + j));
			//p_serial_printf("SETQ BIT %i\n", i + j);
		}
	}

	unsigned int xOffset = (num * VGA_FONT_CHAR_XSIZE * 3);
	unsigned int nextRowOffset = (VGA_FONT_CHAR_XSIZE * 3 * 9) - (VGA_FONT_CHAR_XSIZE * 3);

	unsigned int bitOffset;
	unsigned int vMemOffset = (xPos * 3) + (yPos * 3 * vbe_table->width);

	for(int i = 0; i < VGA_FONT_CHAR_YSIZE; i++)
	{
		for(int j = 0; j < VGA_FONT_CHAR_XSIZE; j++)
		{
			bitOffset = vMemOffset + (i * pxDispWidth) + (j * 3);
			backbuffer[bitOffset] = 0xFF - vga_font_bitmap[xOffset];
			backbuffer[bitOffset+1] = 0xFF - vga_font_bitmap[xOffset + 1];
			backbuffer[bitOffset+2] = 0xFF - vga_font_bitmap[xOffset + 2];

			xOffset += 3;
		}

		xOffset += (nextRowOffset);
	}
}

unsigned int vga_getCurrentBuffer()
{
	return currentBuffer;
}