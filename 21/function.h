#pragma once

/*
typedef int __gnuc_va_list;
typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;

#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)   __builtin_va_end(v)
#define va_arg(v,l) __builtin_va_arg(v,l)
*/

// ������ �Լ�
void kprintf(char*, int, int,...); // str : ������ ��Ʈ�� �ּ� , int : �� ��° �ٿ� ������ ������, int : �� ��° �࿡ ������ ������
void kprintf_line_clear(int, int);
void kprintf_clear_all();
void kprintf_byte(char*, int, int);
char translate_byte(unsigned char x);
void kprintf_sector(char*);
void kprintf_arg(int, char**);

// ��Ʈ�� �Լ�
int kstrcmp(char*, char*);
int kstrlen(char*);

// HDD �Լ�
void HDDread(unsigned int, char*);
void HDDwrite(unsigned int, char*);
unsigned char HDDstatus();
int HDD_BSY();
int HDD_DRDY();
int HDD_DRQ();
int HDD_ERR();

// ��Ÿ �Լ�
