#pragma once

void kprintf(char*, int, int); // str : ����� ��Ʈ�� �ּ� , int : �� ��° �ٿ� ����� ������, int : �� ��° �࿡ ����� ������
void kprintf_line_clear(int, int);
void kprintf_clear_all();

int kstrcmp(char*, char*);
int kstrlen(char*);