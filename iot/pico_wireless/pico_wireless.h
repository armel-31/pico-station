/*
 * pico_wireless.h file
 */

#ifndef _PICO_WIRELESS_HEADER_
#define _PICO_WIRELESS_HEADER_

/*
 *  Includes
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "libs/fatfs/ff.h"


/*
 *  Structures definition
 */

typedef struct {
    uint8_t sd_card_mount;

} SdCardDef;




/*
 *  Constants
 */


/*
 *  Prototypes
 */





#endif  // _PICO_WIRELESS_HEADER_
