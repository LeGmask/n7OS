#include <n7OS/keyboard.h>
#include <n7OS/cpu.h>

uint8_t shift = 0;

uint8_t buffer[BUFFER_SIZE];
uint8_t head = 0;
uint8_t tail = 0;

void init_keyboard()
{
	outb(inb(0x21) & ~0x02, 0x21);
}

char kgetch()
{
	if (head == tail)
		return 0;

	char c = buffer[tail];
	tail = (tail + 1) % BUFFER_SIZE;
	return c;
}

void keyboard_IT_Handler()
{
	outb(0x20, 0x20); // ACK
	uint8_t key_code = inb(KEYB_ENCODER);

	if (key_code == SHIFT_PRESSED || key_code == SHIFT_RELEASED)
	{
		shift = SHIFT_PRESSED ? 1 : 0;
		return;
	}

	uint32_t c = shift ? scancode_map_shift[key_code] : scancode_map[key_code];

	if (key_code & 0x80) // Key released
		return;

	if (c == 0) // Do not buffer this modifier key
		return;

	int next = (head + 1) % BUFFER_SIZE;
	if(next != head)
	{
		buffer[head] = c;
		head = next;
	}
}
