#!/bin/sh
#
# This file was automatically generated.
#
# It can be overwritten by nios2-flash-programmer-generate or nios2-flash-programmer-gui.
#

#
# Converting ELF File: /afs/ualberta.ca/home/p/k/pkuczera/Documents/ECE492_Projects/capstone/ece492_w16/niosII_gesture_detection/software/g7_gesture_detection_nv/g7_gesture_detection_nv.elf to: "../flash/g7_gesture_detection_nv_generic_tristate_controller_0.flash"
#
elf2flash --input="/afs/ualberta.ca/home/p/k/pkuczera/Documents/ECE492_Projects/capstone/ece492_w16/niosII_gesture_detection/software/g7_gesture_detection_nv/g7_gesture_detection_nv.elf" --output="../flash/g7_gesture_detection_nv_generic_tristate_controller_0.flash" --boot="$SOPC_KIT_NIOS2/components/altera_nios2/boot_loader_cfi.srec" --base=0x1800000 --end=0x1c00000 --reset=0x1800000 --verbose 

#
# Programming File: "../flash/g7_gesture_detection_nv_generic_tristate_controller_0.flash" To Device: generic_tristate_controller_0
#
nios2-flash-programmer "../flash/g7_gesture_detection_nv_generic_tristate_controller_0.flash" --base=0x1800000 --sidp=0x890C0 --id=0x0 --timestamp=1459975887 --device=1 --instance=0 '--cable=USB-Blaster on localhost [2-1.6]' --program --verbose 

