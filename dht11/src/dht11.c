//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program
//  15-January-2012
//  Dom and Gert
//


// Access from ARM Running Linux

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */


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

#include "dht11.h"

int main(int argc, char **argv)
{
  SensorData_t *pSensorData;
  
  if (!bcm2835_init())
        return 1;

  while(1)
  {

    pSensorData = GetData(DHT11_1_Pin);
    printf("DHT11 #1: Temp: %d, RH: %d, DP: %f\n", pSensorData->TemperatureC, pSensorData->RHPercent, pSensorData->DevPointC);

    pSensorData = GetData(DHT11_2_Pin);
    printf("DHT11 #2: Temp: %d, RH: %d, DP: %f\n", pSensorData->TemperatureC, pSensorData->RHPercent, pSensorData->DevPointC);
  
    usleep(3000000);
  }
  
  return 0;
}

float DevPoint(int T, int RH)
{
  return 0;
}

SensorData_t* GetData(int pin)
{
  /* Allocate memory to which return point will be given */
  static SensorData_t ReturnData;
  ReturnData.NewData = 0;
  
  int bits[250], data[100];
  int bitidx = 0;
  
  int counter = 0;
  int laststate = HIGH;
  int j=0, is=0;

  // Set GPIO pin to output
  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);

  bcm2835_gpio_write(pin, HIGH);
  usleep(500000);  // 500 ms
  bcm2835_gpio_write(pin, LOW);
  usleep(20000);

  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);

  data[0] = data[1] = data[2] = data[3] = data[4] = 0;

  // wait for pin to drop?
  while (bcm2835_gpio_lev(pin) == 1)
  {
    usleep(1);
  }

  // read data!
  for (is=0; is< MAXTIMINGS; is++)
  {
    counter = 0;
    while ( bcm2835_gpio_lev(pin) == laststate)
    {
      counter++;
      //nanosleep(1);		// overclocking might change this?
      if (counter == 1000) break;
    }
  
    laststate = bcm2835_gpio_lev(pin);
  
    if (counter == 1000) break;
  
    bits[bitidx++] = counter;

    if ((is>3) && (is%2 == 0))
    {
      // shove each bit into the storage bytes
      data[j/8] <<= 1;
      if (counter > 200) data[j/8] |= 1;
      j++;
    }
  }

/*
  printf("Debug\n");
  for (is=3; is<bitidx; is+=2) {
    printf("bit %d: %d\n", is-3, bits[is]);
    printf("bit %d: %d (%d)\n", is-2, bits[is+1], bits[is+1] > 200);
  }
*/
  
  //printf("Data (%d): 0x%x 0x%x 0x%x 0x%x 0x%x\n", j, data[0], data[1], data[2], data[3], data[4]);

  if ((j >= 39) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) )
  {
    // yay!
    //printf("Temp = %d *C, Hum = %d \%\n", data[2], data[0]);

    ReturnData.TemperatureC = data[2];
    ReturnData.RHPercent = data[0];
    ReturnData.DevPointC = DevPoint(data[2], data[0]);
    ReturnData.NewData = 1;
    return &ReturnData;
  }

  return NULL;
}
