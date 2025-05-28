#include <inttypes.h>
#include <n7OS/irq.h>
#include <n7OS/processor_structs.h>

/**
 * Crée une entrée dans la table d'interruption (IDT) pour un IRQ spécifique.
 * @param irq_num Le numéro de l'IRQ pour lequel l'entrée est créée.
 * @param addr L'adresse de la fonction de gestion de l'IRQ.
 */
void init_irq_entry(int irq_num, uint32_t addr)
{
	idt_entry_t *entry = (idt_entry_t *)&idt[irq_num];

	entry->zeros = 0x0;
	entry->offset_inf = addr & 0xffff;
	entry->offset_sup = addr >> 16;
	entry->sel_segment = KERNEL_CS;
	entry->type_attr = PRESENT | DPL_HIGH | INT_GATE | TYPE_INT32_GATE;
}
