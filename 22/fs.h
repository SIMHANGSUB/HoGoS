#pragma once

#define superblock_sector 2

#define s_inodes_count_offset 0
#define s_blocks_count_offset 4
#define s_r_blocks_count_offset 8
#define s_free_blocks_count_offset 12
#define s_free_inodes_count_offset 16

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

void initFS();
void readSuperblock();
