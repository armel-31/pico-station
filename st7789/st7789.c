/*
 *  st7789.c file
 */

#include "st7789.h"

//Initialize display
void display_init(){
    soft_reset();
    sleep_mode(false);

    //_set_color_mode (COLOR_MODE_65K | COLOR_MODE_16BIT);
    _set_color_mode(COLOR_MODE_16BIT);
    sleep_ms(50);
    rotation(0);
    inversion_mode(true);
    sleep_ms(10);
    _writeCommand(ST7789_NORON);
    sleep_ms(10);
    _writeCommand(ST7789_SLPOUT);
    sleep_ms(10);
    _writeCommand(ST7789_DISPON);
    sleep_ms(500);
    _set_window(0, 0, DISP_WIDTH, DISP_HEIGHT);
    fill(BLACK);

}

static void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	uint16_t x_start = x0, x_end = x1;
	uint16_t y_start = y0, y_end = y1;
	
	/* Column Address set */
	_writeCommand(ST7789_CASET); 
	{
		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
		_writeData(data, sizeof(data));
	}

	/* Row Address set */
	_writeCommand(ST7789_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		_writeData(data, sizeof(data));
	}
	/* Write to RAM */
	_writeCommand(ST7789_RAMWR);
}

void _writeCommand(uint8_t command){
    gpio_put(CS_PIN, 0);
    gpio_put(DC_PIN, 0);
    spi_write_blocking(SPI_PORT, &command, 1);
    gpio_put(CS_PIN, 1);
}

void _writeData(uint8_t data[], uint32_t Len){
    gpio_put(CS_PIN, 0);
    gpio_put(DC_PIN, 1);
    spi_write_blocking(SPI_PORT, data, Len);
    gpio_put(CS_PIN, 1);
}

void soft_reset()   // Soft reset display.
{
    _writeCommand(ST7789_SWRESET);
    sleep_ms(150);
}

void sleep_mode(bool value) //Enable(1) or disable(0) display sleep mode. 
{
    if(value){
        _writeCommand(ST7789_SLPIN);
    }else{
        _writeCommand(ST7789_SLPOUT);
    }
}

void inversion_mode(int value)  //Enable(1) or disable(0) display inversion mode.
{
    if(value){
        _writeCommand(ST7789_INVON);
    }else{
        _writeCommand(ST7789_INVOFF);
    }
}

void _set_color_mode(uint8_t mode)  //Set display color mode.
{
    _writeCommand(ST7789_COLMOD);
    uint8_t data[] = { mode, 0x77 };
    _writeData(data, 2);
}

void rotation(int rotation){
    /*Set display rotation.
        Args:
            rotation (int):
                - 0-Portrait
                - 1-Landscape
                - 2-Inverted Portrait
                - 3-Inverted Landscape*/
    _writeCommand(ST7789_MADCTL);
    uint8_t data[] = {0x00};
    _writeData(data, 1);
}

void _set_columns(int start, int end){
    //Send CASET (column address set) command to display.
    //Args: start (int): column start address end (int): column end address
    if(start > end > DISP_WIDTH) return;
    uint8_t _x = start;
    uint8_t _y = end;
    uint8_t result[] = { 0x00, 0x00, _x, _y };
    _writeCommand(ST7789_CASET);
    _writeData(result, 5);
}

void _set_rows(int start, int end){
    //Send RASET (row address set) command to display.
    //Args: start (int): row start address, end (int): row end address
    if(start > end > DISP_HEIGHT) return;
    _writeCommand(ST7789_RASET);
    uint8_t _x = start;
    uint8_t _y = end;
    uint8_t result[] = { 0x00, 0x00, _x, _y };
    _writeData(result, 5);
}

void _set_window(int x0, int y0, int x1, int y1){;
    //Set window to column and row address.
    //Args:x0 (int): column start address, y0 (int): row start address
    //x1 (int): column end address, y1 (int): row end address
    _set_columns(x0, x1);
    _set_rows(y0, y1);
    _writeCommand(ST7789_RAMWR);
}

void vline(int x, int y, int length, uint16_t color){
    //Draw vertical line at the given location and color.
    //Args: x (int): x coordinate, Y (int): y coordinate 
    //length (int): length of line, color (int): 565 encoded color
    fill_rect(x, y, 1, length, color);
}

void hline(int x, int y, int length, uint16_t color){
    //Draw horizontal line at the given location and color.
    //Args:x (int): x coordinate, Y (int): y coordinate
    //length (int): length of line, color (int): 565 encoded color
    fill_rect(x, y, length, 1, color);
}

void blit_buffer(uint8_t buffer[], uint8_t bufferLen,int x, int y, int width, int height){
    //Copy buffer to display at the given location.
    //Args:buffer (bytes): Data to copy to display
    //x (int): Top left corner x coordinate, Y (int): Top left corner y coordinate
    //width (int): Width, height (int): Height
    _set_window(x, y, x + width - 1, y + height - 1);
    _writeData(buffer, bufferLen);
}

void rect(int x, int y, int w, int h, uint16_t color){
    //Draw a rectangle at the given location, size and color.
    //Args: x (int): Top left corner x coordinate, y (int): Top left corner y coordinate
    //width (int): Width in pixels, height (int): Height in pixels
    //color (int): 565 encoded color
    hline(x, y, w, color);
    vline(x, y, h, color);
    vline(x + w - 1, y, h, color);
    hline(x, y + h - 1, w, color);
}

void fill_rect(int x, int y, int width, int height, uint16_t color){
    //Draw a rectangle at the given location, size and filled with color.
    //Args: x (int): Top left corner x coordinate, y (int): Top left corner y coordinate
    //width (int): Width in pixels, height (int): Height in pixels
    //color (int): 565 encoded color
    _set_window(x, y, x + width - 1, y + height - 1);

    //uint16_t pixel16[1];
    //pixel16[0] = color;
    uint8_t pixel[2];
    pixel[0]=(color >> 8);
    pixel[1]=color & 0xff;

    div_t output;
    output = div(width *= height, _BUFFER_SIZE);
    int chunks = output.quot;

    gpio_put(DC_PIN, 1);    //Open to write
    uint sendbuffer = _BUFFER_SIZE * 2;
    uint8_t _drawpixel[sendbuffer];

    for (int i = 0; i < sendbuffer; i++)
    {
        _drawpixel[i] = pixel[i%2];
    }

    for(int i=_BUFFER_SIZE; i!=0; i--){
        _writeData(_drawpixel, sendbuffer);
    }
}

void fill(uint16_t color){
    //Fill the entire FrameBuffer with the specified color.
    //Args:color (int): 565 encoded color
    fill_rect(0, 0, DISP_WIDTH, DISP_HEIGHT, color);
}

void line(int x0, int y0, int x1, int y1, uint16_t color){
    //Draw a single pixel wide line starting at x0, y0 and ending at x1, y1.
    /*Args:
            x0 (int): Start point x coordinate
            y0 (int): Start point y coordinate
            x1 (int): End point x coordinate
            y1 (int): End point y coordinate
            color (int): 565 encoded color*/
    int a = abs(y1 - y0);
    int b = abs(x1 - x0);
    int steep = (a > b);
    if(steep){
        x0, y0 = y0, x0;
        x1, y1 = y1, x1;
    }

    if(x0 > x1){
        x0, x1 = x1, x0;
        y0, y1 = y1, y0;
    }
    int dx = x1 - x0;
    int dy = abs(y1 - y0);
    int err = dx / 2;

    int ystep;
    if(y0 < y1){
        ystep = 1;
    }else{
        ystep = -1;
    }

    while(x0 <= x1){
        if(steep){
            pixel(y0, x0, color);
        }else{
            pixel(x0, y0, color);
        }
        err -= dy;
        if(err < 0){
            y0 += ystep;
            err += dx;
        }
        x0 += 1;
    }
}

void vscrdef(int tfa, int vsa, int bfa){
    /*Set Vertical Scrolling Definition.
        To scroll a 135x240 display these values should be 40, 240, 40.
        There are 40 lines above the display that are not shown followed by
        240 lines that are shown followed by 40 more lines that are not shown.
        You could write to these areas off display and scroll them into view by
        changing the TFA, VSA and BFA values.
        Args:
            tfa (int): Top Fixed Area
            vsa (int): Vertical Scrolling Area
            bfa (int): Bottom Fixed Area*/
            //\x3e\x48\x48\x48
    uint8_t _vscrdef[] = { 0x3E, 0x48, 0x48, 0x48, tfa, vsa, bfa };
    _writeCommand(ST7789_VSCRDEF);
    _writeData(_vscrdef, 7);
}

void vscsad(int vssa){
    uint8_t _vscsad[] = { 0x3E, 0x48, vssa };
    _writeCommand(ST7789_VSCSAD);
    _writeData(_vscsad, 3);
}

uint16_t convert_rgb_to_hex(int red, int green, int blue){
    //Convert red, green and blue values (0-255) into a 16-bit 565 encoding
    return (red & 0xf8) << 8 | (green & 0xfc) << 3 | blue >> 3;
}

void writeChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
	uint32_t i, b, j;
	setAddressWindow(x, y, x + font.width - 1, y + font.height - 1);

	for (i = 0; i < font.height; i++) {
		b = font.data[(ch - 32) * font.height + i];
		for (j = 0; j < font.width; j++) {
			if ((b << j) & 0x8000) {
				uint8_t data[] = {color >> 8, color & 0xFF};
				_writeData(data, sizeof(data));
			}
			else {
				uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
				_writeData(data, sizeof(data));
			}
		}
	}
}

void writeString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	while (*str) {
		if (x + font.width >= DISP_WIDTH) {
			x = 0;
			y += font.height;
			if (y + font.height >= DISP_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		writeChar(x, y, *str, font, color, bgcolor);
		x += font.width;
		str++;
	}
}

