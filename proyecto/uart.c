#include "uart.h"
#include <stdio.h>


void UART_Init(void) {
    TRISC6 = 0; // TX como salida
    TRISC7 = 1; // RX como entrada

    SPBRG1 = 25; // Baudrate 9600 para Fosc = 16MHz
    TXSTA1bits.BRGH = 0; // Baja velocidad (BRGH = 0)
    BAUDCON1bits.BRG16 = 0; // 8 bits del generador de baudrate

    RCSTA1bits.SPEN = 1; // Habilita el m�dulo serial
    TXSTA1bits.SYNC = 0; // Modo as�ncrono
    TXSTA1bits.TXEN = 1; // Habilita transmisi�n
    RCSTA1bits.CREN = 1; // Habilita recepci�n

    
    PIE1bits.RC1IE = 1;   // Habilita interrupci�n por recepci�n UART
    PIR1bits.RC1IF = 0;   // Limpia bandera
    INTCONbits.PEIE = 1;  // Habilita interrupciones perif�ricas
    INTCONbits.GIE = 1;   // Habilita interrupciones globales
}

void UART_WriteChar(char data) {
    while (!TXSTA1bits.TRMT); // Espera a que se vac�e el buffer de transmisi�n
    TXREG1 = data;
}

void UART_WriteString(const char* str) {
    while (*str) {
        UART_WriteChar(*str++);
    }
}