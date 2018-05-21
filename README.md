# HoGoS
Operating System based on x86 - Goal for minimalism OS

You can learn how I develop the HoGoS on http://blog.naver.com/simhs93/221256936264 But sadly, it is written in korean.

But the source code itself can help to make your own OS I think.

Each folder's name is some kind of version of the HoGoS. So the more higher, the better HoGoS implemented.

Here is the list of each folder and some description about them.

===================================================================

3. Boot the kernel

Simple OS code which just print the "hiih" in the monitor. 

Boot.asm >> Loaded in 0x7C00 by BIOS. read the 1 sector, load it on 0x10000, jump to them.

Sector2.asm >> Print "ih" make sure jump is succeed.


disk.img >> you can see the result of HoGoS when you run this image in the virtual pc.

===================================================================

5. Switch real mode to protected mode

x86 support the real mode(16bit) but have to switch protected mode(32bit) for better OS.

Boot.asm >> Add the GDT(Global Descript Table).

Sector2.asm >> In protected mode(32bit), we can access the memory by using segment offset. Use VideoSegment to print "P".


disk.img >> you can see the result of HoGoS when you run this image in the virtual pc.

===================================================================

6. Make function in assembly

Make the "print" function just for fun. Now you can print the whole text line with that function.

Sector2.asm >> Add the function called "printf"

disk.img >> you can see the result of HoGoS when you run this image in the virtual pc.

===================================================================

7. Making OS with C language

You can now make the OS with C language! Look at the file "Makefile" carefully. It will show you how can we develop the OS with C.

From this version, you can simply complile HoGoS with "Make" command.

Boot.asm >> Read the 2 sector to load the "main.c". So, "Sector2" loaded in the 0x10000 and "main.c" loaded in the 0x10200.

Sector2.asm >> Jump to the 0x10200 where "main.c" will be loaded.

main.c >> where the start of the HoGoS implemented by C.

Makefile >> Compile them and merge the whole code into one sigle image which is "final.img"

final.img >> you can see the result of HoGoS when you run this image in the virtual pc

===================================================================

9. Make function in C

There is nothing change in version 7, but we make the "kprint" function in C, so we can use them anywhere.

function.c & function.h >> Many function will be implemented in this source code. This version, just "kprint" which print the text in the screen implemented.

Makefile >> Add some command because function.c & function.h added.

final.img >> you can see the result of HoGoS when you run this image in the virtual pc

===================================================================

10. Interrupt 

HoGoS now can receive the interrupt. But it is really complicated. So please read the description carefully.

Boot.asm >> Add the PIC setting code.

interrupt.c & .h >> Implement the IDT(Interrupt Descript Table) and load them to 0x0. HoGoS will provide the 2 ISR(Interrupt Service Routine), keyboard and timer. idt_timer() and idt_keyboard() will provide that. Other interrupt will be ignored using idt_ignore().
(interrupt will happen but HoGoS do nothing, because idt_ignore() do nothing. But idt_timer() will print the text when interrupt happend and idt_keyboard() will print the text when you tap the keyboard )

Makefile >> Add some command for interrupt.c & .h

final.img >> you can see the result of HoGoS when you run this image in the virtual pc

===================================================================

14. Keyboard driver

HoGoS now receive the keyboard value using keyboard interrupt.

interrupt.c >> Receive the keyboard value called ScanCode, and translate them to ASCII using function "transScan". And print them using "kprint". when user enter the "Backspace", delete one text. 

function.c >> Add the "kprintf_line_clear" which clear the whole text line.

final.img >> you can see the result of HoGoS when you run this image in the virtual pc

===================================================================

16. Shell

Implement the basic Shell which provide the 2 command, clear and version.

main.c >> Make the shell(). Shell() will check the keyboard[] which command line stored. When user tap the keyboard, single text will be stored in keyboard[]. When user finally tap the enter key, Shell() will call the translate_shell() to identify which command user want.

shell.c & .h >> implement the command which HoGoS provide. Also, we have to make if statement in the translate_shell() in main.c

interrupt.c >> In idt_keyboard(), store the single text in keyboard[] when user tap the keyboard.

final.img >> you can see the result of HoGoS when you run this image in the virtual pc

===================================================================

19. Hard Disk Driver - Read

Read the HDD sector. Before run this version, you have to plug in HDD. I implemented this by using Qemu.

function.c >> Implement the function "HDDread". HoGoS now can read a single sector. For example, when you call HDDread(10, buf), Sector 10 in HDD will be stored in buf(512byte)

final.img >> you can see the result of HoGoS when you run this image in the virtual pc

===================================================================

20. Hard Disk Driver - Write

Write the HDD sector. Logic is same as version 19.

function.c >> Implement the function "HDDwrite". HoGoS now can write a single sector. For example, when you call HDDwrite(10, buf), buf(512byte) will be printed to sector 10 in HDD. You can test this function when you command "write" to HoGoS shell. It will print "Hello world" to sector 2 and 3 ( Please see sh_HDDwrite in shell.c )

final.img >> you can see the result of HoGoS when you run this image in the virtual pc

===================================================================
