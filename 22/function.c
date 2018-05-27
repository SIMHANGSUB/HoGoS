#include <stdarg.h>
#include "function.h"
#include "data.h"

void kprintf(char* str, int line, int col,...) // str ���ڸ� line ��° �ٿ� �����ϴ� �Լ�
{

	va_list va;
	va_start(va,col);

	char *video = (char*)(0xB8000 + 160 * line + 2*col);

	for (int i = 0; str[i] != 0; i++)
	{
		if ( str[i] == '%' )
			kprintf_arg(((int)va_arg(va,int)), &video);

		else
		{
			*video++ = str[i];
			*video++ = 0x03; // ù °�� ������, �� ���� ���� ��ü ��
		}
	}

	va_end(va);

	return;
}

void kprintf_arg(int target, char** video)
{
	unsigned char buffer[64];
	int i = 0;

	for( ; target != 0 ; i++)
	{
		buffer[i] = '0' + target % 10;
		target = target / 10;
	}

	buffer[i] = 0;

	for( i-- ; i >= 0 ; i-- )
	{
		**video = buffer[i];
		(*video)++;

		**video = 0x03;
		(*video)++;

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

	for (int i = 0; ( i < kstrlen(str1) ) || ( i < kstrlen(str2) ); i++)
		if (str1[i] != str2[i]) return FALSE;

	return TRUE;
}

int kstrlen(char* str1)
{
	int i = 0;
	while (str1[i] != 0) i++;
	return i;
}

int kmemcpy(char* src, char* dest, int size)
{
	int i;

	for ( i = 0 ; i < size ; i++ )
		dest[i] = src[i];

	return i;
}

void HDDwrite(unsigned int sector, char* buffer)
{

	unsigned char LBA_a = sector & 0xFF; // sector�� [7:0] ��Ʈ ����
	unsigned char LBA_b = (sector >> 8) & 0xFF; // sector�� [15:8] ��Ʈ ����
	unsigned char LBA_c = (sector >> 16) & 0xFF; // sector�� [23:16] ��Ʈ ����
	unsigned char LBA_d = ((sector >> 24) & 0x0F) | 0xE0; // sector�� [27:24] ��Ʈ ����

	// HDD INT Ȱ��ȭ
	__asm__ __volatile__
	(
		"mov al, 0;"
		"mov dx, 0x3F6;"
		"out dx, al;"
	);

	while (HDD_BSY() == 1); // HDD�� busy �ϴٸ� ���� ����

	/////////////////////////////////////////////////
	// �ϵ�����ũ ���� ����
	/////////////////////////////////////////////////

	// �����̺�/���� �������� �ʱ�ȭ + LBA �ּ� [27:24] 4��Ʈ
	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx, 0x1F6;"
		"out dx, al;"::"r"(LBA_d)
	);

	__asm__ __volatile__
	(
		"mov al, 0x01;"
		"mov dx,0x1F2;"
		"out dx, al;"
	); // ���� 1�� ����

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F3;"
		"out dx, al;" ::"r"(LBA_a)
	); // LBA �ּ� [7:0] 8��Ʈ

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F4;"
		"out dx, al;" ::"r"(LBA_b)
	); // LBA �ּ� [15:8] 8��Ʈ

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F5;"
		"out dx, al;" ::"r"(LBA_c)
	); // LBA �ּ� [23:16] 8��Ʈ

	   /////////////////////////////////////////////////
	   // �ϵ�����ũ ���� ��
	   /////////////////////////////////////////////////


	   // ����(0x30) ������ �� �ϵ�����ũ �����̹��� ������ ���� �� �ִ��� üũ
	while ((HDD_BSY() == 1) || (HDD_DRDY() == 0));


	// ����(0x30) ���� ������
	__asm__ __volatile__
	(
		"mov al, 0x30;"
		"mov dx,0x1F7;"
		"out dx, al;"
	);

	// ���� ���ȴµ� ������ �߻��ߴٸ� ���⸦ �ߴ��Ѵ�.
	if (HDD_ERR() == 1)
	{
		kprintf("Error!!", videomaxline - 1, 0);
		return;
	}

	while (HDD_DRQ() == 0); // �����͸� �� ������ ����


	//  Buffer�� 512����Ʈ��ŭ �����͸� �޸���(0x1F0)�� ����
	__asm__ __volatile__("mov dx,0x1F0;");
	__asm__ __volatile__("mov esi, %0;" : : "r"(buffer));
	__asm__ __volatile__("mov ecx, 256");
	__asm__ __volatile__("rep outsw");


}

void HDDread(unsigned int sector, char* buffer)
{
	
	unsigned char LBA_a = sector & 0xFF; // sector�� [7:0] ��Ʈ ����
	unsigned char LBA_b = ( sector >> 8 ) & 0xFF; // sector�� [15:8] ��Ʈ ����
	unsigned char LBA_c = ( sector >> 16) & 0xFF; // sector�� [23:16] ��Ʈ ����
	unsigned char LBA_d = ( ( sector >> 24) & 0x0F ) | 0xE0 ; // sector�� [27:24] ��Ʈ ����

	// HDD INT Ȱ��ȭ
	__asm__ __volatile__
	(
		"mov al, 0;"
		"mov dx, 0x3F6;"
		"out dx, al;"
	);

	while ( HDD_BSY() == 1); // HDD�� busy �ϴٸ� ���� ����
	
	/////////////////////////////////////////////////
	// �ϵ�����ũ ���� ����
	/////////////////////////////////////////////////

	// �����̺�/���� �������� �ʱ�ȭ + LBA �ּ� [27:24] 4��Ʈ
	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx, 0x1F6;"
		"out dx, al;"::"r"(LBA_d)
	);

	__asm__ __volatile__
	(
		"mov al, 0x01;"
		"mov dx,0x1F2;"
		"out dx, al;"
	); // ���� 1�� �д´�

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F3;"
		"out dx, al;" ::"r"(LBA_a)
	); // LBA �ּ� [7:0] 8��Ʈ

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F4;"
		"out dx, al;" ::"r"(LBA_b)
	); // LBA �ּ� [15:8] 8��Ʈ

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F5;"
		"out dx, al;" ::"r"(LBA_c)
	); // LBA �ּ� [23:16] 8��Ʈ

	/////////////////////////////////////////////////
	// �ϵ�����ũ ���� ��
	/////////////////////////////////////////////////


	// �б�(0x20) ������ �� �ϵ�����ũ �����̹��� ������ ���� �� �ִ��� üũ
	while ((HDD_BSY() ==1 )|| (HDD_DRDY()==0));


	// �б�(0x20) ���� ������
	__asm__ __volatile__
	(
		"mov al, 0x20;"
		"mov dx,0x1F7;"
		"out dx, al;"
	);

	// ���� ���ȴµ� ������ �߻��ߴٸ� �б⸦ �ߴ��Ѵ�.
	if (HDD_ERR() == 1)
	{
		kprintf("Error!!", videomaxline - 1, 0);
		return;
	}

	while (HDD_DRQ() == 0); // �����͸� �� ���� ������ ����


	// �бⰡ ���������Ƿ� Buffer���ٰ� 512����Ʈ��ŭ �����͸� �ű���.
	__asm__ __volatile__("mov dx,0x1F0;");
	__asm__ __volatile__("mov edi, %0;" : : "r"(buffer));
	__asm__ __volatile__("mov ecx, 256");
	__asm__ __volatile__("rep insw");


}

void kprintf_byte(char* str, int line, int col)
{
	char string[3];

	if ((int)*str == 0)
	{
		string[0] = '0';
		string[1] = '0';
		string[2] = 0;
	}
	else 
	{
		string[0] = translate_byte(((unsigned char)(*str)) / 16);
		string[1] = translate_byte(((unsigned char)(*str)) % 16);
		string[2] = 0;
	}
	
	kprintf(string, line, col);
}

char translate_byte(unsigned char x)
{
	switch (x)
	{
	case 0: return '0';
	case 1: return '1';
	case 2: return '2';
	case 3: return '3';
	case 4: return '4';
	case 5: return '5';
	case 6: return '6';
	case 7: return '7';
	case 8: return '8';
	case 9: return '9';
	case 10: return 'A';
	case 11: return 'B';
	case 12: return 'C';
	case 13: return 'D';
	case 14: return 'E';
	case 15: return 'F';
	default: return 'Z';
	}
}

void kprintf_sector(char* buffer)
{
	int i;
	int col = 0;
	curline++;

	for (i = 0; i < 512; i++)
	{
		kprintf_byte(&buffer[i], curline, col);
		if ( col >= videomaxcol-3 )
		{
			curline++;
			col = 0;
		}
		else col = col + 3;
	}
	
}

unsigned char HDDstatus()
{
	unsigned char value;

	__asm__ __volatile__
	(
		"mov dx,0x1F7;"
		"in al, dx;"
	);
	__asm__ __volatile__("mov %0, al;" :"=r"(value));

	return value;

}

int HDD_BSY()
{
	unsigned char status = HDDstatus();

	if ( (status & 0x80) == 0x80) return 1; // busy
	else return 0; // non-busy

}

int HDD_DRDY()
{
	unsigned char status = HDDstatus();

	if ((status & 0x40) == 0x40) return 1; // ready
	else return 0; // non-ready
}

int HDD_DRQ()
{
	unsigned char status = HDDstatus();

	if ((status & 0x08) == 0x08) return 1; // Data Request
	else return 0; // non-Data Request
}

int HDD_ERR()
{
	unsigned char status = HDDstatus();

	if ((status & 0x01) == 0x01) return 1; // error!
	else return 0; // non-error
}

