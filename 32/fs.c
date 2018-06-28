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
    kmemcpy(&diskbuffer[superblock_ignore],&(superblock.ignore),123*4);

}

void writeSuperblock()
{
    HDDwrite(superblock_sector,&superblock);
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

    kmemcpy(&diskbuffer[groupblock_ignore],&(groupblock.ignore),123*4);

}

void writeGroupblock()
{
    HDDwrite(groupblock_sector,&groupblock);
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
    HDDread(groupblock.bg_inode_bitmap*2,diskbuffer);
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

    curInode = 2;

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

void printFile(unsigned int inode)
{
    struct INODE file;
    int i, j, len, row, col;

    i = 0 ; j = 0; col = 0;
    
    findnReadInode(inode,&file);

    HDDread(file.i_block[0] * 2, diskbuffer);
    kprintf(diskbuffer, ++curline, 0);

    do
    {
        len = kstrlen(diskbuffer);

        if ( len > 512 ) len = 512;

        row = len / videomaxcol;
        col = len % videomaxcol;

        curline = curline + row;

        if ( len < 512 ) break;
        
        if ( j == 0 ) j++;
        else { i++; j--; }

        HDDread(file.i_block[i] * 2 + j, diskbuffer);
        kprintf(diskbuffer, curline, col);

    } while (file.i_block[i] != 0);


}

unsigned int Block_addr(unsigned int blocknum)
{
    return 2*blocknum;
}

unsigned int Block_alloc()
{
    int result = -1;

    for ( int i = 0 ; i < 1024 ; i++ )
    {
        if ( ( BlockBitmap[i] & Bit1 ) != Bit1 ) { 
            BlockBitmap[i] = BlockBitmap[i] | Bit1;
            result = 8*i + 1; break;
            }

        if ( ( BlockBitmap[i] & Bit2 ) != Bit2 ) { 
            BlockBitmap[i] = BlockBitmap[i] | Bit2;
            result = 8*i + 2; break;
            }

        if ( ( BlockBitmap[i] & Bit3 ) != Bit3 ) { 
            BlockBitmap[i] = BlockBitmap[i] | Bit3;
            result = 8*i + 3; break;
            }

        if ( ( BlockBitmap[i] & Bit4 ) != Bit4 ) { 
            BlockBitmap[i] = BlockBitmap[i] | Bit4;
            result = 8*i + 4; break;
            }

        if ( ( BlockBitmap[i] & Bit5 ) != Bit5 ) { 
            BlockBitmap[i] = BlockBitmap[i] | Bit5;
            result = 8*i + 5; break;
            }

        if ( ( BlockBitmap[i] & Bit6 ) != Bit6 ) { 
            BlockBitmap[i] = BlockBitmap[i] | Bit6;
            result = 8*i + 6; break;
            }

        if ( ( BlockBitmap[i] & Bit7 ) != Bit7 ) { 
            BlockBitmap[i] = BlockBitmap[i] | Bit7;
            result = 8*i + 7; break;
            }

        if ( ( BlockBitmap[i] & Bit8 ) != Bit8 ) { 
            BlockBitmap[i] = BlockBitmap[i] | Bit8;
            result = 8*i + 8; break;
            }
    }

    HDDwrite(groupblock.bg_block_bitmap*2,&BlockBitmap[0]);
    HDDwrite(groupblock.bg_block_bitmap*2+1,&BlockBitmap[512]);

    superblock.s_free_blocks_count--;
    groupblock.bg_free_blocks_count--;

    writeSuperblock();
    writeGroupblock();

    return result;
}

void Block_free(unsigned int num)
{
    unsigned char bit;
    num--;

    switch ( num % 8 )
    {
        case 0 : bit = ~Bit1; break;
        case 1 : bit = ~Bit2; break;
        case 2 : bit = ~Bit3; break;
        case 3 : bit = ~Bit4; break;

        case 4 : bit = ~Bit5; break;
        case 5 : bit = ~Bit6; break;
        case 6 : bit = ~Bit7; break;
        case 7 : bit = ~Bit8; break;
    }

    BlockBitmap[num/8] = BlockBitmap[num/8] & bit;

    
    HDDwrite(groupblock.bg_block_bitmap*2,&BlockBitmap[0]);
    HDDwrite(groupblock.bg_block_bitmap*2+1,&BlockBitmap[512]);

    superblock.s_free_blocks_count++;
    groupblock.bg_free_blocks_count++;

    writeSuperblock();
    writeGroupblock();
    
}

unsigned int Inode_alloc()
{

    int result = -1;

    for ( int i = 0 ; i < 1024 ; i++ )
    {
        if ( ( InodeBitmap[i] & Bit1 ) != Bit1 ) { 
            InodeBitmap[i] = InodeBitmap[i] | Bit1;
            result = 8*i + 1; break;
            }

        if ( ( InodeBitmap[i] & Bit2 ) != Bit2 ) { 
            InodeBitmap[i] = InodeBitmap[i] | Bit2;
            result = 8*i + 2; break;
            }

        if ( ( InodeBitmap[i] & Bit3 ) != Bit3 ) { 
            InodeBitmap[i] = InodeBitmap[i] | Bit3;
            result = 8*i + 3; break;
            }

        if ( ( InodeBitmap[i] & Bit4 ) != Bit4 ) { 
            InodeBitmap[i] = InodeBitmap[i] | Bit4;
            result = 8*i + 4; break;
            }

        if ( ( InodeBitmap[i] & Bit5 ) != Bit5 ) { 
            InodeBitmap[i] = InodeBitmap[i] | Bit5;
            result = 8*i + 5; break;
            }

        if ( ( InodeBitmap[i] & Bit6 ) != Bit6 ) { 
            InodeBitmap[i] = InodeBitmap[i] | Bit6;
            result = 8*i + 6; break;
            }

        if ( ( InodeBitmap[i] & Bit7 ) != Bit7 ) { 
            InodeBitmap[i] = InodeBitmap[i] | Bit7;
            result = 8*i + 7; break;
            }

        if ( ( InodeBitmap[i] & Bit8 ) != Bit8 ) { 
            InodeBitmap[i] = InodeBitmap[i] | Bit8;
            result = 8*i + 8; break;
            }
    }

    HDDwrite(groupblock.bg_inode_bitmap*2,&InodeBitmap[0]);
    HDDwrite(groupblock.bg_inode_bitmap*2+1,&InodeBitmap[512]);

    superblock.s_free_inodes_count--;
    groupblock.bg_free_inodes_count--;

    writeSuperblock();
    writeGroupblock();

    return result;
}

void Inode_addr(unsigned int inodeNum, unsigned int* base, unsigned int* offset)
{
    *base = groupblock.bg_inode_table*2 + (inodeNum-1) / 4;
    *offset = (inodeNum-1) % 4;  
}

void Inode_free(unsigned int num)
{
    struct INODE target;
    unsigned char bit;
    num--;

    switch ( num % 8 )
    {
        case 0 : bit = ~Bit1; break;
        case 1 : bit = ~Bit2; break;
        case 2 : bit = ~Bit3; break;
        case 3 : bit = ~Bit4; break;

        case 4 : bit = ~Bit5; break;
        case 5 : bit = ~Bit6; break;
        case 6 : bit = ~Bit7; break;
        case 7 : bit = ~Bit8; break;
    }

    InodeBitmap[num/8] = InodeBitmap[num/8] & bit;

    HDDwrite(groupblock.bg_inode_bitmap*2,&InodeBitmap[0]);
    HDDwrite(groupblock.bg_inode_bitmap*2+1,&InodeBitmap[512]);

    superblock.s_free_inodes_count++;
    groupblock.bg_free_inodes_count++;

    writeSuperblock();
    writeGroupblock();

    // Free all blocks allocated to the inode
    findnReadInode(num,&target);

    for ( int i = 0 ; target.i_block[i] != 0 ; i++ )
        Block_free(target.i_block[i]);

}

void mkdir(unsigned int sector ,unsigned char* disk, unsigned int base, unsigned char* name)
{
    unsigned int inode = Inode_alloc();
    unsigned char nlen = kstrlen(name);
	unsigned short rlen = 1024 - base;
	unsigned char ft = 2; // DIR.

    unsigned char nullnum = 0;
    unsigned int InodeBase;
    unsigned int InodeOffset;

    struct INODE mk;
    unsigned char buffer[512];

    // save meta-inode data on current directory.
    kmemcpy(&inode, &disk[base], 4);
    kmemcpy(&rlen , &disk[base + 4], 2);
    kmemcpy(&nlen, &disk[base + 6], 1);
    kmemcpy(&ft, &disk[base + 7], 1);
    kmemcpy(name, &disk[base + 8], nlen);
    while ( ((nlen+nullnum) % 4) != 0 )
		disk[base+8+nlen+(nullnum++)] = 0;

    HDDwrite(sector,&disk[0]);
    HDDwrite(sector+1,&disk[512]);

    // save data on new inode and write to HDD.
    kmemcpy(&cur,&mk,128);
    for ( int i = 0 ; i < 15 ; i++ ) mk.i_block[i] = 0;

    mk.i_block[0] = Block_alloc();

    Inode_addr(inode,&InodeBase,&InodeOffset);
    HDDread(InodeBase,diskbuffer);
    kmemcpy(&mk,&diskbuffer[128*InodeOffset],128);
    HDDwrite(InodeBase,diskbuffer);

    // save directory "." and ".."
    nlen = 1;
    rlen = 12;

    kmemcpy(&inode, &buffer[0], 4);
    kmemcpy(&rlen, &buffer[4], 2);
    kmemcpy(&nlen, &buffer[6], 1);
    kmemcpy(&ft, &buffer[7], 1);
    buffer[8] = '.'; buffer[9] = 0; buffer[10] = 0; buffer[11] = 0;

    nlen = 2;
    rlen = 1024-12;

    kmemcpy(&curInode, &buffer[12], 4);
    kmemcpy(&rlen, &buffer[16], 2);
    kmemcpy(&nlen, &buffer[18], 1);
    kmemcpy(&ft, &buffer[19], 1);
    buffer[20] = '.'; buffer[21] = '.';
    buffer[22] = 0; buffer[23] = 0;

    HDDwrite(Block_addr(mk.i_block[0]),buffer);
}

void RemoveArrageDir(unsigned int target)
{
    int Lastfound = FALSE;
    int Middlefound = FALSE;
    int i,j,base,baseR, baseP;
    unsigned char buffer[1024];
    unsigned char result[1024];
    unsigned short newrlen;
    unsigned int inode;
    unsigned short rlen;
    unsigned short temp;

    //search target file and delete. Then arrage the directory.
    for (i = 0; cur.i_block[i] != 0; i++)
    {
     
        HDDread(cur.i_block[i] * 2, &buffer[0]);
        HDDread(cur.i_block[i] * 2 + 1, &buffer[512]);

        for (base = 0 , baseR = 0; base < 1024; base = base + rlen)
        {
            kmemcpy(&buffer[base], &inode, 4);
            kmemcpy(&buffer[base + 4], &rlen, 2);

            if (inode == target)
            {
                if (base + rlen == 1024)
                {
                    kmemcpy(&buffer[baseP + 4], &temp, 2);

                    temp = temp + rlen;
                    kmemcpy(&temp, &result[baseP + 4], 2);

                    Lastfound = TRUE;
                }

                else
                {
                    newrlen = rlen;
                    Middlefound = TRUE;
                }
            }
            else
            {
                kmemcpy(&buffer[base], &result[baseR], rlen);
                baseP = baseR;
                baseR = baseR + rlen;
            }
        }

        if (Lastfound || Middlefound)
        {
            if (Middlefound)
                result[baseR - rlen + 4] = result[baseR - rlen + 4] + newrlen;

            HDDwrite(cur.i_block[i] * 2, &result[0]);
            HDDwrite(cur.i_block[i] * 2 + 1, &result[512]);

            return;
        }

    }
}

int dirnumber(unsigned int inode)
{
    struct INODE temp;
    unsigned char buffer[1024];
    unsigned short rlen;
    int i, base, result = 0;
    findnReadInode(inode,&temp);

    for (i = 0; temp.i_block[i] != 0; i++)
    {
        HDDread(temp.i_block[i] * 2, &buffer[0]);
        HDDread(temp.i_block[i] * 2 + 1, &buffer[512]);

        for (base = 0 ; base < 1024; base = base + rlen)
            { kmemcpy(&buffer[base + 4], &rlen, 2); result++;}
    }

    return result;
}