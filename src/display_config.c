#include "display_config.h"
#include "yaar_state.h"
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_display() 
{
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(17, GPIO_FUNC_I2C);
    gpio_set_function(16, GPIO_FUNC_I2C);
    gpio_pull_up(17);
    gpio_pull_up(16);

    uint8_t cmds[22] = 
    {
        0x00,           // control byte: <COMMAND>
        0xAE,           // set display off

        0xA8, 0x3F,     // set mux ratio to 63 + 1
        0xD3, 0x00,     // set display offset
        0x40,           // set display start line to 0
        0xA1,           // set segment remap to NO
        0xC8,           // set com direction 0 -> 63 (mux ratio)
        0xDA, 0x12,     // com pins hardware config (seq, no-remap)
        0x81, 0x7F,     // set contrast contral to 127
        0x20, 0x00,     // set mem_addressing_mode, horizontal addressing
        0xA4,           // entire display off, display whats on RAM
        0xA6,           // set !inverted display
        0xD5, 0xF0,     // set oscillator
        0x8D, 0x14,     // set charge pump

        0xAF,           // set display on
                        // 0xA5,           // entire display off, display whats on RAM
    };

    int res = i2c_write_blocking(&i2c0_inst, 0x3C, cmds, 22, false);
    // if (res < 0) printf("no byte wrote!!!\n"); 
    // else printf("%i bytes wrote for init\n", res);

    for (int i = 1; i < BUF_LEN; ++i) 
        display_buf[i] = 0xFF;

    render_display();
    sleep_ms(200);

    for (int page = 0; page < 8; ++page) 
    {
        for (int i = 0; i < 129; ++i) 
        {
            int index = (page * 128 + i);
            if (index == 0)
                continue;
            display_buf[index] = 0x00;
        };
        render_display();

        sleep_ms(10);
    };

};

void build_ui()
{
    memset(display_buf + 1, 0, BUF_LEN - 1);

    // STATUS BAR
    draw_line((Line) { .x1 = 0, .y1 = DISPLAY_HEIGHT - 10, .x2 = DISPLAY_WIDTH, .y2 = DISPLAY_HEIGHT - 10 }, 1);
    draw_line((Line) { .x1 = 0, .y1 = DISPLAY_HEIGHT - 11, .x2 = g_state.threshold_meter, .y2 = DISPLAY_HEIGHT - 11 }, 1);

    // MODE
    if (g_state.mode == NORMAL)
        draw_char(DISPLAY_WIDTH - 8, DISPLAY_HEIGHT - 7, 'N', 1);
    else if (g_state.mode == INSERT)
        draw_char(DISPLAY_WIDTH - 8, DISPLAY_HEIGHT - 7, 'I', 1);
    else if (g_state.mode == VISUAL)
        draw_char(DISPLAY_WIDTH - 8, DISPLAY_HEIGHT - 7, 'V', 1);

    // LIST OF FILES
    int y = 1;
    for (int i = g_state.f_ls_range[0]; (i < g_state.total_ls && i <= g_state.f_ls_range[1]); ++i) 
    {
        draw_text(0, y, g_state.display_contents[i].data, 1);
        y += 10;
    };

    draw_text(0,  DISPLAY_HEIGHT - 7, g_state.ip_box_buf.data, 1);
    draw_text(90, DISPLAY_HEIGHT - 7, g_state.morse_buf.data, 1);

    // DRAW CURSOR
    if (g_state.mode == NORMAL) 
    {
        invert_rect((Rect) { .x = g_state.ip_box_cursor * 6, .y = DISPLAY_HEIGHT - 8, .w = 6, .h = 8 });
    }
    else if (g_state.mode == INSERT) 
    {
        // draw_line((Line) { .x1 = g_state.ip_box_cursor * 6, .y1 = DISPLAY_HEIGHT - 8, .x2 = g_state.ip_box_cursor * 6, .y2 = DISPLAY_HEIGHT }, 1);
        invert_rect((Rect) { .x = g_state.ip_box_cursor * 6, .y = DISPLAY_HEIGHT - 8, .w = 6, .h = 8 });
    }
    else if (g_state.mode == VISUAL) 
    {
        invert_rect((Rect) { .x = 0, .y =  (g_state.file_cursor - g_state.f_ls_range[0]) * 10, .w = DISPLAY_WIDTH, .h = 10 });
    }

    return;
};

void render_display() 
{
    uint8_t draw_cmds[] = 
    {
        0x00,               // control byte: <COMMAND>
        0x21, 0x00, 0x7F,   // set col  address, 
        0x22, 0x00, 0x07,   // set page address, 
    };
    int res = i2c_write_blocking(i2c_default, 0x3C, draw_cmds, 7, false);
    // if (res < 0) printf("no byte wrote!!!\n"); 
    // else printf("%i bytes wrote for draw_commands\n", res);

    res = i2c_write_blocking(i2c_default, 0x3C, display_buf, 1025, false);
    // if (res < 0) printf("no byte wrote!!!\n"); 
    // else printf("%i bytes wrote for display_buffer\n", res);
}

static inline void draw_pixel(u8 x, u8 y, u8 color) {
    ++x;
    if (x < 0 || x >= 128 || y < 0 || y >= 64) return;

    int index = x + (y / 8) * 128;
    u8 bit   = y % 8;

    if (color) display_buf[index] |= (1 << bit);
    else display_buf[index] &= ~(1 << bit);
}

static inline void invert_pixel(u8 x, u8 y) {
    ++x;
    if (x < 0 || x >= 128 || y < 0 || y >= 64) return;

    int index = x + (y / 8) * 128;
    u8 bit   = y % 8;

    if (display_buf[index] & (1<< bit)) 
        display_buf[index] &= ~(1 << bit);
    else 
        display_buf[index] |= (1 << bit);
}

void __draw_outline_rect(u8 x, u8 y, u8 w, u8 h, u8 color) {
    for (u8 i = x; i < x + w; i++) {
        draw_pixel(i, y, color);
        draw_pixel(i, y + h - 1, color);
    }

    for (u8 j = y; j < y + h; j++) {
        draw_pixel(x, j, color);
        draw_pixel(x + w - 1, j, color);
    }
}

void draw_filled_rect(Rect rect, u8 color) 
{
    __draw_filled_rect(rect.x, rect.y, rect.w, rect.h, color);
};

void __draw_filled_rect(u8 x, u8 y, u8 w, u8 h, u8 color) {
    for (u8 j = y; j < y + h; j++) {
        for (u8 i = x; i < x + w; i++) {
            draw_pixel(i, j, color);
        }
    }
}

void draw_outline_rect(Rect rect, u8 color) 
{
    __draw_outline_rect(rect.x, rect.y, rect.w, rect.h, color);
};

void draw_char(u8 x, u8 y, char c, bool is_block) 
{

    size_t index = c * 5;
     for (int col = 0; col < 5; col++) {
        uint8_t line = font[index + col];

        for (int row = 0; row < 8; row++) {
            if (line & (1 << row)) {
                draw_pixel(x + col, y + row, is_block);
            }
        }
    }
}

void draw_text(u8 x, u8 y, const char *str, bool is_block) {
    // printf("str: %s\n", str);
    while (*str) 
    {
        draw_char(x, y, *str++, is_block);
        x += 6; // 5 pixels per character
        if (x > 110)
            break;
    }
}

void __invert_rect(u8 x, u8 y, u8 w, u8 h) 
{
    for (u8 j = y; j < y + h; j++) {
        for (u8 i = x; i < x + w; i++) {
            invert_pixel(i, j);
        }
    }
};

void invert_rect(Rect rect) 
{
    __invert_rect(rect.x, rect.y, rect.w, rect.h);
};

void draw_line(Line l, bool color) 
{
    int dx = abs((int)l.x2 - (int)l.x1);
    int dy = abs((int)l.y2 - (int)l.y1);
    int sx = l.x1 < l.x2 ? 1 : -1;
    int sy = l.y1 < l.y2 ? 1 : -1;
    int err = dx - dy;
    
    u8 x = l.x1;
    u8 y = l.y1;
    
    while (1) {
        draw_pixel(x, y, (u8)color);
        
        if (x == l.x2 && y == l.y2) {
            break;
        }
        
        int e2 = 2 * err;
        
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}
