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

unsigned short curline; // shell 상의 현재 라인
unsigned short curcol; // shell 상의 현재 column

unsigned int timer;