#include "interrupt.h"
#include "function.h"
#include "data.h"

struct IDT inttable[3];
struct IDTR idtr = { 256 * 8 - 1,0 };

unsigned char keyt[2] = { 'A',0 };
unsigned char keybuf;
unsigned int tick;

void init_intdesc()
{

	int i,j;
	unsigned int ptr;
	unsigned short *isr;

	{  // 0x00 : isr_ignore
		ptr = (unsigned int)idt_ignore;
		inttable[0].selector = (unsigned short)0x08;
		inttable[0].type = (unsigned short)0x8E00;
		inttable[0].offsetl = (unsigned short)(ptr & 0xFFFF);
		inttable[0].offseth = (unsigned short)(ptr >> 16);

	}

	{  // 0x01 : isr_timer
		ptr = (unsigned int)idt_timer;
		inttable[1].selector = (unsigned short)0x08;
		inttable[1].type = (unsigned short)0x8E00;
		inttable[1].offsetl = (unsigned short)(ptr & 0xFFFF);
		inttable[1].offseth = (unsigned short)(ptr >> 16);

	}

	{  // 0x02 : isr_keyboard
		ptr = (unsigned int)idt_keyboard;
		inttable[2].selector = (unsigned short)0x08;
		inttable[2].type = (unsigned short)0x8E00;
		inttable[2].offsetl = (unsigned short)(ptr & 0xFFFF);
		inttable[2].offseth = (unsigned short)(ptr >> 16);

	}

	// �����ּ� 0x0 ������ ISR ��ġ ����

	for (i = 0; i < 256; i++)
	{
		isr = (unsigned short*)(0x0 + i * 8);
		*isr = inttable[0].offsetl;
		*(isr + 1) = inttable[0].selector;
		*(isr + 2) = inttable[0].type;
		*(isr + 3) = inttable[0].offseth;

	}

	// Ÿ�̸� ISR ����
	{
		isr = (unsigned short*)(0x0 + 8 * 0x20);
		*isr = inttable[1].offsetl;
		*(isr + 1) = inttable[1].selector;
		*(isr + 2) = inttable[1].type;
		*(isr + 3) = inttable[1].offseth;

		timer = 0;
		tick = 0;
	}

	// Ű���� ISR ����

	{
		isr = (unsigned short*)(0x0 + 8 * 0x21);
		*isr = inttable[2].offsetl;
		*(isr + 1) = inttable[2].selector;
		*(isr + 2) = inttable[2].type;
		*(isr + 3) = inttable[2].offseth;

		kindex = 0;
	
		for (int i = 0; i < videomaxcol; i++)
			keyboard[i] = 0;
	}

	// Ű���� �۵�
	__asm__ __volatile__
	(
		"mov al, 0xAE;"
		"out 0x64, al;"
	);
	//  ���ͷ�Ʈ �۵� ����

	__asm__ __volatile__("mov eax, %0"::"r"(&idtr));
	__asm__ __volatile__("lidt [eax]");
	__asm__ __volatile__
	(
		"mov al, 0x00;" //�����̺� PIC�� ���� ���ͷ�Ʈ��
		"out 0xA1, al;" // �����д�.
		"mov al, 0x00;"// ������ PIC�� ���� ���ͷ�Ʈ��
		"out 0x21, al;" //�����д�
	);
	__asm__ __volatile__("sti");

	return;


}

void idt_ignore()
{

	__asm__ __volatile__
	(
		"push gs;"
		"push fs;"
		"push es;"
		"push ds;"
		"pushad;"
		"pushfd;"
		"mov al, 0x20;"
		"out 0x20, al;"
	);

	kprintf("idt_ignore", videomaxline-1, 40);
	
	__asm__ __volatile__
	(
		"popfd;"
		"popad;"
		"pop ds;"
		"pop es;"
		"pop fs;"
		"pop gs;"
		"leave;"
		"nop;"
		"iretd;"
	);
	
	
}

void idt_timer()
{

	__asm__ __volatile__
	(
		"push gs;"
		"push fs;"
		"push es;"
		"push ds;"
		"pushad;"
		"pushfd;"
		"mov al, 0x20;"
		"out 0x20, al;"
		
	);

	kprintf(keyt, videomaxline-1, videomaxcol-1);
	keyt[0]++;

	__asm__ __volatile__
	(
		"popfd;"
		"popad;"
		"pop ds;"
		"pop es;"
		"pop fs;"
		"pop gs;"
		"leave;"
		"nop;"
		"iretd;"
	);


}

void idt_keyboard()
{

	__asm__ __volatile__
	(
		"push gs;"
		"push fs;"
		"push es;"
		"push ds;"
		"pushad;"
		"pushfd;"
		"xor al,al;"
		"in al, 0x60;"
	);

	__asm__ __volatile__("mov %0, al;" :"=r"(keybuf) );
	
	keybuf = transScan(keybuf);

	if (keybuf == BACKSPACE && kindex != 0) // �齺���̽� �Է�
		keyboard[--kindex] = 0;
	else if (keybuf != 0xFF && keybuf != BACKSPACE)
		keyboard[kindex++] = keybuf;

	__asm__ __volatile__
	(
		"mov al, 0x20;"
		"out 0x20, al;"
	);

	__asm__ __volatile__
	(
		"popfd;"
		"popad;"
		"pop ds;"
		"pop es;"
		"pop fs;"
		"pop gs;"
		"leave;"
		"nop;"
		"iretd;"
	);

}

unsigned char transScan(unsigned char target)
{
	unsigned char result;

	switch (target) // scan code set 1 ����
	{
	case 0x1E: result = 'a'; break;
	case 0x30: result = 'b'; break;
	case 0x2E: result = 'c'; break;
	case 0x20: result = 'd'; break;
	case 0x12: result = 'e'; break;
	case 0x21: result = 'f'; break;
	case 0x22: result = 'g'; break;
	case 0x23: result = 'h'; break;
	case 0x17: result = 'i'; break;
	case 0x24: result = 'j'; break;
	case 0x25: result = 'k'; break;
	case 0x26: result = 'l'; break;
	case 0x32: result = 'm'; break;
	case 0x31: result = 'n'; break;
	case 0x18: result = 'o'; break;
	case 0x19: result = 'p'; break;
	case 0x10: result = 'q'; break;
	case 0x13: result = 'r'; break;
	case 0x1F: result = 's'; break;
	case 0x14: result = 't'; break;
	case 0x16: result = 'u'; break;
	case 0x2F: result = 'v'; break;
	case 0x11: result = 'w'; break;
	case 0x2D: result = 'x'; break;
	case 0x15: result = 'y'; break;
	case 0x2C: result = 'z'; break;
	case 0x39: result = ' '; break; // �����̽�
	case 0x34: result = '.'; break;
	case 0x0E: result = BACKSPACE; break; // �齺���̽� �ƽ�Ű�ڵ� = 8
	case 0x1C: result = ENTER; break; // Enter key
	default: result = 0xFF; break; 
		// �����ȵ� ���� �����Ѵ�. �����ڴ� 0xFF

	}

	return result;

}