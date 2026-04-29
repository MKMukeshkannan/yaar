#pragma once

#include <pico/types.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
typedef int8_t  i8;

typedef struct 
{
    u8 len;
    u8 cap;
    char* data;
} YaarString;
void ys_push(YaarString* string, const char c);
void ys_insert(YaarString* string, const char c, u8 pos);
void ys_clear(YaarString* string);
bool ys_delete(YaarString* s, u8 pos);
void ys_free(YaarString* s);
void ys_from_str(YaarString* s, const char* c);


typedef enum { NORMAL, INSERT, VISUAL } YaarMode;
typedef struct 
{
    u8              ip_box_cursor;
    bool            rerender;
    u8              threshold_meter;
    size_t          file_cursor;
    size_t          f_ls_range[2]; // FILE LIST RANGE, [0, 4]
    size_t          total_ls;

    // SOL INPUT STATE
    // 00 00 00 00 -> mode(prev, curr) | play(prev, curr) | prev(prev, curr) | next(prev, curr) 
    u8              btn_state;
    YaarMode        mode;
    YaarString      ip_box_buf;
    YaarString      morse_buf;
    YaarString      dir_contents[20];

    absolute_time_t modebt_prev_active;
    absolute_time_t playbt_prev_active;
    absolute_time_t nextbt_prev_active;
    absolute_time_t prevbt_prev_active;
} YaarState;
void fill_state_input(absolute_time_t current_time);
char decode_morse(YaarString s);


extern char m_buf[5];
extern char ip_buf[20];
extern YaarState g_state;

// BTN _ STATE
// Masks (2 bits each)
#define MODE_MASK   (0x3 << 6)
#define PLAY_MASK   (0x3 << 4)
#define PREV_MASK   (0x3 << 2)
#define NEXT_MASK   (0x3 << 0)

// Set macros (value must be 0–3)
#define SET_MODE(state, val) \
    do { (state) = ((state) & ~MODE_MASK) | (((val) & 0x3) << 6); } while(0)

#define SET_PLAY(state, val) \
    do { (state) = ((state) & ~PLAY_MASK) | (((val) & 0x3) << 4); } while(0)

#define SET_PREV(state, val) \
    do { (state) = ((state) & ~PREV_MASK) | (((val) & 0x3) << 2); } while(0)

#define SET_NEXT(state, val) \
    do { (state) = ((state) & ~NEXT_MASK) | (((val) & 0x3) << 0); } while(0)

// Get macros
#define GET_MODE(state)   (((state) >> 6) & 0x3)
#define GET_PLAY(state)   (((state) >> 4) & 0x3)
#define GET_PREV(state)   (((state) >> 2) & 0x3)
#define GET_NEXT(state)   (((state) >> 0) & 0x3)

// Clear (unset → set to 0)
#define CLR_MODE(state)   ((state) &= ~MODE_MASK)
#define CLR_PLAY(state)   ((state) &= ~PLAY_MASK)
#define CLR_PREV(state)   ((state) &= ~PREV_MASK)
#define CLR_NEXT(state)   ((state) &= ~NEXT_MASK)

#define LONG_PRESS_THRESHOLD    (350000)

#define MODE_BUTTON             (0)
#define PREV_BUTTON             (1)
#define NEXT_BUTTON             (2)
#define PLAY_BUTTON             (3)

