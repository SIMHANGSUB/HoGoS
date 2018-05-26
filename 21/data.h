#pragma once

#define sectorsize 512

#define videomaxline 25
#define videomaxcol 80

#define BACKSPACE 0x08
#define ENTER 0x13

#define TRUE 1
#define FALSE 0

#define DEBUG 24

unsigned char keyboard[videomaxcol];
unsigned short kindex;

unsigned char diskbuffer[sectorsize];
unsigned char HDDflag;

unsigned short curline; // shell ���� ���� ����
unsigned short curcol; // shell ���� ���� column

unsigned int timer;
