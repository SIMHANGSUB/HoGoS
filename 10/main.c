// main.c

#include "function.h"
#include "interrupt.h"

void main()
{
	kprintf("We are now in C!", 10, 10);
	
	init_intdesc();

}