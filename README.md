# HoGoS
Operating System based on x86 - Goal for minimalism OS

You can learn how I develop the HoGoS on http://blog.naver.com/simhs93/221256936264 But sadly, it is written in korean.

But the source code itself can help to make your own OS I think.

Each folder's name is some kind of version of the HoGoS. So the more higher, the better HoGoS implemented.

Here is the list of each folder and some description about them.

===================================================================

3. Boot the kernel

Simple OS code which just print the "hiih" in the monitor. 

Boot.asm > Loaded in 0x7C00 by BIOS. read the 1 sector, load it on 0x10000, jump to them.

Sector2.asm > Print "ih" make sure jump is succeed.


disk.img > you can see the result of HoGoS when you run this image in the virtual pc.

===================================================================

5. Switch real mode to protected mode

x86 support the real mode(16bit) but have to switch protected mode(32bit) for better OS.

Boot.asm > Add the GDT(Global Descript Table).

Sector2.asm > In protected mode(32bit), we can access the memory by using segment offset. Use VideoSegment to print "P".


disk.img > you can see the result of HoGoS when you run this image in the virtual pc.

===================================================================

6. Make function in assembly

Make the "print" function just for fun. Now you can print the whole text line with that function.

Sector2.asm > Add the function called "printf"


disk.img > you can see the result of HoGoS when you run this image in the virtual pc.

===================================================================

7. Making OS with C language

You can now make the OS with C language! Look at the file "Makefile" carefully. It will show you how can we develop the OS with C.

From this version, you can simply complile HoGoS with "Make" command.

Boot.asm > Read the 2 sector to load the "main.c". So, "Sector2" loaded in the 0x10000 and "main.c" loaded in the 0x10200.

Sector2.asm > Jump to the 0x10200 where "main.c" will be loaded.

main.c > where the start of the HoGoS implemented by C.

function.c & function.h > Many function will be implemented in this source code. This version, just "kprint" which print the text in the screen implemented.

Makefile > Compile them and merge the whole code into one sigle image which is "final.img"

final.img > you can see the result of HoGoS when you run this image in the virtual pc

