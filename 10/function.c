#include "function.h"

void kprintf(char* str, int line, int col) // str ���ڸ� line ��° �ٿ� ����ϴ� �Լ�
{
	char *video = (char*)(0xB8000 + 160 * line + col);

	for (int i = 0; str[i] != 0; i++)
	{
		*video++ = str[i];
		*video++ = 0x03; // ù °�� ����, �� ���� ���� ��ü ��
	}

	return;
}

