// PIC18F45K22 Configuration Bit Settings
#pragma config FOSC = INTIO67
#pragma config WDTEN = OFF
#pragma config MCLRE = EXTMCLR
#pragma config PBADEN = OFF

#define _XTAL_FREQ 48000000UL

#include <xc.h>
#include <stdlib.h>
#include <stdio.h> // Necesario para sprintf
#include "i2c.h"
#include "i2c_lcd.h"
#include "uart.h" 

extern unsigned char murcielago_x_anterior;

unsigned int score = 0; // Variable global para el puntaje

void setup_timer0_for_seed() {  
    T0CONbits.TMR0ON = 0;
    T0CON = 0x07;
    TMR0H = 0; TMR0L = 0;
    T0CONbits.TMR0ON = 1;
}

unsigned int get_timer0_value() {
    T0CONbits.TMR0ON = 0;
    unsigned int val = (TMR0H << 8) | TMR0L;
    T0CONbits.TMR0ON = 1;
    return val;
}

void main()
{
    setup_timer0_for_seed();

    I2C_init();
    UART_Init(); 
    
    ANSELB = 0; 
    TRISBbits.TRISB0 = 1; 
    TRISBbits.TRISB1 = 0; 
    LATBbits.LATB1 = 0; 

    lcd_init(); 
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_write_string("Presiona boton");
    lcd_set_cursor(1, 0);
    lcd_write_string("para empezar...");
    
    // Mensaje de "Conectado" por UART
    UART_WriteString("Conectado\r\n"); 
    
    while (leer_dip_switch() != 0) {
        // Esperar mientras el botón NO esté presionado
    }
    
    srand(get_timer0_value());

    lcd_createChar(0, murcielago_char);
    lcd_createChar(1, obstaculo_char);

    score = 0; // Reiniciar puntaje al inicio del juego
    // Mostrar puntaje inicial por UART
    UART_WriteString("Juego iniciado! Puntos: ");
    reiniciar_juego(); // Esto también inicializa los obstáculos

    while (1)
    {
        dibujar_obstaculos_actuales();

        mover_murcielago();

        // Si el murciélago completa un ciclo de pantalla
        if (murcielago_x == 0 && murcielago_x_anterior == 15) {
            score++; // Incrementa el puntaje
            // Enviar puntaje actualizado por UART
            UART_WriteString("Puntos: ");
            UART_WriteString("\r\n");
            reiniciar_juego(); // Genera nuevos obstáculos
        }

        if (leer_dip_switch() == 0) {
            cambiar_carril();
            LATBbits.LATB1 = 1;
            __delay_ms(10);
        } else {
            LATBbits.LATB1 = 0;
        }

        if (detectar_colision()) {
            lcd_clear();
            lcd_set_cursor(0, 0);
            lcd_write_string("GAME OVER!");
            lcd_set_cursor(1, 0); // Muestra el puntaje final en la segunda línea de la LCD
            lcd_write_string("Puntos: ");
            
            char score_str[6]; // Buffer para convertir el entero a cadena
            sprintf(score_str, "%u", score); // Convierte el puntaje a cadena
            lcd_write_string(score_str); // Muestra la cadena en la LCD

            // Enviar mensaje de Game Over con puntaje final por UART
            UART_WriteString("GAME OVER! Puntos finales: ");
            
            __delay_ms(50);
            score = 0; // Reiniciar puntaje para la nueva partida
            // Enviar mensaje de juego reiniciado con puntaje a cero por UART
            UART_WriteString("Juego Reiniciado. Puntos: ");
            reiniciar_juego();
        }
        
        __delay_ms(5);
    }
}