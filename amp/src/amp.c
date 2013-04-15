//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program
//  15-January-2012
//  Dom and Gert
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <bcm2835.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <math.h>

#include "amp.h"

int main(int argc, char **argv)
{  
  if (bcm2835_init() == 0)
  {
    exit(0);
  }

  InitPin(AMP_R_CHANNEL);
  InitPin(AMP_L_CHANNEL);
  
  while(1)
  {
    ControlPin(AMP_R_CHANNEL, HIGH);
    ControlPin(AMP_L_CHANNEL, LOW);
    printf("Setting pins\n");
    usleep(500000);
    ControlPin(AMP_R_CHANNEL, LOW);
    ControlPin(AMP_L_CHANNEL, HIGH);
    printf("Toggling pins\n");
    usleep(500000);
  }
  
  return 0;
}

void InitPin(int pin)
{
  // Set GPIO pin to output
  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
}

void ControlPin(int pin, int state)
{
  bcm2835_gpio_write(pin, state);
}
