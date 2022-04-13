/*
 *   Pico Wireless device
 */


#include "pico_wireless.h"


FATFS fs;
FIL fil;
FRESULT fr;

SdCardDef sdcard;

uint8_t main() {
    stdio_init_all();
    sdcard.sd_card_mount = 1;

    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK) {
        printf("Failed to mount SD card, error: %d\n", fr);
        sdcard.sd_card_mount = 0;
    }

    if(sdcard.sd_card_mount == 1) {
        FILINFO file;
        DIR *dir;
        printf("Listing /\n");

        f_opendir(dir, "/");
        while(f_readdir(dir, &file) == FR_OK && file.fname[0]) {
            printf("%s %lld\n", file.fname, file.fsize);
        }
        f_closedir(dir);
        while(1) {
            sleep_ms(100);
        }
    } else {
        while(1) {
            printf("SD Card not mounted\n");
        }
    }

    return 0;
}
