#include "data.h"
#include "shell.h"
#include "function.h"

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
	HDDread(0, diskbuffer);
	kprintf(diskbuffer, ++curline, 0);

	HDDread(1, diskbuffer);
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