/*
 *  st7789.h file
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "fonts.h"
#include "pico_common.h"

/*
 * Display Settings
 */
#define DISP_WIDTH 240
#define DISP_HEIGHT 240
#define CENTER_Y (DISP_WIDTH/2)
#define CENTER_X (DISP_HEIGHT/2)

/*
 * ST7789 commands
 */
static const uint8_t  ST7789_NOP = 0x00;
static const uint8_t  ST7789_SWRESET = 0x01;
static const uint8_t  ST7789_RDDID = 0x04;
static const uint8_t  ST7789_RDDST = 0x09;

static const uint8_t  ST7789_SLPIN = 0x10;
static const uint8_t  ST7789_SLPOUT = 0x11;
static const uint8_t  ST7789_PTLON = 0x12;
static const uint8_t  ST7789_NORON = 0x13;

static const uint8_t  ST7789_INVOFF = 0x20;
static const uint8_t  ST7789_INVON = 0x21;
static const uint8_t  ST7789_DISPOFF = 0x28;
static const uint8_t  ST7789_DISPON = 0x29;
static const uint8_t  ST7789_CASET = 0x2A;
static const uint8_t  ST7789_RASET = 0x2B;
static const uint8_t  ST7789_RAMWR = 0x2C;
static const uint8_t  ST7789_RAMRD = 0x2E;

static const uint8_t  ST7789_PTLAR = 0x30;
static const uint8_t  ST7789_VSCRDEF = 0x33;
static const uint8_t  ST7789_COLMOD = 0x3A;
static const uint8_t  ST7789_MADCTL = 0x36;
static const uint8_t  ST7789_VSCSAD = 0x37;

static const uint8_t  ST7789_MADCTL_MY = 0x80;
static const uint8_t  ST7789_MADCTL_MX = 0x40;
static const uint8_t  ST7789_MADCTL_MV = 0x20;
static const uint8_t  ST7789_MADCTL_ML = 0x10;
static const uint8_t  ST7789_MADCTL_BGR = 0x08;
static const uint8_t  ST7789_MADCTL_MH = 0x04;
static const uint8_t  ST7789_MADCTL_RGB = 0x00;

static const uint8_t  ST7789_RDID1 = 0xDA;
static const uint8_t  ST7789_RDID2 = 0xDB;
static const uint8_t  ST7789_RDID3 = 0xDC;
static const uint8_t  ST7789_RDID4 = 0xDD;

static const uint8_t  COLOR_MODE_65K = 0x50;
static const uint8_t  COLOR_MODE_262K = 0x60;
static const uint8_t  COLOR_MODE_12BIT = 0x03;
static const uint8_t  COLOR_MODE_16BIT = 0x05;
static const uint8_t  COLOR_MODE_18BIT = 0x06;
static const uint8_t  COLOR_MODE_16M = 0x07;

/*
 * Color voidinitions
 */
static const uint16_t  BLACK = 0x0000;
static const uint16_t  BLUE = 0x001F;
static const uint16_t  RED = 0xF800;
static const uint16_t  GREEN = 0x07E0;
static const uint16_t  CYAN = 0x07FF;
static const uint16_t  MAGENTA = 0xF81F;
static const uint16_t  YELLOW = 0xFFE0;
static const uint16_t  WHITE = 0xFFFF;

static const int _BUFFER_SIZE = 256;

static const uint8_t _BIT7 = 0x80;
static const uint8_t _BIT6 = 0x40;
static const uint8_t _BIT5 = 0x20;
static const uint8_t _BIT4 = 0x10;
static const uint8_t _BIT3 = 0x08;
static const uint8_t _BIT2 = 0x04;
static const uint8_t _BIT1 = 0x02;
static const uint8_t _BIT0 = 0x01;

/*
 * Prototypes
 */
void display_init();
uint16_t convert_rgb_to_hex(int red, int green, int blue);

/* Low level functions */
void _writeCommand(uint8_t command);
void _writeData(uint8_t data[], uint32_t Len);
void _set_color_mode(uint8_t mode);
void _set_columns(int start, int end);
void _set_rows(int start, int end);
void _set_window(int x0, int y0, int x1, int y1);

/* Commands function */
void hard_reset();
void soft_reset();
void sleep_mode(bool value);
void inversion_mode(int value);
void rotation(int rotation); //Set display rotation. Args:rotation (int): 0-Portrait 1-Landscape 2-Inverted Portrait 3-Inverted Landscape

/* Graphical functions */
void vline(int x, int y, int length, uint16_t color);
void hline(int x, int y, int length, uint16_t color);
void pixel(int x, int y, uint16_t color);
void blit_buffer(uint8_t buffer[], uint8_t bufferLen, int x, int y, int width, int height);
void rect(int x, int y, int w, int h, uint16_t color);
void fill_rect(int x, int y, int width, int height, uint16_t color);
void fill(uint16_t color);
void line(int x0, int y0, int x1, int y1, uint16_t color);
void vscrdef(int tfa, int vsa, int bfa);
void vscsad(int vssa);

/* Text function */
void writeChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
void writeString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor);
