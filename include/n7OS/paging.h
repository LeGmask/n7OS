/**
 * @file paging.h
 * @brief Gestion de la pagination dans le noyau
 */
#ifndef _PAGING_H
#define _PAGING_H

#include <inttypes.h>

/**
 * @brief La page est présente en mémoire
 */
#define PRESENT 0x1

/**
 * @brief La page est accessible en écriture
 */
#define WRITABLE 0x1

/**
 * @brief La page est en mode utilisateur
 */
#define USERLAND 0x1

/**
 * @brief Une entrée dans le répertoire de page peut être manipulée en utilisant
 *        la structure ou directement la valeur
 */
typedef union
{
	struct
	{
		uint32_t P : 1;
		uint32_t RW : 1;
		uint32_t SU : 1;
		uint32_t RSVD : 9;
		uint32_t page : 20;
	};
	uint32_t value;
} PDE; // PDE = Page Directory Entry

/**
 * @brief Un répertoire de page (PageDirectory) est un tableau de descripteurs de page
 */
typedef PDE *PageDirectory;

/**
 * @brief Une entrée dans la table de page peut être manipulée en utilisant
 *        la structure ou directement la valeur
 */
typedef union
{
	struct
	{
		uint32_t P : 1;
		uint32_t RW : 1;
		uint32_t SU : 1;
		uint32_t RSVD1 : 2;
		uint32_t A : 1;
		uint32_t D : 1;
		uint32_t RSVD2 : 2;
		uint32_t AVL : 3;
		uint32_t page : 20;
	};
	uint32_t value;
} PTE; // PTE = Page Table Entry

/**
 * @brief Une table de page (PageTable) est un tableau de descripteurs de page
 */
typedef PTE *PageTable;

typedef union
{
	struct
	{
		uint32_t page_idx : 12;
		uint32_t table_idx : 10;
		uint32_t directory_idx : 10;
	};
	uint32_t value;
} Virtual_addr;

/**
 * @brief Cette fonction initialise le répertoire de page, alloue les pages de table du noyau
 *        et active la pagination
 */
void init_paging();

/**
 * @brief Cette fonction alloue une page de la mémoire physique à une adresse de la mémoire virtuelle
 *
 * @param address       Adresse de la mémoire virtuelle à mapper
 * @param is_writeable  Si is_writeable == 1, la page est accessible en écriture
 * @param is_kernel     Si is_kernel == 1, la page ne peut être accédée que par le noyau
 * @return PageTable    La table de page modifiée
 */
PageTable alloc_page_entry(uint32_t address, int is_writeable, int is_kernel);
#endif
