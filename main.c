#include <stdio.h>
#include "pico/stdlib.h"

/* Estructura para el LCD */
typedef struct {
  uint8_t rs;		/* GPIO de RS */
  uint8_t en;		/* GPIO de EN */
  uint8_t d4;		/* GPIO de D4 */
  uint8_t d5;		/* GPIO de D5 */
  uint8_t d6;		/* GPIO de D6 */
  uint8_t d7;		/* GPIO de D7 */
} lcd_t;

/* Inline functions */

/* Genera una estructura por defecto para el LCD */
static inline lcd_t lcd_get_default_config(void) {
  lcd_t lcd = { 0, 1, 2, 3, 4, 5 };
  return lcd;
}

/* Obtiene la mascara de pines de datos */
static inline uint32_t lcd_get_data_mask(lcd_t lcd) {
  return 1 << lcd.d4 | 1 << lcd.d5 | 1 << lcd.d6 | 1 << lcd.d7;
}

/* Obtiene la mascara de pines conectadas al LCD */
static inline uint32_t lcd_get_mask(lcd_t lcd) {
  return 1 << lcd.rs | 1 << lcd.en | lcd_get_data_mask(lcd);
}

/* Function prototypes */
void lcd_init(lcd_t lcd);
void lcd_put_nibble(lcd_t lcd, uint8_t nibble);
void lcd_put_command(lcd_t lcd, uint8_t cmd);
void lcd_putc(lcd_t lcd, char c);
void lcd_puts(lcd_t lcd, const char* str);
void lcd_clear(lcd_t lcd);
void lcd_go_to_xy(lcd_t lcd, uint8_t x,  uint8_t y);

/* Programa principal */

int main() {
	/* Inicializo el USB */
  stdio_init_all();

	gpio_init(6);
	gpio_init(7);
	gpio_init(8);
	gpio_init(9);
	gpio_init(13);
	gpio_set_dir(6,0);
	gpio_set_dir(7,0);
  gpio_set_dir(8,0);
	gpio_set_dir(9,0);
	gpio_set_dir(13,1);

	/* Obtengo una variable para elegir los pines del LCD */
  lcd_t lcd = lcd_get_default_config();
	/* Por defecto, los pines son:
	 	- GP0 (RS)
		- GP1 (EN) 
		- GP2 (D4)
		- GP3 (D5)
		- GP4 (D6)
		- GP5 (D7)
		
		Si quieren cambiar alguno pueden hacerlo como lcd.d5 = 6 por ejemplo */
  
	/* Inicializo el LCD */
	lcd_init(lcd);

  while (true) {
	
		bool b1 = gpio_get(6);
		bool b2 = gpio_get(7);
		bool b3 = gpio_get(8);
		bool b4 = gpio_get(9);
		char cerrados[4] ={' ', ' ', ' ', ' '};
		char abiertos[4] ={' ', ' ', ' ', ' '};
		int x= 0;
		int y= 0;

		if (b1){
			cerrados[x] = '6';
			x =x+1;
		}
		else{
			abiertos[y] = '6';
			y=y+1;
		}
		if (b2){
			cerrados[x] = '7';
			x =x+1;
		}
		else{
			abiertos[y] = '7';
			y=y+1;
		}
		if (b3){
			cerrados[x] = '8';
			x =x+1;
		}
		else{
			abiertos[y] = '8';
			y=y+1;
		}
		if (b4){
			cerrados[x] = '9';
			x =x+1;
		}
		else{
			abiertos[y] = '9';
			y=y+1;
		}
		
		
		

	/* Limpio el LCD. Por defecto, va al 0;0 */
    lcd_clear(lcd);
		/* Escribo en la pantalla */
		lcd_puts(lcd, "Abiertos:");

		/* Imprime un mensaje en la pantalla */
		for(int i= 0; i<y; i++)
		{
			if (abiertos[i]!= ' '||abiertos[i]!= '\0' ){
			lcd_putc(lcd, abiertos[i]);
			if (abiertos[i+1]!= ' '){
			lcd_putc(lcd,'|');
				}
			}
		}

	 
		/* Voy a la columna 6, fila 1 */
		lcd_go_to_xy(lcd, 0, 1);
		/* Imprime un mensaje en la pantalla */
    lcd_puts(lcd, "Cerrados:");

		for(int i= 0; i<x; i++)
		{
			if (cerrados[i] != ' '|| cerrados[i] != '\0'){
			lcd_putc(lcd, cerrados[i]);
			if (cerrados[i+1]!= ' '){
			lcd_putc(lcd,'|');
					}
				}
			}
		
		
    /* Espero medio segundo */
		sleep_ms(500);
  }
}

/* Ignoren todo lo de abajo. Son las funciones que pueden usar */

/*
 * 	@brief	Initialize LCD with default configuration
 * 	@param	lcd: struct to an LCD
 */
void lcd_init(lcd_t lcd) {
  /* Get pin mask */
  uint32_t mask = lcd_get_mask(lcd);
  /* Initialize GPIO pins */
  gpio_init_mask(mask);
  /* Set pins direction as output */
  gpio_set_dir_out_masked(mask);
	/* Function set: 8 bits interface length */
	lcd_put_command(lcd, 0x03);
	/* Wait for 4.1 ms */
	sleep_ms(5);
	/* Function set: 8 bits interface length */
	lcd_put_command(lcd, 0x03);
	/* Wait for 100 us */
	sleep_us(100);
	/* Function set: 8 bits interface length */
	lcd_put_command(lcd, 0x03);
	/* Now the other instructions can be send */

	/* Function set: set interface to 4 bits length */
	lcd_put_command(lcd, 0x02);
	/* Function set: interface is 4 bits length */
	lcd_put_command(lcd, 0x02);
  /* Function set: two row display and 5x7 font */
  lcd_put_command(lcd, 0x08 | (false << 2));
  /* Display on, cursor off, blink off */
  lcd_put_command(lcd, 0x00);
  lcd_put_command(lcd, (3 << 2) | (false) | false);
  /* Input set command: increment cursor */
  lcd_put_command(lcd, 0x00);
  lcd_put_command(lcd, 0x06);
}

/*
 * 	@brief	Write a new nibble in data pins
 * 	@param	lcd: LCD pin struct
 * 	@param	nibble: four bit data
 */
void lcd_put(lcd_t lcd, uint8_t nibble) {
  /* Get data pins mask */
  uint32_t mask = lcd_get_data_mask(lcd);
	/* Get value from nibble */
	uint32_t value = 	((nibble & 0x8)? true : false) << lcd.d7 | 
										((nibble & 0x4)? true : false) << lcd.d6 |
										((nibble & 0x2)? true : false) << lcd.d5 | 
										((nibble & 0x1)? true : false) << lcd.d4;
  /* Send nibble */
  gpio_put_masked(mask, value);
	/* Set enable pin */
	gpio_put(lcd.en, true);
	/* Wait for 40 us */
	sleep_us(40);
	/* Clear enable pin */
	gpio_put(lcd.en, false);
}

/*
 * 	@brief	Write a new command in data pins
 * 	@param	lcd: LCD pin struct
 * 	@param	cmd: four bit command
 */
void lcd_put_command(lcd_t lcd, uint8_t cmd) {
	/* Clear rs pin */
	gpio_put(lcd.rs, false);
	/* Put command in data pins */
	lcd_put(lcd, cmd);
}

/*
 * 	@brief	Write a new character in data pins
 * 	@param	lcd: LCD pin struct
 * 	@param	c: char to be sent
 */
void lcd_putc(lcd_t lcd, char c) {
	/* Set rs pin */
	gpio_put(lcd.rs, true);
	/* Repeat twice */
	for(uint8_t nibble = 0; nibble < 2; nibble++) {
		/* Send lower nibble if it's the first time. Send higher nibble if second */
		uint8_t n = (nibble)? c & 0x0f : c >> 4;
		/* Write nibble */
		lcd_put(lcd, n);
	}
}

/*
 * 	@brief	Write a new string in LCD
 * 	@param	lcd: LCD pin struct
 * 	@param	str: string to be sent
 */
void lcd_puts(lcd_t lcd, const char* str) {
	/* Loop intul null character */
	while(*str) {
		/* Write the char */
		lcd_putc(lcd, *str);
		/* Increment pointer */
		str++;
	}
}

/*
 * 	@brief	Clear screen screen
 * 	@param	lcd: LCD pin struct
 */
void lcd_clear(lcd_t lcd) {
	/* Send clear command:  first nibble */
	lcd_put_command(lcd, 0x00);
	/* Send clear command: second nibble */
	lcd_put_command(lcd, 0x01);
	/* Wait for 4ms */
	sleep_ms(4);
}

/*
 * 	@brief	Go to starting coordinate
 * 	@param	lcd: LCD pin struct
 * 	@param	x: column number (starting in 0)
 * 	@param	y: row number (starting in 0)
 */
void lcd_go_to_xy(lcd_t lcd, uint8_t x, uint8_t y) {
	/* Auxiliary variable */
	uint8_t aux;
	/* Check if the first row (0) is required */
	if(y == 0) {
		/* Set direction */
		aux = 0x40 + x;
		/* Send first nibble */
		lcd_put_command(lcd, aux >> 4);
		/* Send seconds byte */
		lcd_put_command(lcd, aux & 0x0F);
	}
	/* Check if the second row (2) is required  */
	else if (y == 1) {
		/* Set direction */
		aux = 0xC0 + x;;
		/* Send first nibble */
		lcd_put_command(lcd, aux >> 4);
		/* Eend second nibble */
		lcd_put_command(lcd, aux & 0x0F);
	}
}
