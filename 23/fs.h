#pragma once

// sector offset
#define superblock_sector 2
#define groupblock_sector 4

// superblock offset
#define s_inodes_count_offset 0
#define s_blocks_count_offset 4
#define s_r_blocks_count_offset 8
#define s_free_blocks_count_offset 12
#define s_free_inodes_count_offset 16

// groupblock offset
#define bg_block_bitmap_offset 0
#define bg_inode_bitmap_offset 4
#define bg_inode_table_offset 8
#define bg_free_blocks_count_offset 12
#define bg_free_inodes_count_offset 14
#define bg_used_dirs_count_offset 16
#define bg_pad_offset 18

struct SUPER
{
	unsigned int s_inodes_count;
	unsigned int s_blocks_count;
	unsigned int s_r_blocks_count;
	unsigned int s_free_blocks_count;
    unsigned int s_free_inodes_count;

    unsigned int ignore[123];

}__attribute__((packed));

struct SUPER superblock;

struct GROUP
{
	unsigned int bg_block_bitmap;
	unsigned int bg_inode_bitmap;
	unsigned int bg_inode_table;
	
	unsigned short bg_free_blocks_count;
	unsigned short bg_free_inodes_count;
	unsigned short bg_used_dirs_count;
	unsigned short bg_pad;

	unsigned int ignore[3];

}__attribute__((packed));

struct GROUP groupblock;


void initFS();
void readSuperblock();
void readGroupblock();
