#ifndef _hwioh
#define _hwioh
void pbyteout(unsigned short port, unsigned char data);
unsigned char pbytein(unsigned short port);

void p_initserial();
int p_serial_received();
char p_serial_read();
int p_transmit_empty();
void p_serial_write(char byte);
#endif