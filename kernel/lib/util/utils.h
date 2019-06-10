#ifndef _utilsh
#define _utilsh

unsigned int util_genRand();
unsigned char* util_genUUID();

char** strsplt(char* str, char token, int* count);
char strcmp(char* str1, char* str2);
unsigned int strlen(char* str);

#endif