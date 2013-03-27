/*---------------------------------------------------------------------------*/
/*               Raspberry Pi Funduino I2C 1602 LCD Driver
 *
 *          LCD attached to I2C bus on Raspberry Pi via PCF8574
 *          The PCF8574 enables control of 1602LCD via 4 bit mode
 *
 /*---------------------------------------------------------------------------*/

/* Linux header files */
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* Include Funduiono specific header file */
#include "Funduino_I2C_LCD1602.h"

#define LCD_COMMAND_TRACE 0

/*----------------------------------------------------------------------------
 * Global variables
 *---------------------------------------------------------------------------*/

/* File descriptor for character device */
int fd = 0;

/*************************************************************************************
 *
 * Function:     ByteToBinary
 *
 * Description:  This function is used for translating a byte into a binary number
 *
 * Parameters:   Byte value to convert to binary
 *
 * Return:       Pointer to static memory string containing binary value
 *
 *************************************************************************************/
const char *ByteToBinary(char x)
{
  static char b[9];
  int z;
    
  b[0] = '\0';
    
  for (z = 128; z > 0; z >>= 1)
  {
    strcat(b, ((x & z) == z) ? "1" : "0");
  }
  
  return b;
}

/*************************************************************************************
 *
 * Function:     PrintProgramArgument
 *
 * Description:  Simple trace function for tracing the text given in the program argument
 *
 * Parameters:   *text: pointer to the string, size: number of characters to print
 *
 * Return:       void
 *
 *************************************************************************************/
void PrintProgramArgument(char *text, unsigned int size)
{
  int i;
  
  printf("Printing following text to LCD:\n",text);
  
  for(i = 0; i <= strlen(text)-1; i++)
  {
    /* Check if the size increases one line */
    if(i == LCD_COLS)
    {
      printf("\n");
    }
    
    /* Write the character to the LCD */
    printf("%c", text[i]);
  }
  printf("\n");
}

/*************************************************************************************
 *
 * Function:     ConnectI2CSlave
 *
 * Description:  Connects to the I2C bus and registers a file handle for the slave given
 *               in the specified address
 *
 * Parameters:   *FileName: character device string, Address: Address of the slave
 *
 * Return:       File desciptor handle
 *
 *************************************************************************************/
int ConnectI2CSlave(char *FileName, unsigned int Address)
{
  /* File desciptor handle */
  int filedesciptor;
  
  /* Open the character device for the I2C bus */
  if ((filedesciptor = open(FileName, O_RDWR)) < 0)
  {
    printf("Failed to open i2c port\n");
    exit(1);
  }
    
  /* Open I2C bus as slave on address */
  if (ioctl(filedesciptor, I2C_SLAVE, Address) < 0)
  {
    printf("Unable to get bus access to talk to slave\n");
    exit(1);
  }
  
  return filedesciptor;
}

/*************************************************************************************
 *
 * Function:     WriteByteI2C
 *
 * Description:  This functions writes a byte to the PCF8574 which is the translated
 *               into 8 bits attached to the LCD
 *
 * Parameters:   Byte to send
 *
 * Return:       void
 *
 *************************************************************************************/
void WriteByteI2C(char byteval)
{
    char buf[2];
    buf[0] = byteval;

    if(fd == 0)
    {
        printf("Error file descriptor not initialized!\n");
        exit(1);
    }

    /* Write the byte to I2C bus */
    if ((write(fd, buf, 1)) != 1)
    {
        printf("Error writing to i2c slave1\n");
        exit(1);
    }
  
#if LCD_COMMAND_TRACE
    /* Trace the value in binary */
    printf("Wrote 0b%s\n", ByteToBinary(buf[0]));
#endif
}

/*************************************************************************************
 *
 * Function:     PulseEnable
 *
 * Description:  This functions triggers the data in value via E pin
 *
 * Parameters:   Byte to trigger
 *
 * Return:       void
 *
 *************************************************************************************/
void PulseEnable(char value)
{
    /* Backlight on */
    value |= BL;
    
    /* Set the Enable pin high and keep backlight on */
    WriteByteI2C(value | En);
    
    /* Wait at least 450ns while E pin is high */
    usleep(1);
    
    /* Set the Enable pin low again */
    WriteByteI2C(value & ~En);
    
    /* Commands needs at least 37us to settle */
    usleep(50);
}

/*************************************************************************************
 *
 * Function:     WriteLcdDataByte
 *
 * Description:  Function used to split 8 bit commands into two 4 bit commands.
 *               Note: This can only be used after setting LCD module in 4 bit mode
 *
 * Parameters:   byte: Byte value to trigger, Bank: Setting of RS bit
 *
 * Return:       void
 *
 *************************************************************************************/
void WriteLcdDataByte(char byte, unsigned char Bank)
{
    unsigned char highnib;
    unsigned char lownib;
    
    if(Bank == INSTRUCTION_REG)
    {
        lownib = (byte & 0x0F) << 4 | BL;
        highnib = (byte & 0xF0) | BL;
    }
    
    if(Bank == DATA_REG)
    {
        lownib = (byte & 0x0F) << 4 | BL | RS;
        highnib = (byte & 0xF0) | BL | RS;
    }
    
    /* First write the 4 MSB bits */
    WriteByteI2C(highnib);
    
    /* Trigger the data */
    PulseEnable(highnib);
    
    /* Now write the 4 LSB bits */
    WriteByteI2C(lownib);
    
    /* Trigger the data */
    PulseEnable(lownib);
}

/*************************************************************************************
 *
 * Function:     WriteLcdDataNibble
 *
 * Description:  Function that writes only 4 bits to the LCD
 *
 * Parameters:   Nibble value to trigger
 *
 * Return:       void
 *
 *************************************************************************************/
void WriteLcdDataNibble(unsigned char nibble)
{
  /* High nibble */
  unsigned char nibble_val = (nibble & 0x0F) << 4 | BL;
    
  /* Write the byte to I2C (nibble value in above 4 bits) */
  WriteByteI2C(nibble_val);
    
  /* Trigger the data */
  PulseEnable(nibble_val);
}

/*************************************************************************************
 *
 * Function:     SetCursor
 *
 * Description:  Sets the display cursor to the requested row and column on the row
 *
 * Parameters:   Row and column position
 *
 * Return:       void
 *
 *************************************************************************************/
void SetCursor(unsigned int row, unsigned int col)
{
  int row_offsets[] = { 0x00, 0x40 };
  WriteLcdDataByte(LCD_SETDDRAMADDR + row_offsets[row] + col, INSTRUCTION_REG);
}

/*************************************************************************************
 *
 * Function:     CreateChar
 *
 * Description:  Fills one of the first 8 CGRAM locations with custom characters
 *
 * Parameters:   location: Postition 1 to 8, charmap[]: The character data
 *
 * Return:       void
 *
 *************************************************************************************/
void CreateChar(unsigned int location, unsigned char charmap[])
{
  int i;
  location &= 0x7; // we only have 8 locations 0-7
  WriteLcdDataByte(LCD_SETCGRAMADDR | (location << 3), INSTRUCTION_REG);
  
  for (i=0; i<8; i++)
  {
    WriteLcdDataByte(charmap[i], DATA_REG);
  }
}

/*************************************************************************************
 *
 * Function:     Clear
 *
 * Description:  Clears the entire LCD display
 *
 * Parameters:   void
 *
 * Return:       void
 *
 *************************************************************************************/
void Clear()
{
  /* Clear display, Set cursor position to zero */
  WriteLcdDataByte(LCD_CLEARDISPLAY, INSTRUCTION_REG); 
  usleep(2000);
}

/*************************************************************************************
 *
 * Function:     Home
 *
 * Description:  Moves the curser to line 1 position 0
 *
 * Parameters:   void
 *
 * Return:       void
 *
 *************************************************************************************/
void Home()
{
  /* Clear display, Set cursor position to zero */
  WriteLcdDataByte(LCD_RETURNHOME, INSTRUCTION_REG);
  usleep(2000);
}

/*************************************************************************************
 *
 * Function:     PrintText
 *
 * Description:  Prints the number of characters given in size from the *text. When
 *               the size increases 16 chars the cursor is shifted to the next line
 *
 * Parameters:   *text: pointer to the string, size: number of characters to write
 *
 * Return:       void
 *
 *************************************************************************************/
void PrintText(char *text, unsigned int size)
{
  int i;
  
  for(i = 0; i <= size; i++)
    {
    /* Check if the size increases one row */
    if(i == LCD_COLS)
      {
      SetCursor(1, 0);
      }
    
    /* Write the character to the LCD */
    WriteLcdDataByte(text[i], DATA_REG);
    }
}

/*************************************************************************************
 *
 * Function:     InitializeLCD
 *
 * Description:  Initializes the LCD in 4 bit operation with 2 lines and no blinking
 *               cursor with left entrymode
 *
 * Parameters:   void
 *
 * Return:       void
 *
 *************************************************************************************/
void InitializeLCD()
{
  WriteLcdDataNibble(LCD_INIT_VALUE);
  usleep(5000);
  
  WriteLcdDataNibble(LCD_INIT_VALUE);
  usleep(5000);
  
  WriteLcdDataNibble(LCD_INIT_VALUE);
  usleep(5000);
  
  WriteLcdDataNibble(LCD_4BIT_INIT_VALUE);
  usleep(5000);
  
  WriteLcdDataByte(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE, INSTRUCTION_REG);
  usleep(5000);
  
  WriteLcdDataByte(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKOFF, INSTRUCTION_REG);
  
  WriteLcdDataByte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, INSTRUCTION_REG);
  
  Clear();
}

/*************************************************************************************
 *
 * Function:     main
 *
 * Description:  Program main loop
 *
 * Parameters:   Command line parameters
 *
 * Return:       void
 *
 *************************************************************************************/
int main(int argc, char **argv)
{
  char *text = argv[1];
  char *Filename = "/dev/i2c-1";
  
  /* Trace the program argument */
  PrintProgramArgument(text, strlen(text)-1);
  
  /* Connect to the LCD display on the I2C bus and register a file descriptor handle */
  fd = ConnectI2CSlave(Filename, LCD_I2C_ADDRESS);
    
  /* Setup the LCD */
  InitializeLCD();

  /* Move cursor to first line 0th position */
  SetCursor(0, 0);
  
  /* Print some text on the LCD */
  PrintText(text, strlen(text)-1);
  
  return 0;
}