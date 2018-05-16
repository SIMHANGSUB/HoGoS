#include "function.h"

void kprintf(char* str, int line, int col) // str 글자를 line 번째 줄에 출력하는 함수
{
	char *video = (char*)(0xB8000 + 160 * line + col);

	for (int i = 0; str[i] != 0; i++)
	{
		*video++ = str[i];
		*video++ = 0x03; // 첫 째는 배경색, 둘 때는 글자 자체 색
	}

	return;
}

void kprintf_line_clear(int line)
{
	char *video = (char*)(0xB8000 + 160 * line);
	for (int i = 0; i < 160 ; i++)
	{
		*video++ = 0;
		*video++ = 0x03;
	}
}

