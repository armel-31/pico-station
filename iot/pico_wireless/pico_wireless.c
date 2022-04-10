/*
 *   Pico Wireless device
 */



#include "pico_wireless.h"


FATFS fs;
FIL fil;
FRESULT fr;



uint8_t main() {
    stdio_init_all();

    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK) {
        printf("Failed to mount SD card, error: %d\n", fr);
        return 0;
    }

    FILINFO file;
    DIR *dir;
    printf("Listing /\n");

    f_opendir(dir, "/");
    while(f_readdir(dir, &file) == FR_OK && file.fname[0]) {
        printf("%s %lld\n", file.fname, file.fsize);
    }
    f_closedir(dir);

    return 0;
}
