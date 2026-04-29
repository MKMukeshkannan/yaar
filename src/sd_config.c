#include "ff.h"
#include "sd_card.h"
#include "spi.h"
#include "yaar_state.h"
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


void ls_dir()
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;

    for (int i = 0; i < 20; ++i) 
        ys_free(&g_state.dir_contents[i]);

    res = f_opendir(&dir, "0:/");                   /* Open the directory */
    if (res == FR_OK) {
        nfile = ndir = 0;


        for (int i = 0; i < 20; ++i) 
        {
            res = f_readdir(&dir, &fno);
            if (fno.fname[0] == 0) break;
            if (fno.fattrib & AM_DIR) {
                char tmp[260];
                snprintf(tmp, sizeof(tmp), "%s/", fno.fname);

                ys_from_str(&g_state.dir_contents[i], tmp);
                ndir++;
            } else {
                ys_from_str(&g_state.dir_contents[i], fno.fname);
                nfile++;
            };
        }

        f_closedir(&dir);
        g_state.total_ls = ndir + nfile;
        // printf("%d dirs, %d files.\n", ndir, nfile);
    } else {
        // printf("Failed to open \"%s\". (%u)\n", path, res);
    }

    for (int i = 0; i < 5; ++i) 
    {
        YaarString* y = &g_state.dir_contents[i];
        printf("len: %i, cap: %i dat: %s\n", y->len, y->cap, y->data);
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
