#pragma once


// ���� �Լ�
void kprintf(char*, int, int); // str : ����� ��Ʈ�� �ּ� , int : �� ��° �ٿ� ����� ������, int : �� ��° �࿡ ����� ������
void kprintf_line_clear(int, int);
void kprintf_clear_all();
void kprintf_byte(char*, int, int);
char translate_byte(unsigned char x);
void kprintf_sector(char*);

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
