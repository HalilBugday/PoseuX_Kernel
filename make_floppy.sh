#!/bin/bash

sudo /sbin/losetup /dev/loop51 floppy.img
sudo mount /dev/loop51 /mnt
sudo cp src/kernel /mnt/kernel
sudo umount /dev/loop51
sudo /sbin/losetup -d /dev/loop51
