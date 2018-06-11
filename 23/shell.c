#include "data.h"
#include "shell.h"
#include "function.h"
#include "fs.h"


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
	if (kstrcmp(keyboard, "fs")) { sh_fsstatus(); return; }

	kprintf("There is no such command.",++curline, 0);
}


void sh_clear()
{
	kprintf_clear_all();
	curline = -1;
}

void sh_version()
{
	kprintf("HoGoS [version 0.0.1]", ++curline, 0);
	kprintf("Copyright <c> 2018 HoGuSoft. All rights reserved", ++curline, 0);
}

void sh_HDDread()
{
	HDDread(1, diskbuffer);
	kprintf(diskbuffer, ++curline, 0);

	HDDread(2, diskbuffer);
	kprintf(diskbuffer, ++curline, 0);
}

void sh_HDDwrite()
{

	unsigned char test[512] = "Hello world!";

	HDDwrite(2, test);
	HDDwrite(3, test);

}

void sh_arg()
{
	int test[] = { 12, 345, 6789, 12345 };

	kprintf("Test : % and % and % and % and % ", 
	++curline, 0, test[0], test[1], test[2], test[3], 3);

}

void sh_fsstatus()
{
	
	kprintf("Total block : %  , Total inode : %",++curline,0,
	superblock.s_blocks_count,superblock.s_inodes_count);

	kprintf("Free block : %  , Free inode : %",++curline,0,
	superblock.s_free_blocks_count,superblock.s_free_inodes_count);

	kprintf("Block Bitmap : % , Inode Bitmap : % , InodeTable Bitmap : % "
	,++curline,0,groupblock.bg_block_bitmap,groupblock.bg_inode_bitmap,groupblock.bg_inode_table);

	kprintf("Group Free block : % , Group Free inode : % , Used dir : % "
	,++curline,0,groupblock.bg_free_blocks_count,groupblock.bg_free_inodes_count,groupblock.bg_used_dirs_count);

	
}