#pragma once

#include "sd_card.h"
#include "spi.h"
#include "stdbool.h"

extern spi_t spi;
extern sd_card_t sd_card;

bool ls_dir();
void search();
