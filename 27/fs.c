#include "fs.h"
#include "function.h"
#include "data.h"

void initFS()
{
    readSuperblock();
    readGroupblock();
    readBitmap();
    readRootDir();
}

void readSuperblock()
{
    // read block 2 in HDD - superblock
    HDDread(superblock_sector,diskbuffer);

    // save diskbuffer to superblock
    kmemcpy(&diskbuffer[s_blocks_count_offset],&(superblock.s_blocks_count),4);
    kmemcpy(&diskbuffer[s_free_blocks_count_offset],&(superblock.s_free_blocks_count),4);
    kmemcpy(&diskbuffer[s_free_inodes_count_offset],&(superblock.s_free_inodes_count),4);
    kmemcpy(&diskbuffer[s_inodes_count_offset],&(superblock.s_inodes_count),4);
    kmemcpy(&diskbuffer[s_r_blocks_count_offset],&(superblock.s_r_blocks_count),4);

}

void readGroupblock()
{
    // read block 4 in HDD - Groupblock
    HDDread(groupblock_sector,diskbuffer);

    // save diskbuffer to superblock
    kmemcpy(&diskbuffer[bg_block_bitmap_offset],&(groupblock.bg_block_bitmap),4);
    kmemcpy(&diskbuffer[bg_inode_bitmap_offset],&(groupblock.bg_inode_bitmap),4);
    kmemcpy(&diskbuffer[bg_inode_table_offset],&(groupblock.bg_inode_table),4);

    kmemcpy(&diskbuffer[bg_free_blocks_count_offset],&(groupblock.bg_free_blocks_count),2);
    kmemcpy(&diskbuffer[bg_free_inodes_count_offset],&(groupblock.bg_free_inodes_count),2);
    kmemcpy(&diskbuffer[bg_used_dirs_count_offset],&(groupblock.bg_used_dirs_count),2);
    kmemcpy(&diskbuffer[bg_pad_offset],&(groupblock.bg_pad),2);

}

void readBitmap()
{
    // read BlockBitmap 1 
    HDDread(groupblock.bg_block_bitmap*2,diskbuffer);
    kmemcpy(&diskbuffer,&BlockBitmap[0],512);

    // read BlockBitmap 2
    HDDread(groupblock.bg_block_bitmap*2 + 1,diskbuffer);
    kmemcpy(&diskbuffer,&BlockBitmap[512],512);

    // read InodeBitmap 1
    HDDread(groupblock.bg_inode_bitmap*2 + 1,diskbuffer);
    kmemcpy(&diskbuffer,&InodeBitmap[0],512);

    // read InodeBitmap 2
    HDDread(groupblock.bg_inode_bitmap*2 + 1,diskbuffer);
    kmemcpy(&diskbuffer,&InodeBitmap[512],512);

}

void readRootDir()
{
    // read first inode table
    HDDread(groupblock.bg_inode_table*2,diskbuffer);

    // Save the second inode table to struct root & struct cur
    kmemcpy(&diskbuffer[128],&root,128);
    kmemcpy(&diskbuffer[128],&cur,128);

}

void findnReadInode(unsigned int inodeNumber, struct INODE* target )
{
    unsigned int block = (inodeNumber-1) / 4;
    unsigned int offset = (inodeNumber-1) % 4;  
    
    unsigned int final = groupblock.bg_inode_table*2 + block;

    HDDread(final,diskbuffer);

    kmemcpy(&diskbuffer[128*offset],target,128);

}


unsigned int searchDir(char* target)
{
    
    int i,base;

	unsigned int inode;
	unsigned short rlen;
	unsigned char nlen;
	unsigned char name[256];

    for ( i = 0 ; cur.i_block[i] != 0 ; i++ )
	{
		// first sector ( 0 - 512 )
		HDDread(cur.i_block[i]*2,diskbuffer);

		for ( base = 0 ; base < 512 ; base = base + rlen )
		{
			kmemcpy(&diskbuffer[base],&inode,4);
			kmemcpy(&diskbuffer[base+4],&rlen,2);
			kmemcpy(&diskbuffer[base+6],&nlen,1);
			kmemcpy(&diskbuffer[base+8],name,nlen);
			name[nlen] = 0;

			if ( inode == 0 ) break;

            if ( kstrcmp(name,target) ) return inode;

		}

		// Second sector ( 512 - 1024 )
		HDDread(cur.i_block[i]*2+1,diskbuffer);

		for ( base = 0 ; base < 512 ; base = base + rlen )
		{
			kmemcpy(&diskbuffer[base],&inode,4);
			kmemcpy(&diskbuffer[base+4],&rlen,2);
			kmemcpy(&diskbuffer[base+6],&nlen,1);
			kmemcpy(&diskbuffer[base+8],name,nlen);
			name[nlen] = 0;

			if ( inode == 0 ) break;

            if ( kstrcmp(name,target) ) return inode;

		}

	}

    return -1;
}