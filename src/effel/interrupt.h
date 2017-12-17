#ifndef _INTERRUPT_H
#define _INTERRUPT_H 1

void irq_enable(uint8_t line);
void irq_disable(uint8_t line);

void interrupt_register(uint8_t slot, void* isr);
void interrupt_init();

#endif
