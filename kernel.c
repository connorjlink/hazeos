#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "kernel.h"
#include "utils.h"
#include "video.h"
#include "power.h"
#include "graphics.h"
#include "vga.h"
#include "defs.h"

static uint8_t compose_color(VGAPalette palette)
{
	return (palette.bg << 4) | palette.fg;
}

static uint8_t invert_color(uint8_t color)
{
	return ((color & 0x0F) << 4) | ((color & 0xF0) >> 4);
}

static uint16_t compose_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t)color << 8 | (uint16_t)uc;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static uint16_t* terminal_buffer = (uint16_t*)0xB8000;



void terminal_initialize(void) 
{
	_active_color = compose_color(PALETTE(VGA_COLOR_BLUE, VGA_COLOR_LIGHT_GREY));
	
	for (size_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = compose_entry(' ', _active_color);
		}
	}
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = (y * VGA_WIDTH) + x;
	terminal_buffer[index] = compose_entry(c, color);
}

void terminal_putchar(char c, size_t* x, size_t* y) 
{
	switch (c)
	{
		case '\n': 
			(*x) = 0;
			(*y)++;
			return;
			
		case '\r':
			(*x) = 0;
			return;

		case '\0':
			return;
	}

	terminal_putentryat(c, _active_color, *x, *y);

	(*x)++;

	if ((*x) == VGA_WIDTH) 
	{
		(*x) = 0;

		(*y)++;

		if ((*y) == VGA_HEIGHT)
		{
			(*y) = 0;
		}
	}
}

void terminal_write(const char* data, size_t size, size_t x, size_t y) 
{
	for (size_t i = 0; i < size; i++)
    {
		terminal_putchar(data[i], &x, &y);
    }
}

void terminal_writestring(const char* data, size_t x, size_t y) 
{
	terminal_write(data, strlen(data), x, y);
}

void render_text_center(const char* data, size_t x0, size_t x1, size_t y)
{
	const size_t length = strlen(data);
	const size_t midpoint = (x0 + x1) / 2;
	
	const size_t begin = midpoint - (length / 2);
	terminal_write(data, length, begin, y);
}

void render_menubar()
{
	const size_t header_row = 0;
	const size_t copyright_row = VGA_HEIGHT - 1;

	_active_color = invert_color(_active_color);

	for (size_t x = 0; x < VGA_WIDTH; x++)
	{
		terminal_putentryat(' ', _active_color, x, header_row);
		terminal_putentryat(' ', _active_color, x, copyright_row);
	}

	render_text_center("Configuration", 0, VGA_WIDTH, header_row);
	render_text_center("HazeOS - (c) Connor J. Link. All Rights Reserved.", 0, VGA_WIDTH, copyright_row);

	_active_color = invert_color(_active_color);
}

void render_groupbox(Rect rect, uint8_t color, const char* title, bool is_selected)
{
	const size_t left = rect.pos.x;
	const size_t right = rect.pos.x + rect.size.x;

	const size_t top = rect.pos.y;
	const size_t bottom = rect.pos.y + rect.size.y;

	static const char horizontal_line = '\xC4';
	static const char vertical_line = '\xB3';

	static const char top_left_corner = '\xDA';
	static const char top_right_corner = '\xBF';
	static const char bottom_left_corner = '\xC0';
	static const char bottom_right_corner = '\xD9';

	// draw top line
	for (size_t x = left; x <= right; x++)
	{
		terminal_putentryat(horizontal_line, color, x, top);
	}

	// draw bottom line
	for (size_t x = left; x <= right; x++)
	{
		terminal_putentryat(horizontal_line, color, x, bottom);
	}

	// draw left line
	for (size_t y = top; y <= bottom; y++)
	{
		terminal_putentryat(vertical_line, color, left, y);
	}

	// draw right line
	for (size_t y = top; y <= bottom; y++)
	{
		terminal_putentryat(vertical_line, color, right, y);
	}

	// draw corners properly
	terminal_putentryat(top_left_corner, color, left, top);
	terminal_putentryat(top_right_corner, color, right, top);
	terminal_putentryat(bottom_left_corner, color, left, bottom);
	terminal_putentryat(bottom_right_corner, color, right, bottom);

	const size_t text_left = left + 2;
	const size_t text_right = right - 2;

	const size_t title_length = strlen(title);

	const size_t max_length = text_right - text_left;
	const int32_t difference = title_length - max_length;
	
	const size_t max_right = min(text_right, text_left + title_length - 1);

	if (is_selected) 
	{
		color = invert_color(color);
	}

	for (size_t x = text_left; x <= max_right; x++)
	{
		terminal_putentryat(title[x - text_left], color, x, top);
	}

	if (difference > 0)
	{
		// terminal_putentryat(ellipses, color, text_right, top);
	}
}

void render_text(Rect parent, Point pos, uint8_t color, const char* text)
{
	const size_t start_left = parent.pos.x + pos.x + 1;
	const size_t start_top = parent.pos.y + pos.y + 1;

	const size_t parent_right = parent.pos.x + parent.size.x - 1;

	const size_t text_length = strlen(text);

	size_t x = start_left;
	size_t y = start_top;

	for (size_t i = 0; i < text_length; i++)
	{
		char c = text[i];

		if (x == parent_right ||
			c == '\n')
		{
			y++;
			x = start_left;
		}

		terminal_putentryat(c, color, x, y);

		x++;
	}
}

void scroll_rect(Rect parent, uint8_t color)
{
	const size_t left = parent.pos.x + 1;
	const size_t right = parent.pos.x + parent.size.x;

	const size_t top = parent.pos.y + 1;
	const size_t bottom = parent.pos.y + parent.size.y - 1;

	for (size_t y = top; y < bottom; y++)
	{
		for (size_t x = left; x < right; x++)
		{
			terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
		}
	}

	for (size_t x = left; x < right; x++)
	{
		terminal_putentryat(' ', color, x, bottom);
	}
}

void write_console(Rect parent, Point* cursor, uint8_t color, const char* text)
{
	const size_t bottom = parent.size.y - 2;

	if (cursor->y == bottom)
	{
		scroll_rect(parent, color);
		//cursor->y--;
	}

	else
	{
		cursor->y++;
	}

	render_text(parent, *cursor, color, text);
}

void kernel_main(void) 
{
	terminal_initialize();
	render_menubar();
	//terminal_writestring("Hello, kernel World!\n", 1, 10);

	disable_cursor();

	size_t x = 1;
	size_t y = 17;

	render_groupbox(_explorer_rect, _active_color, "Explorer", false);
	render_groupbox(_console_rect, _active_color, "Console", false);
	render_groupbox(_navigator_rect, _active_color, "Navigator", false);

	//render_text(console_rect, string_point, color, "Hello, World!");

	while (1)
	{
		char c = poll_keyboard();
		if (c != '\0') 
		{
			if (c == 'q')
			{
				shutdown();
			}

			else if (c == 'a') 
			{
				write_console(_console_rect, &_console_cursor, _active_color, "Hello, Antoine!");
			}

			else if (c == 'c')
			{
				write_console(_console_rect, &_console_cursor, _active_color, "Hello, Connor!");
			}
	
			else 
			{
				terminal_putchar(c, &x, &y);
			}
		}
	}
}
