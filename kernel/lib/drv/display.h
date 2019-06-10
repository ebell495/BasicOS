#ifndef _displayh
#define _displayh

//disp_printc
//Param: c-A character to print to the display
//Puts a character onto the screen in the next availiable position
void disp_printc(unsigned char c);

//disp_printstring
//Param: string-A C-Style string to put onto the display
//Puts a string onto the screen in the next availiable position
void disp_printstring(char* string);

//disp_setcursor_rc
//Param: row-the row number(0-24) to put the cursor at, col-the column(0-79) to put the cursor at
//Positions the display cursor at the row,column position specified
void disp_setcursor_rc(int row, int col);

//disp_setcursor_o
//Param: offset-the memory offset to put the cursor at
//Positions the display cursor at the memory offset given
//Memory offset should be based on both the character and color information
void disp_setcursor_o(int offset);

//disp_clearscreen
//Removes all characters on the screen
//Resets the cursor back to the first position (0,0)
void disp_clearscreen();

//disp_clearcursor
//Removes the character and color information from the cursors position
//Sets the color back to White(text) on Black(background)
void disp_clearcursor();

//disp_backspace
//Moves the cursor back one position and clears that location
//Moves the cursor to the nearest taken position, ie. end of the last line where the newline character would be
void disp_backspace();

//disp_scroll
//Moves all of the display memory up one row
void disp_scroll();

//disp_getcursor
//Gets the memory location representation of the display cursor
//Returns: The memory location offset of the cursor
unsigned int disp_getcursor();

//These functions take a variable and print the hexadecimal representation of it
void disp_phex8(unsigned char x);
void disp_phex16(unsigned short x);
void disp_phex32(unsigned int x);

void disp_pnum(long x);

#endif