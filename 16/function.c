#include "function.h"
#include "data.h"

void kprintf(char* str, int line, int col) // str 글자를 line 번째 줄에 출력하는 함수
{
	char *video = (char*)(0xB8000 + 160 * line + 2*col);

	for (int i = 0; str[i] != 0; i++)
	{
		*video++ = str[i];
		*video++ = 0x03; // 첫 째는 배경색, 둘 때는 글자 자체 색
	}

	return;
}

void kprintf_line_clear(int line, int col)
{
	char *video = (char*)(0xB8000 + 160 * line + 2*col);
	for (int i = 0; i < 160-2*col ; i++)
	{
		*video++ = 0;
		*video++ = 0x03;
	}
}

void kprintf_clear_all()
{
	for (int i = 0; i < videomaxline; i++)
		kprintf_line_clear(i,0);
}

int kstrcmp(char* str1, char* str2)
{
	for (int i = 0; i < kstrlen(str1); i++)
		if (str1[i] != str2[i]) return FALSE;
	return TRUE;
}

int kstrlen(char* str1)
{
	int i = 0;
	while (str1[i] != 0) i++;
	return i;
}