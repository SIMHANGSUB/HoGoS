#include "fs.h"
#include "function.h"
#include "data.h"

void initFS()
{
    readSuperblock();
    readGroupblock();
    readBitmap();
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