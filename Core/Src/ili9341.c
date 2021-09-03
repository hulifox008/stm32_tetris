#include <stdint.h>
#include "main.h"


#define ILI9341_CMD_RDII    0x04    /* Read display identification information */
#define ILI9341_CMD_RDID    0xD3    /* Read ID */

extern SPI_HandleTypeDef hspi1;

#define ILI9341_TFTWIDTH 240  ///< ILI9341 max TFT width
#define ILI9341_TFTHEIGHT 320 ///< ILI9341 max TFT height

#define ILI9341_NOP 0x00     ///< No-op register
#define ILI9341_SWRESET 0x01 ///< Software reset register
#define ILI9341_RDDID 0x04   ///< Read display identification information
#define ILI9341_RDDST 0x09   ///< Read Display Status

#define ILI9341_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9341_SLPOUT 0x11 ///< Sleep Out
#define ILI9341_PTLON 0x12  ///< Partial Mode ON
#define ILI9341_NORON 0x13  ///< Normal Display Mode ON

#define ILI9341_RDMODE 0x0A     ///< Read Display Power Mode
#define ILI9341_RDMADCTL 0x0B   ///< Read Display MADCTL
#define ILI9341_RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define ILI9341_RDIMGFMT 0x0D   ///< Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9341_INVOFF 0x20   ///< Display Inversion OFF
#define ILI9341_INVON 0x21    ///< Display Inversion ON
#define ILI9341_GAMMASET 0x26 ///< Gamma Set
#define ILI9341_DISPOFF 0x28  ///< Display OFF
#define ILI9341_DISPON 0x29   ///< Display ON

#define ILI9341_CASET 0x2A ///< Column Address Set
#define ILI9341_PASET 0x2B ///< Page Address Set
#define ILI9341_RAMWR 0x2C ///< Memory Write
#define ILI9341_RAMRD 0x2E ///< Memory Read

#define ILI9341_PTLAR 0x30    ///< Partial Area
#define ILI9341_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ILI9341_MADCTL 0x36   ///< Memory Access Control
#define ILI9341_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9341_PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1                                                        \
  0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3                                                        \
  0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR 0xB4  ///< Display Inversion Control
#define ILI9341_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9341_PWCTR1 0xC0 ///< Power Control 1
#define ILI9341_PWCTR2 0xC1 ///< Power Control 2
#define ILI9341_PWCTR3 0xC2 ///< Power Control 3
#define ILI9341_PWCTR4 0xC3 ///< Power Control 4
#define ILI9341_PWCTR5 0xC4 ///< Power Control 5
#define ILI9341_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9341_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9341_RDID1 0xDA ///< Read ID 1
#define ILI9341_RDID2 0xDB ///< Read ID 2
#define ILI9341_RDID3 0xDC ///< Read ID 3
#define ILI9341_RDID4 0xDD ///< Read ID 4

#define ILI9341_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9341_GMCTRN1 0xE1 ///< Negative Gamma Correction
//#define ILI9341_PWCTR6     0xFC

// Color definitions
#define ILI9341_BLACK 0x0000       ///<   0,   0,   0
#define ILI9341_NAVY 0x000F        ///<   0,   0, 123
#define ILI9341_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9341_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9341_MAROON 0x7800      ///< 123,   0,   0
#define ILI9341_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9341_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9341_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9341_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ILI9341_BLUE 0x001F        ///<   0,   0, 255
#define ILI9341_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9341_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9341_RED 0xF800         ///< 255,   0,   0
#define ILI9341_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9341_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9341_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9341_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9341_PINK 0xFC18        ///< 255, 130, 198

void ili9341_reset()
{
  HAL_GPIO_WritePin(GPIOB, DISP_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOB, DISP_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(100);
}

void ili9341_cs_low()
{
  HAL_GPIO_WritePin(GPIOB, SPI1_CS_Pin, GPIO_PIN_RESET);
}

void ili9341_cs_hi()
{
  HAL_GPIO_WritePin(GPIOB, SPI1_CS_Pin, GPIO_PIN_SET);
}

void ili9341_dc_low()
{
  HAL_GPIO_WritePin(GPIOB, DISP_D_Cn_Pin, GPIO_PIN_RESET);
}

void ili9341_dc_hi()
{
  HAL_GPIO_WritePin(GPIOB, DISP_D_Cn_Pin, GPIO_PIN_SET);
}

void ili9341_send_cmd(uint8_t cmd, uint8_t *param, int param_len)
{
    ili9341_cs_low();

    ili9341_dc_low();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
    ili9341_dc_hi();
    if(param_len>0){
        HAL_SPI_Transmit(&hspi1, param, param_len, 1000);
    }
    ili9341_cs_hi();
    //HAL_Delay(1);
}

void ili9341_send_data(uint8_t *data, int len)
{
    ili9341_cs_low();

    ili9341_dc_hi();
    HAL_SPI_Transmit(&hspi1, &data, len, 1000);

    ili9341_cs_hi();
   // HAL_Delay(1);
}

void ili9341_init()
{
	  ili9341_send_cmd(0xEF, "\x03\x80\x02", 3);
	  ili9341_send_cmd(0xCF, "\x00\xC1\x30", 3);
	  ili9341_send_cmd(0xED, "\x64\x03\x12\x81", 4);
	  ili9341_send_cmd(0xE8, "\x85\x00\x78", 3);
	  ili9341_send_cmd(0xCB, "\x39\x2C\x00\x34\x02", 6);
	  ili9341_send_cmd(0xF7, "\x20", 1);
	  ili9341_send_cmd(0xEA, "\x00\x00", 2);
	  ili9341_send_cmd(ILI9341_PWCTR1  , "\x23", 1);            // Power control VRH[5:0]
	  ili9341_send_cmd(ILI9341_PWCTR2  , "\x10", 1);             // Power control SAP[2:0];BT[3:0]
	  ili9341_send_cmd(ILI9341_VMCTR1  , "\x3e\x28", 2);      // VCM control
	  ili9341_send_cmd(ILI9341_VMCTR2  , "\x86", 1);             // VCM control2
	  ili9341_send_cmd(ILI9341_MADCTL  , "\x48", 1);             // Memory Access Control
	  ili9341_send_cmd(ILI9341_VSCRSADD, "\x00", 1);             // Vertical scroll zero
	  ili9341_send_cmd(ILI9341_PIXFMT  , "\x55", 1);
	  ili9341_send_cmd(ILI9341_FRMCTR1 , "\x00\x18", 2);
	  ili9341_send_cmd(ILI9341_DFUNCTR , "\x08\x82\x27", 3); // Display Function Control

	  ili9341_send_cmd(0xF2, "\x00", 1);                         // 3Gamma Function Disable

	  ili9341_send_cmd(ILI9341_GAMMASET , "\x01", 1);            // Gamma curve selected
	  ili9341_send_cmd(ILI9341_GMCTRP1  , "\x0F\x31\x2B\x0C\x0E\x08\x4E\xF1\x37\x07\x10\x03\x0E\x09\x00", 15); // Set Gamma
	  ili9341_send_cmd(ILI9341_GMCTRN1  , "\x00\x0E\x14\x03\x11\x07\x31\xC1\x48\x08\x0F\x0C\x31\x36\x0F", 15); // Set Gamma
	  ili9341_send_cmd(ILI9341_SLPOUT   , NULL, 0),                // Exit Sleep
      HAL_Delay(200);
	  ili9341_send_cmd(ILI9341_DISPON   , NULL, 0),                // Display on
      HAL_Delay(200);





}

void ili9341_read_id()
{
    uint8_t cmd[4] = {0}, buf[4] = {0};
    cmd[0] = 0x0A;

    ili9341_cs_low();
    ili9341_dc_low();
    //HAL_SPI_TransmitReceive(&hspi1, &cmd[0], &buf[0], 1, 1000);
    HAL_SPI_Transmit(&hspi1, &cmd[0], 1, 1000);
    ili9341_dc_hi();
    cmd[0] = 0;
    //HAL_SPI_TransmitReceive(&hspi1, &cmd[0], &buf[0], 4, 1000);
    HAL_SPI_Receive(&hspi1, &buf[0], 4, 1000);
    ili9341_cs_hi();
}

void _ili9341_set_window(int x, int y, int x1, int y1)
{
	uint8_t buf[4];

	ili9341_dc_low();
	buf[0] = ILI9341_CASET;
	HAL_SPI_Transmit(&hspi1, buf, 1, 1000);
	ili9341_dc_hi();

	buf[0] = x>>8;
	buf[1] = x;
	buf[2] = x1>>8;
	buf[3] = x1;
	HAL_SPI_Transmit(&hspi1, buf, 4, 1000);

	ili9341_dc_low();
	buf[0] = ILI9341_PASET;
	HAL_SPI_Transmit(&hspi1, buf, 1, 1000);
	ili9341_dc_hi();

	buf[0] = y>>8;
	buf[1] = y;
	buf[2] = y1>>8;
	buf[3] = y1;
	HAL_SPI_Transmit(&hspi1, buf, 4, 1000);
}

void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	uint8_t buf[2];
	uint16_t i;

	ili9341_cs_low();
	uint16_t x1 = x+w-1;
	uint16_t y1 = y+h-1;

	_ili9341_set_window(x, y, x1, y1);

	ili9341_dc_low();
	buf[0] = ILI9341_RAMWR;
	HAL_SPI_Transmit(&hspi1, buf, 1, 1000);
	ili9341_dc_hi();

	buf[0] = color>>8;
	buf[1] = color;
	while(h--) {
		for(i=0;i<w;i++) {
			HAL_SPI_Transmit(&hspi1, buf, 2, 1000);
		}
	}

	ili9341_cs_hi();
}

void ili9341_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t*data)
{
	uint8_t buf[2];

	ili9341_cs_low();
	uint16_t x1 = x+w-1;
	uint16_t y1 = y+h-1;

	_ili9341_set_window(x, y, x1, y1);

	ili9341_dc_low();
	buf[0] = ILI9341_RAMWR;
	HAL_SPI_Transmit(&hspi1, buf, 1, 1000);
	ili9341_dc_hi();

	HAL_SPI_Transmit(&hspi1, data, w*h*sizeof(uint16_t), data);

	ili9341_cs_hi();
}
