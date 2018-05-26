// main.c

#include "function.h"
#include "interrupt.h"
#include "data.h"
#include "shell.h"

void shell();
void translate_shell();

void main()
{
	kprintf("We are now in C!", 10, 10);
	
	init_intdesc();

	kprintf_clear_all();

	shell();

}

void shell()
{
	char path[] = "HoGoS>>";

	curline = 0;
	curcol = kstrlen(path);

	while (1)
	{
		//__asm__ __volatile__("cli");

		if ( kindex != 0 && keyboard[kindex - 1] == ENTER)
		{	
			kprintf_line_clear(curline, curcol + kindex - 1);
			keyboard[kindex - 1] = 0;

			translate_shell();

			for (int i = 0; i < videomaxcol; i++)
				keyboard[i] = 0;

			curline++;
			kindex = 0;
		}

		kprintf(path,curline,0);
		kprintf_line_clear(curline, curcol+kindex);
		kprintf(keyboard, curline, curcol);

		//__asm__ __volatile__("sti");
	}
}

void translate_shell()
{
	if (keyboard[0] == 0) { return; } // ���ɾ� ���� �׳� ENTER ħ
	if (kstrcmp(keyboard, "clear")) { sh_clear(); return; }
	if (kstrcmp(keyboard, "version")) { sh_version(); return; }
	if (kstrcmp(keyboard, "read")) { sh_HDDread(); return; }
	if (kstrcmp(keyboard, "write")) { sh_HDDwrite(); return; }
	if (kstrcmp(keyboard, "arg")) { sh_arg(); return; }
	
	kprintf("There is no such command.",++curline, 0);
}