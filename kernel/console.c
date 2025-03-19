#include <n7OS/console.h>
#include <n7OS/cpu.h>
#include <stdio.h>

uint16_t *scr_tab;
int cursor;

void scrollup()
{
	for (int i = HEADER_SIZE * VGA_WIDTH; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++)
	{
		scr_tab[i] = scr_tab[i + VGA_WIDTH];
	}

	// we clear the last line
	for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		scr_tab[i] = CHAR_COLOR << 8 | ' ';
	}
}

void scrollup_if_needed()
{
	if (cursor >= VGA_WIDTH * VGA_HEIGHT)
	{
		scrollup();
		cursor = cursor - VGA_WIDTH;
	}
}

void seak_cursor(int count)
{
	cursor = cursor + count;
	scrollup_if_needed();
}

void seak_cursor_at(int column, int line)
{
	cursor = VGA_WIDTH * line + column;
	scrollup_if_needed();
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
	for (int i = HEADER_SIZE; i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		scr_tab[i] = CHAR_COLOR << 8 | ' ';
	}
	seak_cursor_at(0, HEADER_SIZE);
}

void update_cursor()
{
	outb(CMD_LOW, PORT_CMD);
	outb(cursor & 255, PORT_DATA);
	outb(CMD_HIGH, PORT_CMD);
	outb(cursor >> 8, PORT_DATA);
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
		seak_cursor(8);
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

void draw_header()
{
	for (int i = 0; i < VGA_WIDTH; i++)
	{
		scr_tab[i] = HEADER_COLOR << 8 | ' ';
	}
}

void update_clock(uint32_t timestamp)
{
	char header[15];
	sprintf(header, "n7OS - %02d:%02d:%02d", (timestamp / 3600) % 24, (timestamp / 60) % 60, timestamp % 60);
	for (int i = 0; i < 15; i++)
	{
		scr_tab[i] = HEADER_COLOR << 8 | header[i];
	}
}

void init_console()
{
	scr_tab = (uint16_t *)SCREEN_ADDR;
	cursor = 0;
	clean_screen();
	draw_header();
	update_clock(0);
	update_cursor();
}
