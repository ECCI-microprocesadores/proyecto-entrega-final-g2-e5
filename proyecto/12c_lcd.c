#include <xc.h>
#include <stdlib.h> 
#include "i2c.h"
#include "i2c_lcd.h"

unsigned char murcielago_y = 1;
unsigned char murcielago_x = 0;
unsigned char murcielago_x_anterior = 0;

Obstaculo obstaculos_juego[MAX_OBSTACULOS];

const unsigned char murcielago_char[8] = {
    0b00000, 0b00100, 0b01110, 0b11111, 0b01110, 0b00100, 0b00000, 0b00000
};

const unsigned char obstaculo_char[8] = {
    0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111
};

void inicializar_obstaculos() {
    unsigned char i;
    unsigned char nueva_y;
    
    lcd_clear();

    const unsigned char espacio_necesario = (MAX_OBSTACULOS * 1) + ((MAX_OBSTACULOS - 1) * (MIN_ESPACIO_LIBRE_ENTRE_OBSTACULOS - 1));

    unsigned char inicio_base = 3; 

    unsigned char max_desplazamiento_inicio = 15 - inicio_base - espacio_necesario;
    if (max_desplazamiento_inicio > 5) max_desplazamiento_inicio = 5; 
    
    unsigned char desplazamiento_global = 0;
    if (max_desplazamiento_inicio > 0) {
        desplazamiento_global = (unsigned char)(rand() % (max_desplazamiento_inicio + 1));
    }

    unsigned char ultima_columna_fija = inicio_base + desplazamiento_global;

    for (i = 0; i < MAX_OBSTACULOS; i++) {
        unsigned char nueva_x = ultima_columna_fija;
        
        ultima_columna_fija += 1 + (MIN_ESPACIO_LIBRE_ENTRE_OBSTACULOS - 1); 

        nueva_y = (unsigned char)(rand() % 2); 

        obstaculos_juego[i].x = nueva_x;
        obstaculos_juego[i].y = nueva_y;
    }
    
    // Dibuja los obstáculos por primera vez
    for (i = 0; i < MAX_OBSTACULOS; i++) { 
        lcd_set_cursor(obstaculos_juego[i].y, obstaculos_juego[i].x);
        lcd_write_char(1);
    }
}

// Nueva función: Redibuja todos los obstáculos
void dibujar_obstaculos_actuales() {
    unsigned char i;
    for (i = 0; i < MAX_OBSTACULOS; i++) {
        if (obstaculos_juego[i].x <= 15 && obstaculos_juego[i].y <= 1) { 
            lcd_set_cursor(obstaculos_juego[i].y, obstaculos_juego[i].x);
            lcd_write_char(1);
        }
    }
}


void mover_murcielago() {
    murcielago_x_anterior = murcielago_x;

    lcd_set_cursor(murcielago_y, murcielago_x);
    lcd_write_char(' '); // Borra posición anterior
    murcielago_x++;
    if (murcielago_x >= 16) {
        murcielago_x = 0;
    }
    lcd_set_cursor(murcielago_y, murcielago_x);
    lcd_write_char(0); // Dibuja en nueva posición
}

void lcd_init(void)
{
    __delay_ms(20);
    lcd_cmd(0x33);
    lcd_cmd(0x32);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
    __delay_ms(3);
}

void lcd_cmd(unsigned char cmd)
{
    char data_u, data_l;
    data_u = (cmd & 0xF0);
    data_l = ((cmd << 4) & 0xF0);

    I2C_start();
    I2C_write(ADDRESS_LCD);
    I2C_write(data_u | 0x0C);
    I2C_write(data_u | 0x08);
    I2C_write(data_l | 0x0C);
    I2C_write(data_l | 0x08);
    I2C_stop();
}

void lcd_write_char(char c)
{
    char data_u, data_l;
    data_u = (c & 0xF0);
    data_l = ((c << 4) & 0xF0);

    I2C_start();
    I2C_write(ADDRESS_LCD);
    I2C_write(data_u | 0x0D);
    I2C_write(data_u | 0x09);
    I2C_write(data_l | 0x0D);
    I2C_write(data_l | 0x09);
    I2C_stop();
}

void lcd_set_cursor(unsigned char row, unsigned char col)
{
    if (row == 0) lcd_cmd(0x80 + col);
    else lcd_cmd(0xC0 + col);
}

void lcd_write_string(const char *str)
{
    while(*str != '\0')
    {
        lcd_write_char(*str++);
    }
}

void lcd_clear(void)
{
    lcd_cmd(0x01);
    __delay_ms(2);
}

void lcd_createChar(unsigned char location, const unsigned char *charmap) // <--- ASÍ DEBE SER
{
    unsigned char i;
    location &= 0x07;
    lcd_cmd((unsigned char)(0x40 | (location << 3)));
    for (i = 0; i < 8; i++) {
        lcd_write_char(charmap[i]);
    }
}

int leer_dip_switch() {
    return PULSADOR_PIN;
}

unsigned char detectar_colision() {
    unsigned char i;
    for (i = 0; i < MAX_OBSTACULOS; i++) {
        if (murcielago_x == obstaculos_juego[i].x && murcielago_y == obstaculos_juego[i].y) {
            return 1;
        }
    }
    return 0;
}

void reiniciar_juego() {
    murcielago_x = 0;
    murcielago_y = 1;

    inicializar_obstaculos();

    lcd_set_cursor(murcielago_y, murcielago_x);
    lcd_write_char(0);
}

void cambiar_carril() {
    lcd_set_cursor(murcielago_y, murcielago_x);
    lcd_write_char(' '); // Borra posición anterior
    murcielago_y = (murcielago_y == 0) ? 1 : 0;
    lcd_set_cursor(murcielago_y, murcielago_x);
    lcd_write_char(0); // Dibuja en nueva posición
}