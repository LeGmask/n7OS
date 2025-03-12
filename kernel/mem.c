#include <n7OS/mem.h>
#include <stdio.h>

uint32_t free_page_bitmap_table[BITMAP_ARRAY_SIZE];

/**
 * @brief Marque la page allouée
 *
 * Lorsque la page a été choisie, cette fonction permet de la marquer allouée
 *
 * @param addr Adresse de la page à allouer
 */
void setPage(uint32_t addr)
{
    uint32_t index = addr / PAGE_SIZE;
    uint32_t offset = index % 32;

    free_page_bitmap_table[index / 32] |= 1 << offset;
}

/**
 * @brief Désalloue la page
 *
 * Libère la page allouée.
 *
 * @param addr Adresse de la page à libérer
 */
void clearPage(uint32_t addr)
{
    uint32_t index = addr / PAGE_SIZE;
    uint32_t offset = index % 32;

    free_page_bitmap_table[index / 32] &= ~(1 << offset);
}

/**
 * @brief Fourni la première page libre de la mémoire physique tout en l'allouant
 *
 * @return uint32_t Adresse de la page sélectionnée
 */
uint32_t findfreePage()
{
    for (uint32_t page_idx = 0x0; page_idx < PAGE_COUNT; page_idx++)
    {
        uint32_t index = page_idx / 32;
        uint32_t offset = page_idx % 32;

        if (!(free_page_bitmap_table[index] & (1 << offset)))
        {
            setPage(page_idx * PAGE_SIZE);
            return page_idx * PAGE_SIZE;
        }
    }

    // an error occured, no free page found
    return 0x0;
}

/**
 * @brief Initialise le gestionnaire de mémoire physique
 */
void init_mem()
{
    for (int i = 0x0; i < BITMAP_ARRAY_SIZE; i++)
    {
        free_page_bitmap_table[i] = 0x0;
    }
}

/**
 * @brief Affiche l'état de la mémoire physique
 *
 */
void print_mem()
{
    for (int i = 0; i < BITMAP_ARRAY_SIZE; i++)
    {
        printf("0x%08x ", free_page_bitmap_table[i]);

        if ((i + 1) % 7 == 0)
            printf("\n");
    }
}