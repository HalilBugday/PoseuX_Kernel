#!/bin/bash
sudo mkdir /media/floppy
sudo mount /dev/fd0 /media/floppy
sudo cp src/kernel /media/floppy/kernel
sudo umount /media/floppy
sudo rm -rf /media/floppy
