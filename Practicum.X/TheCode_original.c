#include <xc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "i2c_thingy.h"
#include "oTHER_THINGY.h"

#define _XTAL_FREQ 4000000      // 4MHz clock

// Configuration Bits for PIC16F1928
#pragma config FOSC = INTOSC    // Internal oscillator
#pragma config WDTE = OFF       // Watchdog Timer disabled
#pragma config PWRTE = OFF      // Power-up Timer disabled
#pragma config MCLRE = ON       // MCLR Pin enabled
#pragma config CP = OFF         // Flash Program Memory Protection disabled
#pragma config CPD = OFF        // Data Memory Code Protection disabled
#pragma config BOREN = OFF      // Brown-out Reset disabled
#pragma config CLKOUTEN = OFF   // Clock Out disabled
#pragma config IESO = OFF       // Internal/External Switchover disabled
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor disabled

// Function Prototypes
unsigned int ADC_Read(void);
void ADC_Initialize(void);
void clockAndpin_config(void);

void main() {
    unsigned int pot_value;
    char buffer[10];
    
    // Initialize Hardware
    clockAndpin_config();
    ADC_Initialize();
    i2c_Init();
    SSD1306_Init();
    
    // Display UTSA splash screen (preloaded in memory)
    SSD1306_Fill(0);
    __delay_ms(2000);
    
    // Display Group Name for 2 seconds
    SSD1306_Out16(2, 2, "thicc", 1);
    __delay_ms(2000);
    SSD1306_Clear();
    
    while(1) {
        // Read POT value
        pot_value = ADC_Read();
        
        // Convert to string with spacing to clear old numbers
        sprintf(buffer, "Pot=%d    ", pot_value);
        
        // Display value on OLED
        SSD1306_Out16(4, 4, buffer, 1);
        
        __delay_ms(500); // Refresh delay
    }
}

void clockAndpin_config(void) {
    OSCCON = 0x68;                      // 4MHz internal oscillator
    ANSELA = 0;                         // All PORTA digital
    ANSELB = 0x20;                      // RB5 (AN11) analog
    TRISA = 0x00;                       // PORTA outputs
    TRISB = 0xA0;                       // RB7 (button) and RB5 (POT) as inputs
    LATA = 0x24;                        // Initial LED states
}

void ADC_Initialize() {
    ADCON0 = 0b00101101;                // Enable ADC, channel AN11 (RB5)
    ADCON1 = 0b10000000;                // Right justified, Fosc/32 clock
}

unsigned int ADC_Read() {
    __delay_ms(2);                      // Acquisition time
    ADCON0bits.GO = 1;                  // Start conversion
    while(ADCON0bits.GO);               // Wait for completion
    return (ADRESH << 8) + ADRESL;      // Return 10-bit result
}