#include <n7OS/paging.h>
#include <n7OS/kheap.h>
#include <n7OS/mem.h>
#include <n7OS/processor_structs.h>
#include <stddef.h> // nécessaire pour NULL
#include <stdio.h>

PageDirectory directory;

/**
 * Setup paging by initializing the page directory and allocating pages.
 * This function sets up the paging mechanism in the kernel, allowing for
 * memory management through paging.
 */
void init_paging()
{
	init_mem();
	directory = (PageDirectory)kmalloc_a(PAGE_SIZE);

	for (uint32_t idx = 0; idx < PAGE_SIZE / sizeof(PDE); idx++)
	{
		directory[idx].page = kmalloc_a(PAGE_SIZE) >> 12;
		directory[idx].P = PRESENT;
		directory[idx].RW = WRITABLE;
		directory[idx].SU = !USERLAND;
	}

	for (uint32_t i = 0; i < get_stack_top(); i += PAGE_SIZE)
	{
		alloc_page_entry(i, 1, 1);
	}

	__asm__ __volatile__("mov %0, %%cr3" ::"r"(directory)); // set %cr3 with the directory addr

	// On active la pagination
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	__asm__ __volatile__("mov %0, %%cr0" ::"r"(cr0));

	setup_base((uint32_t)directory);
}

/**
 * Allocates a page entry in the page table for a given address.
 * This function maps a virtual address to a physical page, allowing
 * the kernel to access that page with the specified permissions.
 * 
 * @param address       The virtual address to map.
 * @param is_writeable  If true, the page is writable; otherwise, it is read-only.
 * @param is_kernel     If true, the page is accessible only by the kernel; otherwise, it is accessible by userland.
 * @return PageTable    The page table that contains the allocated page entry.
 */
PageTable alloc_page_entry(uint32_t address, int is_writeable, int is_kernel)
{
	Virtual_addr virtual_addr;
	virtual_addr.value = address;

	PDE pde = directory[virtual_addr.directory_idx];
	PageTable pt = (PageTable)(pde.page << 12);
	PTE *pte = &pt[virtual_addr.table_idx];

	pte->P = PRESENT;
	pte->RW = is_writeable;
	pte->SU = ~is_kernel;
	pte->page = findfreePage() >> 12;

	return pt;
}
