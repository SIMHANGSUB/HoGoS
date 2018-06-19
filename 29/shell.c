#include "data.h"
#include "shell.h"
#include "function.h"
#include "fs.h"

unsigned char FileType[][4]=
{
    "UNK","REG","DIR","CHA","BLO","BUF","SOC","SYM"
};

void shell()
{


	path[0] = '/'; path[1] = 0;
	pathIndex = 1;

	dirLevel = 0;

	curline = 0; curcol = kstrlen(path) + BASEPATHLEN;
	

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

		kprintf("HoGoS:%s$ ",curline,0,path);
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
	if (kstrcmp(keyboard, "blockmap")) { sh_blockmap(); return; }
	if (kstrcmp(keyboard, "inodemap")) { sh_inodemap(); return; }
	if (kstrcmp(keyboard, "ls")) { sh_ls(); return; }
	if (kstrncmp(keyboard, "cd",2)) { sh_cd(); return; }
	if (kstrncmp(keyboard, "cat",3)) { sh_cat(); return; }
	if (kstrcmp(keyboard, "alloc")) { sh_alloc(); return; }

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
	char str[] = "hello world!";
	int test[] = { 12, 345, 6789, 12345 };

	kprintf("Test : %d and %d and %d and %d and %d ", 
	++curline, 0, test[0], test[1], test[2], test[3], 3);

	kprintf(" Test: %s ",++curline,0 ,&str[0]);

}

void sh_fsstatus()
{
	
	kprintf("Total block : %d  , Total inode : %d",++curline,0,
	superblock.s_blocks_count,superblock.s_inodes_count);

	kprintf("Free block : %d  , Free inode : %d",++curline,0,
	superblock.s_free_blocks_count,superblock.s_free_inodes_count);

	kprintf("Block Bitmap : %d , Inode Bitmap : %d , InodeTable Bitmap : %d "
	,++curline,0,groupblock.bg_block_bitmap,groupblock.bg_inode_bitmap,groupblock.bg_inode_table);

	kprintf("Group Free block : %d , Group Free inode : %d , Used dir : %d "
	,++curline,0,groupblock.bg_free_blocks_count,groupblock.bg_free_inodes_count,groupblock.bg_used_dirs_count);

	kprintf("Root block : %d",++curline,0,root.i_block[0]);
}

void sh_blockmap()
{
	kprintf_sector(&BlockBitmap[0]);
	kprintf_sector(&BlockBitmap[512]);
}

void sh_inodemap()
{
	kprintf_sector(&InodeBitmap[0]);
	kprintf_sector(&InodeBitmap[512]);
}

void sh_ls()
{
	int i,base;

	unsigned int inode;
	unsigned short rlen;
	unsigned char nlen;
	unsigned char ft;
	unsigned char name[32];

	kprintf(" File Type | File name ",++curline,0);

	//search every blocks of current inode
	for ( i = 0 ; cur.i_block[i] != 0 ; i++ )
	{
		// first sector ( 0 - 512 )
		HDDread(cur.i_block[i]*2,diskbuffer);

		for ( base = 0 ; base < 512 ; base = base + rlen )
		{
			kmemcpy(&diskbuffer[base],&inode,4);
			kmemcpy(&diskbuffer[base+4],&rlen,2);
			kmemcpy(&diskbuffer[base+6],&nlen,1);
			kmemcpy(&diskbuffer[base+7],&ft,1);
			kmemcpy(&diskbuffer[base+8],name,nlen);
			name[nlen] = 0;

			if ( inode == 0 ) break;

			kprintf("    %s    | %s ",++curline,0,&FileType[ft],name);
		}

		// Second sector ( 512 - 1024 )
		HDDread(cur.i_block[i]*2+1,diskbuffer);

		for ( base = 0 ; base < 512 ; base = base + rlen )
		{
			kmemcpy(&diskbuffer[base],&inode,4);
			kmemcpy(&diskbuffer[base+4],&rlen,2);
			kmemcpy(&diskbuffer[base+6],&nlen,1);
			kmemcpy(&diskbuffer[base+7],&ft,1);
			kmemcpy(&diskbuffer[base+8],name,nlen);
			name[nlen] = 0;

			if ( inode == 0 ) break;

			kprintf("    %s    | %s ",++curline,0,&FileType[ft],name);
		}

	}
}

void sh_cd()
{
	char* name = &keyboard[3];

	unsigned int inode = searchDir(name);
	unsigned int len = kstrlen(name);

	if ( inode == -1 ) 
	{
		kprintf("Can't find '%s'",++curline,0,name);
		return;
	} 
	else
	{	
		// save to struct cur
		findnReadInode(inode,&cur); 

		// if move to high level -> delete name from path
		if ( kstrcmp("..",name) ) 
		{
			// if current dir is root, return;
			if ( dirLevel == 0 ) return;

			// else, delete name from path
			while ( path[pathIndex] != '/') pathIndex--;

			if ( dirLevel == 1 ) path[++pathIndex] = 0;
			else path[pathIndex] = 0;

			dirLevel--;
		}

		// move to itself -> do nothing
		else if ( kstrcmp(".",name ) ) return;

		// move to lower level -> add name to path
		else
		{

			// if current dir is root, just add name
			if ( dirLevel == 0 )
			{
				kmemcpy( name , &path[pathIndex] , len );
			}

			// else, add '/' + name
			else
			{
				path[pathIndex++] = '/'; 
				kmemcpy( name , &path[pathIndex] , len );
			}

			pathIndex = pathIndex + len;
			path[pathIndex] = 0;

			dirLevel++;
		}

		curcol = kstrlen(path) + BASEPATHLEN; // "HoGoS: >> " is length 8

	}

	/*
	kprintf_line_clear(DEBUG,0);
	kprintf("path: %s len: %d level: %d pathIndex: %d"
	,DEBUG,0,path,len,dirLevel,pathIndex);
	*/

	return;
}

void sh_cat()
{
	char* name = &keyboard[4];
	unsigned int inode = searchDir(name);

	if ( inode == -1 ) 
	{
		kprintf("Can't find '%s'",++curline,0,name);
		return;
	} 

	else
	{
		printFile(inode);
	}

}

void sh_alloc()
{
	kprintf("alloc 1 block , block number : %d ",++curline,0,Block_alloc());
}