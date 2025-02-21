#include <n7OS/console.h>
#include <n7OS/cpu.h>

uint16_t *scr_tab;
int cursor;

void init_console()
{
    scr_tab = (uint16_t *)SCREEN_ADDR;
    cursor = 0;
}

void seak_cursor(int count)
{
    cursor = (cursor + count) % (80 * VGA_HEIGHT + VGA_WIDTH);
}

void seak_cursor_at(int column, int line)
{
    cursor = VGA_WIDTH * line + column;
}

int get_cursor_line()
{
    return cursor / VGA_WIDTH;
}

int get_cursor_column()
{
    return cursor % VGA_WIDTH;
}

void clean_screen()
{
    for (int i = 0; i < 80 * VGA_HEIGHT + VGA_WIDTH; i++)
    {
        scr_tab[i] = CHAR_COLOR << 8 | ' ';
    }
    
    seak_cursor_at(0, 0);
}

void update_cursor()
{
    outb(0xF, 0x3D4);
    outb(cursor & 255, 0x3D5);
    outb(0xE, 0x3D4);
    outb(cursor << 8, 0x3D5);
}

void console_putchar(const char c)
{
    switch (c)
    {
    case '\n':
        seak_cursor_at(0, get_cursor_line() + 1);
        break;
    case '\r':
        seak_cursor_at(0, get_cursor_line());
        break;
    case '\t':
        console_putbytes("       ", 8);
        break;
    case '\f':
        clean_screen();
        break;
    case '\b':
        seak_cursor(-1);
        scr_tab[cursor] = CHAR_COLOR << 8 | ' ';
        break;
    default:
        if (c > 31 && c < 127)
        {
            scr_tab[cursor] = CHAR_COLOR << 8 | c;
            seak_cursor(1);
        }
        break;
    }

    update_cursor();
}

void console_putbytes(const char *s, int len)
{
    for (int i = 0; i < len; i++)
    {
        console_putchar(s[i]);
    }
}