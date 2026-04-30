#include "ff.h"
#include "sd_card.h"
#include "spi.h"
#include "yaar_state.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

spi_t spi = 
{
    .hw_inst = spi1,
    .miso_gpio = 12,
    .mosi_gpio = 15,
    .sck_gpio =  14,
    .baud_rate = 25 * 1000 * 1000, 
};

sd_card_t sd_card = 
{
    .pcName = "0:",
    .spi = &spi,
    .ss_gpio = 13,
    .use_card_detect = false,
    .card_detect_gpio = 13,
    .card_detected_true = -1
};


bool ls_dir()
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;

    for (int i = 0; i < 100; ++i) 
    {
        ys_free(&g_state.dir_contents[i]);
        ys_free(&g_state.display_contents[i]);
    };

    res = f_opendir(&dir, g_state.location.data);
    if (res == FR_OK) 
    {
        nfile = ndir = 0;

        for (int i = 0; i < 100; ++i) 
        {
            res = f_readdir(&dir, &fno);
            if (fno.fname[0] == 0) break;
            if (fno.fattrib & AM_DIR) {
                char tmp[260];
                snprintf(tmp, sizeof(tmp), "%s/", fno.fname);

                ys_from_str(&g_state.dir_contents[i], tmp);
                ys_from_str(&g_state.display_contents[i], tmp);
                ndir++;
            } else {
                ys_from_str(&g_state.dir_contents[i], fno.fname);
                ys_from_str(&g_state.display_contents[i], fno.fname);
                nfile++;
            };
        }

        f_closedir(&dir);
        g_state.total_ls = ndir + nfile;
    } else {
        return YAAR_DIR_OPEN_FAILED;
    }

    return YAAR_OK;
};


static bool is_contains(YaarString haystack, YaarString needle)
{
    if (needle.len == 0) return true; // empty needle matches
    if (needle.len > haystack.len) return false;

    // printf("searching: %s in %s\n", haystack.data, needle.data);

    for (u8 i = 0; i <= haystack.len - needle.len; i++)
    {
        u8 j = 0;

        while (j < needle.len &&
               tolower((unsigned char)haystack.data[i + j]) ==
               tolower((unsigned char)needle.data[j]))
        {
            j++;
        }

        if (j == needle.len)
        {
            // printf("FOUND: %s in %s\n", haystack.data, needle.data);
            return true; // match found
        }
    }

    return false;
}

void search() 
{
    if (g_state.ip_box_buf.len == 0) 
    {
        printf("ehere\n");
        for (u8 i = 0; i < 100; ++i) 
            ys_from_str(&g_state.display_contents[i], g_state.dir_contents[i].data);
        return;
    };


    u8 res_ind = 0;
    for (u8 i = 0; i < 20; ++i) 
    {
        if (is_contains(g_state.dir_contents[i], g_state.ip_box_buf)) 
            g_state.display_contents[res_ind++] = g_state.dir_contents[i];
    };

    for (int i = 0; i < 5; ++i) 
    {
        // printf("%s\n", g_state.display_contents[i].data);
    };

    return;
};

size_t sd_get_num() { return 1; }
sd_card_t *sd_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &sd_card;
    } else {
        return NULL;
    }
}
size_t spi_get_num() { return 1; }
spi_t *spi_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &spi;
    } else {
        return NULL;
    }
}
