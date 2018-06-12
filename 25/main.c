// main.c

#include "function.h"
#include "interrupt.h"
#include "data.h"
#include "shell.h"
#include "fs.h"

void main()
{
	
	init_intdesc(); // init interrupt
	initFS(); // init File System

	kprintf_clear_all();

	shell();

}
