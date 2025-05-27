#ifndef LCD_I2C_H
#define LCD_I2C_H

#define _XTAL_FREQ 48000000UL

#define ADDRESS_LCD 0x4E
// #define ADDRESS_LCD 0x7E

#define PULSADOR_PIN PORTBbits.RB0
#define TRIS_PULSADOR TRISBbits.TRISB0
#define ANSEL_PULSADOR ANSELBbits.ANSB0

#define MIN_ESPACIO_LIBRE_ENTRE_OBSTACULOS 2 // Para garantizar al menos 1 carácter vacío entre obstáculos

void lcd_init(void);
void lcd_cmd(unsigned char cmd);
void lcd_set_cursor(unsigned char row, unsigned char col);
void lcd_write_char(char c);
void lcd_write_string(const char *str);
void lcd_clear(void);
void lcd_createChar(unsigned char location, const unsigned char *charmap);

extern unsigned char murcielago_y;
extern unsigned char murcielago_x;
extern unsigned char murcielago_x_anterior;

extern const unsigned char murcielago_char[8];
extern const unsigned char obstaculo_char[8];

typedef struct {
    unsigned char x;
    unsigned char y;
} Obstaculo;

#define MAX_OBSTACULOS 4

extern Obstaculo obstaculos_juego[MAX_OBSTACULOS];

void inicializar_obstaculos(void);
void dibujar_obstaculos_actuales(void); // Nuevo prototipo

int leer_dip_switch(void);
void mover_murcielago(void);
void reiniciar_juego(void);
unsigned char detectar_colision(void);
void cambiar_carril(void);

#endif