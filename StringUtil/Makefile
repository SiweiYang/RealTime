include ../Makefile.inc

PATH := ../../gcc/arm-elf/bin:../../gcc/libexec/gcc/arm-elf/4.0.2:$(PATH)
CFLAGS  = -c -fPIC -Wall -I. -I.. -I../include -mcpu=arm920t -msoft-float $(OPTMZFLAGS)

all:  StringUtil.o

# test: StringUtil.c ../include/StringUtil.h
# 	g++ -I. -I.. -I../include -DTEST StringUtil.c test.c -o test

StringUtil.s: StringUtil.c ../include/StringUtil.h
	$(XCC) -S $(CFLAGS) StringUtil.c

StringUtil.o: StringUtil.s
	$(AS) $(ASFLAGS) -o StringUtil.o StringUtil.s

clean:
	-rm -f *.s *.o test
