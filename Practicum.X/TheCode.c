#include <xc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "i2c_thingy.h"
#include "oTHER_THINGY.h"

#define _XTAL_FREQ 4000000  // 4MHz internal clock

// Configuration Bits for PIC16F1829
#pragma config FOSC = INTOSC
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config CLKOUTEN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = ON

// Function Prototypes
unsigned int ADC_Read(unsigned char channel);
void ADC_Initialize(void);
void clockAndpin_config(void);

// I/O Setup
void clockAndpin_config(void) {
    OSCCON = 0x68;       // 4MHz internal
    ANSELA = 0x03;       // RA0 and RA1 as analog
    ANSELB = 0x00;
    TRISA |= (1 << 0);   // RA0 = input (water)
    TRISA |= (1 << 1);   // RA1 = input (light)
}

// ADC Setup
void ADC_Initialize() {
    ADCON0 = 0b00000001;  // Start with AN0
    ADCON1 = 0b11000000;  // Right justified, FRC clock
}

// ADC Read Function
unsigned int ADC_Read(unsigned char channel) {
    ADCON0 = (channel << 2) | 0x01; // Select channel, turn on
    __delay_us(30);
    GO_nDONE = 1;
    while(GO_nDONE);
    return (ADRESH << 8) + ADRESL;
}

// Main
void main() {
    unsigned int water_level;
    unsigned int light_level;
    char buffer[16];

    clockAndpin_config();
    ADC_Initialize();
    i2c_Init();
    SSD1306_Init();

    SSD1306_Fill(0);
    __delay_ms(1000);
    SSD1306_Out16(0, 6, "UTSA", 1);
    SSD1306_Out16(3, 2, "DECEPTICONS", 1);
    __delay_ms(2000);
    SSD1306_Clear();

    while (1) {
        water_level = ADC_Read(0);  // RA0
        light_level = ADC_Read(1);  // RA1

        // Display water level
        sprintf(buffer, "Water: %-4u", water_level);
        SSD1306_Out16(0, 0, buffer, 1);

        // Display light level
        sprintf(buffer, "Light: %-4u", light_level);
        SSD1306_Out16(2, 0, buffer, 1);

        __delay_ms(1000);
    }
}