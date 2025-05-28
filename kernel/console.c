#include <n7OS/console.h>
#include <n7OS/cpu.h>
#include <stdio.h>

uint16_t *scr_tab;
int cursor;

/**
 * This function scrolls up the console
 * It moves all lines up by one, and clears the last line.
 * Called when we reach the end of the console.asm
 *
 * We take care to not scroll the header line, so we start at HEADER_SIZE * VGA_WIDTH
 * and stop at VGA_WIDTH * (VGA_HEIGHT - 1).
 */
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

/**
 * Trigger a scroll up if the cursor is at the end of the screen.
 */
void scrollup_if_needed()
{
	if (cursor >= VGA_WIDTH * VGA_HEIGHT)
	{
		scrollup();
		cursor = cursor - VGA_WIDTH;
	}
}

/**
 * Move the cursor by a certain number of characters.
 * If the cursor goes beyond the end of the screen, it will scroll up.
 * @param count The number of characters to move the cursor.
 */
void seak_cursor(int count)
{
	cursor = cursor + count;
	scrollup_if_needed();
}

/**
 * Move the cursor to a specific column and line.
 * If the cursor goes beyond the end of the screen, it will scroll up.
 * @param column The column to move the cursor to.
 * @param line The line to move the cursor to.
 */
void seak_cursor_at(int column, int line)
{
	cursor = VGA_WIDTH * line + column;
	scrollup_if_needed();
}

/**
 * Get the current cursor line and column.
 * @return The current cursor line.
 */
int get_cursor_line()
{
	return cursor / VGA_WIDTH;
}

/**
 * Get the current cursor column.
 * @return The current cursor column.
 */
int get_cursor_column()
{
	return cursor % VGA_WIDTH;
}

/**
 * Clean the screen by filling it with spaces and resetting the cursor to the header line.
 */
void clean_screen()
{
	for (int i = HEADER_SIZE * VGA_WIDTH; i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		scr_tab[i] = CHAR_COLOR << 8 | ' ';
	}
	seak_cursor_at(0, HEADER_SIZE);
}

/**
 * Update the hardware cursor position.
 * This function writes the cursor position to the VGA controller.
 */
void update_cursor()
{
	outb(CMD_LOW, PORT_CMD);
	outb(cursor & 255, PORT_DATA);
	outb(CMD_HIGH, PORT_CMD);
	outb(cursor >> 8, PORT_DATA);
}

/**
 * This function writes a character to the console.
 * It handles special characters like newline, carriage return, tab, form feed, and backspace.
 * For other characters, it writes them to the screen and updates the cursor position.
 * @param c The character to write to the console.
 */
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

/**
 * This function writes a string to the console.
 * It calls console_putchar for each character in the string.
 * @param s The string to write to the console.
 * @param len The length of the string.
 */
void console_putbytes(const char *s, int len)
{
	for (int i = 0; i < len; i++)
	{
		console_putchar(s[i]);
	}
}

/**
 * Draw the header line at the top of the console.
 * It fills the first line with spaces and sets the color to HEADER_COLOR.
 */
void draw_header()
{
	for (int i = 0; i < VGA_WIDTH; i++)
	{
		scr_tab[i] = HEADER_COLOR << 8 | ' ';
	}
}

/**
 * Update the header content with the updated time.
 */
void update_clock(uint32_t timestamp)
{
	char header[15];
	sprintf(header, "n7OS - %02d:%02d:%02d", (timestamp / 3600) % 24, (timestamp / 60) % 60, timestamp % 60);
	for (int i = 0; i < 15; i++)
	{
		scr_tab[i] = HEADER_COLOR << 8 | header[i];
	}
}

/**
 * Setup the console.asm
 * It initializes the screen pointer, sets the cursor to 0,
 * clears the screen, draws the header, and updates the cursor.
 */
void init_console()
{
	scr_tab = (uint16_t *)SCREEN_ADDR;
	cursor = 0;
	clean_screen();
	draw_header();
	update_cursor();
}
