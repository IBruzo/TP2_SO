// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <idtLoader.h>

#pragma pack(push) /* Push de la alineación actual */
#pragma pack(1)    /* Alinear las siguiente estructuras a 1 byte */

/* Descriptor de interrupcion */
typedef struct
{
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop) /* Reestablece la alinceación actual */

DESCR_INT *idt = (DESCR_INT *)0; // IDT de 255 entradas

static void setup_IDT_entry(int index, uint64_t offset);

/**
 * @brief carga las interrupciones y excepciones en la tabla de idt
 * las interrupciones arrancan en el 20 con timertick
 *
 */
void load_idt()
{

  setup_IDT_entry(0x20, (uint64_t)&_irq00Handler);
  setup_IDT_entry(0x00, (uint64_t)&_exception0Handler);
  setup_IDT_entry(0x06, (uint64_t)&_exception6Handler);
  setup_IDT_entry(0x21, (uint64_t)&_irq01Handler);
  setup_IDT_entry(0x80, (uint64_t)&_irq60Handler); // syscalls

  initialize();

  // Solo interrupcion timer tick  y teclado habilitadas /fe
  picMasterMask(0xFC); // fc para habilitar la de teclado
  picSlaveMask(0xFF);
}

static void setup_IDT_entry(int index, uint64_t offset)
{
  idt[index].offset_l = (uint16_t)(offset & 0xFFFF);
  idt[index].selector = (uint16_t)0x08;
  idt[index].offset_m = (uint16_t)(offset >> 16 & 0xFFFF);
  idt[index].offset_h = (uint32_t)(offset >> 32 & 0xFFFFFFFF);
  idt[index].access = (uint8_t)ACS_INT;
  idt[index].cero = (uint8_t)0;
  idt[index].other_cero = (uint32_t)0;
}
