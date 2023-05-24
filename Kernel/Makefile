include Makefile.inc


MEMM=-D BUDDY_MM # -D BITMAP_MM o -D BUDDY_MM
KERNEL=kernel.bin

SOURCES=$(wildcard *.c) $(wildcard */*.c)
SOURCES_ASM=$(wildcard asm/*.asm)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

INCLUDE_DIRS := ./*/include ./include
INCLUDES_DECLARATION := $(foreach dir,$(wildcard $(INCLUDE_DIRS)),-I$(dir)) # pone todos los includes en una variable en forma de -I

LOADERSRC=loader.asm
LOADEROBJECT=$(LOADERSRC:.asm=.o)


all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS)  $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) -o $(KERNEL)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) -o kernel.elf
%.o: %.c
	$(GCC) $(GCCFLAGS) $(MEMM) $(INCLUDES_DECLARATION) -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf *.o *.bin *.elf ./*/*.o
.PHONY: all clean
