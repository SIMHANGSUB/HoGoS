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
#define superblock_ignore 20

// groupblock offset
#define bg_block_bitmap_offset 0
#define bg_inode_bitmap_offset 4
#define bg_inode_table_offset 8
#define bg_free_blocks_count_offset 12
#define bg_free_inodes_count_offset 14
#define bg_used_dirs_count_offset 16
#define bg_pad_offset 18
#define groupblock_ignore 20

// Bitmap offset
#define Bit1 0x01
#define Bit2 0x02
#define Bit3 0x04
#define Bit4 0x08
#define Bit5 0x10
#define Bit6 0x20
#define Bit7 0x40
#define Bit8 0x80

// Define SUPER block
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


// Define Group block
struct GROUP
{
	unsigned int bg_block_bitmap;
	unsigned int bg_inode_bitmap;
	unsigned int bg_inode_table;
	
	unsigned short bg_free_blocks_count;
	unsigned short bg_free_inodes_count;
	unsigned short bg_used_dirs_count;
	unsigned short bg_pad;

	unsigned int ignore[123];

}__attribute__((packed));

struct GROUP groupblock;


// Define InodeTable
struct INODE
{
	unsigned short i_mode;
	unsigned short i_uid;

	unsigned int i_size;
	unsigned int i_atime;
	unsigned int i_ctime;
	unsigned int i_mtime;
	unsigned int i_dtime;

	unsigned short i_gid;
	unsigned short i_links_count;
	unsigned int i_blocks;
	unsigned int i_flags;
	unsigned int i_osd1;

	unsigned int i_block[15];
	unsigned int ignore[7];

}__attribute__((packed));

struct INODE root;
struct INODE cur;
unsigned int curInode;

// Define Bitmap
unsigned char BlockBitmap[1024];
unsigned char InodeBitmap[1024];


// Define Function
void initFS();
void readSuperblock();
void writeSuperblock();
void readGroupblock();
void writeGroupblock();
void readBitmap();
void readRootDir();

unsigned int searchDir(char*); // find Dir by name. return inode number
void findnReadInode(unsigned int, struct INODE* ); // find Inode block and save it
void printFile(unsigned int);

unsigned int Block_alloc();
void Block_free(unsigned int);
unsigned int Block_addr(unsigned int);

unsigned int Inode_alloc();
void Inode_free(unsigned int);
void Inode_addr(unsigned int, unsigned int* , unsigned int* );

void mkdir(unsigned int, unsigned char*, unsigned int, unsigned char* );