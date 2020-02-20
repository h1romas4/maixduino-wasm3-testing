#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "maixduino.h"
#include "lcd/lcd.h"

#define PLL1_OUTPUT_FREQ 400000000UL

uint16_t getValueByRotation(uint8_t rotation);

//
// for Maixduino
//
void k210_begin(void)
{
    // use 8MB memory @see lds/kendryte.ld
    //
    // The AI SRAM memory is only accessible if the following conditions are met:
    //  PLL1 enabled and clock system configuration is correct
    //  KPU not performing neural network calculations
    sysctl_pll_set_freq(SYSCTL_PLL1, PLL1_OUTPUT_FREQ);
    sysctl_clock_enable(SYSCTL_CLOCK_AI);

    // set clock 600MHz
    sysctl_cpu_set_freq(600 * 1000 * 1000);
}

//
// for Maixduino (SPI0)
//
void lcd_begin(void)
{
    fpioa_set_function(SIPEED_ST7789_SS_PIN  , (fpioa_function_t)(FUNC_SPI0_SS0 + SIPEED_ST7789_SS));
    fpioa_set_function(SIPEED_ST7789_SCLK_PIN, (fpioa_function_t)FUNC_SPI0_SCLK);
    sysctl_set_spi0_dvp_data(1);
    lcd_init(0, SIPEED_ST7789_SS, SIPEED_ST7789_RST_GPIONUM, SIPEED_ST7789_DCX_GPIONUM, 20000000, SIPEED_ST7789_RST_PIN, SIPEED_ST7789_DCX_PIN, 3);
    lcd_set_direction(getValueByRotation(0));
    lcd_clear(0x0000);
}

uint16_t getValueByRotation(uint8_t rotation)
{
    uint16_t v = DIR_YX_RLDU;
    switch(rotation) {
        case 0:
            v = DIR_YX_RLDU;
            break;
        case 1:
            v = DIR_XY_RLUD;
            break;
        case 2:
            v = DIR_YX_LRUD;
            break;
        case 3:
            v = DIR_XY_LRDU;
            break;
    }
    return v;
}
