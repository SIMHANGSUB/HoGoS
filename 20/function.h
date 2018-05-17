#pragma once


// 비디오 함수
void kprintf(char*, int, int); // str : 출력할 스트링 주소 , int : 몇 번째 줄에 출력할 것인지, int : 몇 번째 행에 출력할 것인지
void kprintf_line_clear(int, int);
void kprintf_clear_all();
void kprintf_byte(char*, int, int);
char translate_byte(unsigned char x);
void kprintf_sector(char*);

// 스트링 함수
int kstrcmp(char*, char*);
int kstrlen(char*);

// HDD 함수
void HDDread(unsigned int, char*);
void HDDwrite(unsigned int, char*);
unsigned char HDDstatus();
int HDD_BSY();
int HDD_DRDY();
int HDD_DRQ();
int HDD_ERR();

// 기타 함수
