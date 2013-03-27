/*---------------------------------------------------------------------------*/
/*               Raspberry Pi Funduino I2C 1602 LCD Driver
 *
 *          LCD attached to I2C bus on Raspberry Pi via PCF8574
 *          The PCF8574 enables control of 1602LCD via 4 bit mode
 *
 /*---------------------------------------------------------------------------*/
#ifndef Funduino_I2C_LCD1602_h
#define Funduino_I2C_LCD1602_h

/* Initialization */
#define LCD_4BIT_INIT_VALUE 0x02
#define LCD_INIT_VALUE 0x03

/* Commands */
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

/* Flags for display entry mode */
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

/* Flags for display on/off control */
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

/* Flags for display/cursor shift */
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

/* Flags for function set */
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

/* Register Bank */
#define INSTRUCTION_REG 0
#define DATA_REG 1

/* Address of the Funduino I2C 1602 LCD */
#define LCD_I2C_ADDRESS 0x27

/* Display size */
#define LCD_ROWS  2
#define LCD_COLS 16

/*
 Pin attachment:
 P0 : RS, P1 : RW, P2 : E, P3 : Backlight, P4 : DB04, P5 : DB05, P6 : DB06, P7 : DB07 */

/* DB04-DB07
 Four high order bidirectional tristate data bus
 pins. Used for data transfer and receive between
 the MPU and the HD44780U. DB7 can be used
 as a busy flag.
 */
#define D7 0b10000000
#define D6 0b01000000
#define D5 0b00100000
#define D4 0b00010000

/* Backlight pin */
#define BL 0b00001000

/* E - Starts data read/write
 */
#define En 0b00000100

/* R/W - Selects read or write.
 0: Write
 1: Read
 */
#define RW 0b00100010

/* RS - Selects registers
 0: Instruction register (for write) Busy flag: address counter (for read)
 1: Data register (for write and read)
 */
#define RS 0b00000001


#endif