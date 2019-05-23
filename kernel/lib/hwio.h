#ifndef _hwioh
#define _hwioh

//pbyeout
//Params: Port-the port to output to, data-the byte to write
//Outputs a byte of data to the port specified
void pbyteout(unsigned short port, unsigned char data);

//pbytein
//Params: Port-the port to read
//Reads in a byte from the port specified
//Returns: The byte read in
unsigned char pbytein(unsigned short port);

//pwordout
//Params: Port-the port to output to, data-the word to write
//Outputs a word(16 bits) of data to the port specified
void pwordout(unsigned short port, unsigned short data);

//pwordin
//Params: Port-the port to read
//Reads in a word(16 bits) from the port specified
//Returns: The word read in
unsigned short pwordin(unsigned short port);

//p_initserial
//Intializes COM1 serial port
//Sets the baud rate to 38400, no parity, one stop bit
void p_initserial();

//p_serial_received
//Checks if there is data availible on the serial port
//Returns: 1 if there data ready to be read
int p_serial_received();

//p_serial_read
//Reads one byte of data from the serial port
//Will not wait for data to be on the line before reading
//Returns: The byte read from the serial port or 0 if there was no data on the line
char p_serial_read();

//p_transmit_empty
//Checks if the transmision buffer is empty and that the port is ready to send more data
//Returns: 1 if the buffer is empty and ready to send more data
int p_transmit_empty();

//p_serial_write
//Params: byte-the byte of data to send on the serial port
//Sends a byte of data to the serial port
void p_serial_write(char byte);

//p_serial_writestring
//Params: string-the c string to send on the serial port
//Sends a string to the serial port
void p_serial_writestring(char* string);

//p_serial_writebytes
//Params: bytes-the bytes of data to send on the serial port, size-the size of the data to send
//Sends multiple bytes of data to the serial port
void p_serial_writebytes(char* bytes, unsigned int size);

void p_serial_writenum(long in);
#endif