#include "yaar_state.h"
#include <hardware/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ys_push(YaarString* s, const char c)
{
    if (s == NULL) return;
    if (s->len >= s->cap) return;

    s->data[s->len++] = c;
};

void ys_clear(YaarString* s) 
{
    if (s == NULL) return;

    s->len = 0;
    for (int i = 0; i < s->cap; ++i)
        s->data[i] = ' ';
};

void ys_insert(YaarString* s, const char c, u8 pos) 
{
    if (s == NULL) return;
    if (pos > s->len) return;

    for (u8 i = s->len; i > pos; --i)
        s->data[i] = s->data[i - 1];

    s->data[pos] = c;
    s->len++;
    g_state.ip_box_cursor++;
};

bool ys_delete(YaarString* s, u8 pos) 
{
    if (s == NULL) return false;
    if (pos == 0) return false;

    for (u8 i = pos - 1; i < s->len; ++i) 
        s->data[i] = s->data[i + 1];

    s->len--;
    return true;
};

void ys_free(YaarString* s) 
{
    if (s == NULL) return;
    if (s->data == NULL) return;

    free(s->data);
    s->len = 0;
    s->cap = 0;
    s->data = NULL;
};

void ys_from_str(YaarString* s, const char* c) 
{
    if (s == NULL || c == NULL) return;

    size_t len = strlen(c);
    s->len = len;
    s->cap = len + 1;
    s->data = malloc(len + 1);
    strcpy(s->data, c);
};

void ys_set(YaarString* s, const char* c) 
{
    if (s == NULL || c == NULL) return;

    size_t len = strlen(c);
    if (s->cap < len) return;

    s->len = len;
    strcpy(s->data, c);
};


void fill_state_input(absolute_time_t current_time)
{
    u8* state = &g_state.btn_state;

    u8 prev_modebt = 0b00;
    u8 prev_playbt = 0b00;
    u8 prev_prevbt = 0b00;
    u8 prev_nextbt = 0b00;

    if (GET_MODE(*state) & 0b01) 
        prev_modebt = 0b10;
    if (GET_MODE(*state) == 0b01) 
        g_state.modebt_prev_active = current_time; 

    if (GET_PLAY(*state) & 0b01) 
        prev_playbt = 0b10;
    if (GET_PLAY(*state) == 0b01)
        g_state.playbt_prev_active = current_time;


    if (GET_PREV(*state) & 0b01) 
        prev_prevbt = 0b10;
    if (GET_PREV(*state) == 0b01) 
        g_state.prevbt_prev_active = current_time;

    if (GET_NEXT(*state) & 0b01) 
        prev_nextbt = 0b10;
    if (GET_NEXT(*state) == 0b01) 
        g_state.nextbt_prev_active = current_time;

    SET_MODE(*state, gpio_get(MODE_BUTTON) | prev_modebt);
    SET_PLAY(*state, gpio_get(PLAY_BUTTON) | prev_playbt);
    SET_PREV(*state, gpio_get(PREV_BUTTON) | prev_prevbt);
    SET_NEXT(*state, gpio_get(NEXT_BUTTON) | prev_nextbt);
};


char decode_morse(YaarString s) {

    if (s.len == 0 || s.cap == 0 || s.data == NULL)
        return '?';

    if (s.data[0]=='.' && s.data[1]=='_' && s.data[2]==' ' && s.data[3]==' ' && s.data[4]==' ') return 'A'; // ._
    if (s.data[0]=='_' && s.data[1]=='.' && s.data[2]=='.' && s.data[3]=='.' && s.data[4]==' ') return 'B'; // _...
    if (s.data[0]=='_' && s.data[1]=='.' && s.data[2]=='_' && s.data[3]=='.' && s.data[4]==' ') return 'C'; // _._.
    if (s.data[0]=='_' && s.data[1]=='.' && s.data[2]=='.' && s.data[3]==' ' && s.data[4]==' ') return 'D'; // _..
    if (s.data[0]=='.' && s.data[1]==' ' && s.data[2]==' ' && s.data[3]==' ' && s.data[4]==' ') return 'E'; // .
    if (s.data[0]=='.' && s.data[1]=='.' && s.data[2]=='_' && s.data[3]=='.' && s.data[4]==' ') return 'F'; // .._.
    if (s.data[0]=='_' && s.data[1]=='_' && s.data[2]=='.' && s.data[3]==' ' && s.data[4]==' ') return 'G'; // __.
    if (s.data[0]=='.' && s.data[1]=='.' && s.data[2]=='.' && s.data[3]=='.' && s.data[4]==' ') return 'H'; // ....
    if (s.data[0]=='.' && s.data[1]=='.' && s.data[2]==' ' && s.data[3]==' ' && s.data[4]==' ') return 'I'; // ..
    if (s.data[0]=='.' && s.data[1]=='_' && s.data[2]=='_' && s.data[3]=='_' && s.data[4]==' ') return 'J'; // .___
    if (s.data[0]=='_' && s.data[1]=='.' && s.data[2]=='_' && s.data[3]==' ' && s.data[4]==' ') return 'K'; // _._
    if (s.data[0]=='.' && s.data[1]=='_' && s.data[2]=='.' && s.data[3]=='.' && s.data[4]==' ') return 'L'; // ._..
    if (s.data[0]=='_' && s.data[1]=='_' && s.data[2]==' ' && s.data[3]==' ' && s.data[4]==' ') return 'M'; // __
    if (s.data[0]=='_' && s.data[1]=='.' && s.data[2]==' ' && s.data[3]==' ' && s.data[4]==' ') return 'N'; // _.
    if (s.data[0]=='_' && s.data[1]=='_' && s.data[2]=='_' && s.data[3]==' ' && s.data[4]==' ') return 'O'; // ___
    if (s.data[0]=='.' && s.data[1]=='_' && s.data[2]=='_' && s.data[3]=='.' && s.data[4]==' ') return 'P'; // .__.
    if (s.data[0]=='_' && s.data[1]=='_' && s.data[2]=='.' && s.data[3]=='_' && s.data[4]==' ') return 'Q'; // __._
    if (s.data[0]=='.' && s.data[1]=='_' && s.data[2]=='.' && s.data[3]==' ' && s.data[4]==' ') return 'R'; // ._.
    if (s.data[0]=='.' && s.data[1]=='.' && s.data[2]=='.' && s.data[3]==' ' && s.data[4]==' ') return 'S'; // ...
    if (s.data[0]=='_' && s.data[1]==' ' && s.data[2]==' ' && s.data[3]==' ' && s.data[4]==' ') return 'T'; // _
    if (s.data[0]=='.' && s.data[1]=='.' && s.data[2]=='_' && s.data[3]==' ' && s.data[4]==' ') return 'U'; // .._
    if (s.data[0]=='.' && s.data[1]=='.' && s.data[2]=='.' && s.data[3]=='_' && s.data[4]==' ') return 'V'; // ..._
    if (s.data[0]=='.' && s.data[1]=='_' && s.data[2]=='_' && s.data[3]==' ' && s.data[4]==' ') return 'W'; // .__
    if (s.data[0]=='_' && s.data[1]=='.' && s.data[2]=='.' && s.data[3]=='_' && s.data[4]==' ') return 'X'; // _.._
    if (s.data[0]=='_' && s.data[1]=='.' && s.data[2]=='_' && s.data[3]=='_' && s.data[4]==' ') return 'Y'; // _.__
    if (s.data[0]=='_' && s.data[1]=='_' && s.data[2]=='.' && s.data[3]=='.' && s.data[4]==' ') return 'Z'; // __..

    return '?';
};

