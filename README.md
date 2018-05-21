# HoGoS
Operating System based on x86 - Goal for minimalism OS

You can learn how I develop the HoGoS on http://blog.naver.com/simhs93/221256936264 But sadly, it is written in korean.

But the source code itself can help to make your own OS I think.

Each folder's name is some kind of version of the HoGoS. So the more higher, the better HoGoS implemented.

Here is the list of each folder and some description about them.

=======================================================================================

3. Boot the kernel

Simple OS code which just print the "hiih" in the monitor. 

Boot.asm > Loaded in 0x7C00 by BIOS. read the 1 sector, load it on 0x10000, jump to them.
Sector2.asm > Print "ih" make sure jump is succeed.

disk.img > you can see the result of HoGoS when you run this image in the virtual pc.

=======================================================================================

5. Switch real mode to protected mode

x86 support the real mode(16bit) but have to switch protected mode(32bit) for better OS.

Boot.asm > Add the GDT(Global Descript Table).
Sector2.asm > In protected mode(32bit), we can access the memory by using segment offset. Use VideoSegment to print "P".

disk.img > you can see the result of HoGoS when you run this image in the virtual pc.

=======================================================================================

