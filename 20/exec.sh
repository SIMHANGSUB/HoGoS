#! /bin/bash

qemu-system-i386 -m 64 -fda final.img -M pc -hda hdd.img
