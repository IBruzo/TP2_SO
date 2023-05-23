include ../Makefile.inc

USER=0000-sampleCodeModule.bin

SOURCES=$(wildcard [^_]*.c) $(wildcard */*.c)
SOURCES_ASM=$(wildcard asm/*.asm)

OBJECTS_ASM=$(SOURCES_ASM:asm/%.asm=obj/%.asm.o)


INCLUDE_DIRS := ./*/include ./include
INCLUDES_DECLARATION := $(foreach dir,$(wildcard $(INCLUDE_DIRS)),-I$(dir)) # pone todos los includes en una variable en forma de -I

all: $(USER)

$(USER): $(SOURCES) $(OBJECTS_ASM) 
	$(GCC) $(GCCFLAGS) $(INCLUDES_DECLARATION) -T sampleCodeModule.ld _loader.c $(OBJECTS_ASM) $(SOURCES) -o ../$(USER)
	$(GCC) $(GCCFLAGS) $(INCLUDES_DECLARATION) -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 _loader.c $(OBJECTS_ASM) $(SOURCES) -o ../0000-sampleCodeModule.elf
obj/%.asm.o : asm/%.asm
	mkdir -p obj
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o
	rm -rf obj
	rm -rf */*.o
.PHONY: all clean
