#
# Copyright (c) 1995, 3Dfx Interactive, Inc.
# All Rights Reserved.
#
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of 3Dfx Interactive, Inc.;
# the contents of this file may not be disclosed to third parties, copied or
# duplicated in any form, in whole or in part, without the prior written
# permission of 3Dfx Interactive, Inc.
#
# RESTRICTED RIGHTS LEGEND:
# Use, duplication or disclosure by the Government is subject to restrictions
# as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
# and Computer Software clause at DFARS 252.227-7013, and/or in similar or
# successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished  -
# rights reserved under the Copyright Laws of the United States.
#

# Override environment variables you may have set for older kits
BUILD_ROOT        = ..
BUILD_ROOT_SWLIBS = $(BUILD_ROOT)
BUILD_ROOT_SST1   = $(BUILD_ROOT)

# FX_COMPILER:      WATCOM or MICROSOFT are fully supported.
#                   There are partial hooks for DJGPP, GCC, and BORLAND.
FX_COMPILER       = MICROSOFT

# FX_TARGET:        Only WIN32 is supported for Glide3x
#                   DOS is possible with Glide2x
FX_TARGET         = WIN32

LDIRT= *.exe *.map *.sys *.obj *.lib

LCINCS  = $(LCINCS)  -I$(BUILD_ROOT)\include -WX

!if "$(FX_COMPILER)"=="WATCOM"
LIBOBJS = tlib.lib
!else
LIBOBJS = tlib.obj
!endif

GLIDELIB = glide3x.lib

LLDLIBS = $(LIBOBJS) $(BUILD_ROOT)\lib\Win32\$(GLIDELIB)\
          $(BUILD_ROOT_SWLIBS)\lib\win32\fxmisc.lib

PRIVATE_HEADERS = tlib.h tlib.c tldata.inc

CFILES   = test00.c \
           test01.c \
           test02.c \
           test03.c \
           test04.c \
           test05.c \
           test06.c \
           test07.c \
           test08.c \
           test09.c \
           test10.c \
           test11.c \
           test12.c \
           test13.c \
           test14.c \
           test15.c \
           test16.c \
           test17.c \
           test18.c \
           test19.c \
           test20.c \
           test21.c \
           test22.c \
           test23.c \
           test24.c \
           test25.c \
           test26.c \
           test27.c \
           test28.c \
           test29.c \
           test30.c \
           test31.c \
           test32.c \
           test33.c \
           test34.c \
           test35.c \
           test36.c \
           display.c \
           sbench.c

PROGRAMS = $(CFILES:.c=.exe)

DATAFILES = alpha.3df decal1.3df lava.3df light.3df matt1.3df miro.3df

!include $(BUILD_ROOT_SWLIBS)/include/nmake/3dfx.mak

$(PROGRAMS):  $(LLDLIBS)

!if "$(FX_COMPILER)"=="WATCOM"
!if "$(FX_TARGET)"=="DOS"
tlib.lib: tlib.obj
        wlib -b -c -n -q -p=512 tlib tlib.obj
!else
tlib.lib: tlib.obj
        wlib -b -c -n -q -p=512 tlib tlib.obj
!endif
!endif

test00: test00.exe
test01: test01.exe
test02: test02.exe
test03: test03.exe
test04: test04.exe
test05: test05.exe
test06: test06.exe
test07: test07.exe
test08: test08.exe
test09: test09.exe
test10: test10.exe
test11: test11.exe
test12: test12.exe
test13: test13.exe
test14: test14.exe
test15: test15.exe
test16: test16.exe
test17: test17.exe
test18: test18.exe
test19: test19.exe
test20: test20.exe
test21: test21.exe
test22: test22.exe
test23: test23.exe
test24: test24.exe
test25: test25.exe
test26: test26.exe
test27: test27.exe
test28: test28.exe
test29: test29.exe
test30: test30.exe
test31: test31.exe
test32: test32.exe
test33: test33.exe
test34: test34.exe
test35: test35.exe
test36: test36.exe
sbench: sbench.exe
display: display.exe
        
