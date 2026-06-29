#ifndef __ILI9341_H__
#define __ILI9341_H__

#include "fonts.h" //@TODO disabled this does anything use this?
#include <stdbool.h>
#include "main.h" //TODO important

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

#define ILI9341_CASET 0x2A   // Column address set
#define ILI9341_PASET 0x2B   // Page address set
#define ILI9341_RAMWR 0x2C   // Memory write
#define ILI9341_RAMRD 0x2E   // Memory read

/*** Redefine if necessary ***/
#define ILI9341_SPI_PORT hspi1
extern SPI_HandleTypeDef ILI9341_SPI_PORT;

#define ILI9341_RES_Pin       SPI_RESET_Pin             //original value: GPIO_PIN_7
#define ILI9341_RES_GPIO_Port SPI_RESET_GPIO_Port       //original value: GPIOC
#define ILI9341_CS_Pin        SPI_CS_Pin				//original value: GPIO_PIN_6
#define ILI9341_CS_GPIO_Port  SPI_CS_GPIO_Port			//original value: GPIOB
#define ILI9341_DC_Pin        SPI_DC_Pin 				//original value: GPIO_PIN_9
#define ILI9341_DC_GPIO_Port  GPIOC 					//original value: GPIOA

//// Global variables for pins and ports //TODO GET RID OF THIS
//extern GPIO_TypeDef* ILI9341_RES_GPIO_Port;
//extern uint16_t      ILI9341_RES_Pin;
//
//extern GPIO_TypeDef* ILI9341_CS_GPIO_Port;
//extern uint16_t      ILI9341_CS_Pin;
//
//extern GPIO_TypeDef* ILI9341_DC_GPIO_Port;
//extern uint16_t      ILI9341_DC_Pin;

// default orientation
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320
#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR)

// rotate right
/*
#define ILI9341_WIDTH  320
#define ILI9341_HEIGHT 240
#define ILI9341_ROTATION (ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)
*/

// rotate left
/*
#define ILI9341_WIDTH  320
#define ILI9341_HEIGHT 240
#define ILI9341_ROTATION (ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR)
*/

// upside down
/*
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320
#define ILI9341_ROTATION (ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR)
*/

/****************************/

// Color definitions
#define	ILI9341_BLACK   0x0000
#define	ILI9341_BLUE    0x001F
#define	ILI9341_RED     0xF800
#define	ILI9341_GREEN   0x07E0
#define ILI9341_CYAN    0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW  0xFFE0
#define ILI9341_WHITE   0xFFFF
#define ILI9341_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

// call before initializing any SPI devices
void ILI9341_Unselect();

void ILI9341_Init(void);
void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
//void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ILI9341_FastFillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_ClearScreen(); //fills the screen with black pixels
void ILI9341_FastClear(uint16_t color);
void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void ILI9341_DrawBufferOnDisplay(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void ILI9341_InvertColors(bool invert);


//void ILI9341_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor); // TODO  proble this is static it shouldnt nven need to be defined
void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
//void init_ili9341_pins(); //I added this //TODO GET RID OF THIS

#endif // __ILI9341_H__
