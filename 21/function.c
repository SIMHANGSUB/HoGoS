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

void HDDwrite(unsigned int sector, char* buffer)
{

	unsigned char LBA_a = sector & 0xFF; // sector의 [7:0] 비트 추출
	unsigned char LBA_b = (sector >> 8) & 0xFF; // sector의 [15:8] 비트 추출
	unsigned char LBA_c = (sector >> 16) & 0xFF; // sector의 [23:16] 비트 추출
	unsigned char LBA_d = ((sector >> 24) & 0x0F) | 0xE0; // sector의 [27:24] 비트 추출

	// HDD INT 활성화
	__asm__ __volatile__
	(
		"mov al, 0;"
		"mov dx, 0x3F6;"
		"out dx, al;"
	);

	while (HDD_BSY() == 1); // HDD가 busy 하다면 계속 대기

	/////////////////////////////////////////////////
	// 하드디스크 셋팅 시작
	/////////////////////////////////////////////////

	// 드라이브/헤드 레지스터 초기화 + LBA 주소 [27:24] 4비트
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
	); // 섹터 1개 쓴다

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F3;"
		"out dx, al;" ::"r"(LBA_a)
	); // LBA 주소 [7:0] 8비트

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F4;"
		"out dx, al;" ::"r"(LBA_b)
	); // LBA 주소 [15:8] 8비트

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F5;"
		"out dx, al;" ::"r"(LBA_c)
	); // LBA 주소 [23:16] 8비트

	   /////////////////////////////////////////////////
	   // 하드디스크 셋팅 끝
	   /////////////////////////////////////////////////


	   // 쓰기(0x30) 내리기 전 하드디스크 드라이버가 명령을 받을 수 있는지 체크
	while ((HDD_BSY() == 1) || (HDD_DRDY() == 0));


	// 쓰기(0x30) 명령 내리기
	__asm__ __volatile__
	(
		"mov al, 0x30;"
		"mov dx,0x1F7;"
		"out dx, al;"
	);

	// 명령 내렸는데 오류가 발생했다면 쓰기를 중단한다.
	if (HDD_ERR() == 1)
	{
		kprintf("Error!!", videomaxline - 1, 0);
		return;
	}

	while (HDD_DRQ() == 0); // 데이터를 쓸 때까지 대기


	//  Buffer의 512바이트만큼 데이터를 메모리(0x1F0)에 쓴다
	__asm__ __volatile__("mov dx,0x1F0;");
	__asm__ __volatile__("mov esi, %0;" : : "r"(buffer));
	__asm__ __volatile__("mov ecx, 256");
	__asm__ __volatile__("rep outsw");


}

void HDDread(unsigned int sector, char* buffer)
{
	
	unsigned char LBA_a = sector & 0xFF; // sector의 [7:0] 비트 추출
	unsigned char LBA_b = ( sector >> 8 ) & 0xFF; // sector의 [15:8] 비트 추출
	unsigned char LBA_c = ( sector >> 16) & 0xFF; // sector의 [23:16] 비트 추출
	unsigned char LBA_d = ( ( sector >> 24) & 0x0F ) | 0xE0 ; // sector의 [27:24] 비트 추출

	// HDD INT 활성화
	__asm__ __volatile__
	(
		"mov al, 0;"
		"mov dx, 0x3F6;"
		"out dx, al;"
	);

	while ( HDD_BSY() == 1); // HDD가 busy 하다면 계속 대기
	
	/////////////////////////////////////////////////
	// 하드디스크 셋팅 시작
	/////////////////////////////////////////////////

	// 드라이브/헤드 레지스터 초기화 + LBA 주소 [27:24] 4비트
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
	); // 섹터 1개 읽는다

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F3;"
		"out dx, al;" ::"r"(LBA_a)
	); // LBA 주소 [7:0] 8비트

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F4;"
		"out dx, al;" ::"r"(LBA_b)
	); // LBA 주소 [15:8] 8비트

	__asm__ __volatile__
	(
		"mov al, %0;"
		"mov dx,0x1F5;"
		"out dx, al;" ::"r"(LBA_c)
	); // LBA 주소 [23:16] 8비트

	/////////////////////////////////////////////////
	// 하드디스크 셋팅 끝
	/////////////////////////////////////////////////


	// 읽기(0x20) 내리기 전 하드디스크 드라이버가 명령을 받을 수 있는지 체크
	while ((HDD_BSY() ==1 )|| (HDD_DRDY()==0));


	// 읽기(0x20) 명령 내리기
	__asm__ __volatile__
	(
		"mov al, 0x20;"
		"mov dx,0x1F7;"
		"out dx, al;"
	);

	// 명령 내렸는데 오류가 발생했다면 읽기를 중단한다.
	if (HDD_ERR() == 1)
	{
		kprintf("Error!!", videomaxline - 1, 0);
		return;
	}

	while (HDD_DRQ() == 0); // 데이터를 다 읽을 때까지 대기


	// 읽기가 성공했으므로 Buffer에다가 512바이트만큼 데이터를 옮긴다.
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

