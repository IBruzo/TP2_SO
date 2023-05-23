include Makefile.inc


MEMM=-D BUDDY_MM # -D BITMAP_MM o -D BUDDY_MM
KERNEL=kernel.bin
SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_INTERRUPTIONS=$(wildcard interruptions/*.c)
SOURCES_MEMORY=$(wildcard memory/*.c)
SOURCES_LIBS=$(wildcard libs/*.c)
SOURCES_SCHEDULER=$(wildcard scheduler/*.c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_INTERRUPTIONS=$(SOURCES_INTERRUPTIONS:.c=.o)
OBJECTS_MEMORY=$(SOURCES_MEMORY:.c=.o)
OBJECTS_LIBS=$(SOURCES_LIBS:.c=.o)
OBJECTS_SCHEDULER=$(SOURCES_SCHEDULER:.c=.o)

LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_MEMORY) $(OBJECTS_LIBS) $(OBJECTS_SCHEDULER)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_MEMORY) $(OBJECTS_LIBS) $(OBJECTS_SCHEDULER) $(STATICLIBS) 
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_DRIVERS) $(OBJECTS_INTERRUPTIONS) $(OBJECTS_MEMORY) $(OBJECTS_LIBS) $(OBJECTS_SCHEDULER) $(STATICLIBS)
%.o: %.c
	$(GCC) $(GCCFLAGS) $(MEMM) -I./include -I ./drivers/include -I./interruptions/include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin drivers/*.o interruptions/*.o memory/*.o libs/*.o *.elf

.PHONY: all clean
