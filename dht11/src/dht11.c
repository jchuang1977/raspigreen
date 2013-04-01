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

#include "dht11.h"

int main(int argc, char **argv)
{
  SensorData_t *pSensorData;
  
  if (bcm2835_init() == 0)
  {
    exit(0);
  }

  while(1)
  {
//    printf("Entered while\n");
    /* Read the data from sensor #1 */
    pSensorData = GetData(DHT11_1_Pin);
  
    /* When the sensor reads data print it */
    if(pSensorData->NewData)
    {
      printf("DHT11 #1: Temp: %d, RH: %d, DP: %f\n", pSensorData->TemperatureC, pSensorData->RHPercent, pSensorData->DevPointC);
    }
  else
      printf("blah1\n");

    /* Read the data from sensor #1 */
    pSensorData = GetData(DHT11_2_Pin);
  
    /* When the sensor reads data print it */
    if(pSensorData->NewData)
    {
      printf("DHT11 #2: Temp: %d, RH: %d, DP: %f\n", pSensorData->TemperatureC, pSensorData->RHPercent, pSensorData->DevPointC);
    }
  else
      printf("blah2\n");
  
    usleep(3000000);
    printf("slept\n\n");
  }
  
  return 0;
}

float DevPoint(int T, int RH)
{
  /*
   gamma(T, RF) = ln (RH / 100) + b * T / (c + T)
   Tdp = c * gamma(T, RF) / (b - gamma(T, RH)
   a = 6.112, b = 17.67
   */
  
  return 0;
}

int bits[250], data[100];
int bitidx = 0;

SensorData_t* GetData(int pin)
{
  /* Allocate memory to which return point will be given */
  static SensorData_t ReturnData;
  ReturnData.NewData = 0;
  
  int counter = 0;
  int laststate = HIGH;
  int j=0;
  
  // Set GPIO pin to output
  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
  
  bcm2835_gpio_write(pin, HIGH);
  usleep(500000);  // 500 ms
  bcm2835_gpio_write(pin, LOW);
  usleep(20000);
  
  printf("Hest 1\n");
  
  bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
  
  data[0] = data[1] = data[2] = data[3] = data[4] = 0;
  
  printf("Hest 2\n");
  
  // wait for pin to drop?
  while (bcm2835_gpio_lev(pin) == 1) {
    usleep(1);
  }
  
  // read data!
  int i;
  for (i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while ( bcm2835_gpio_lev(pin) == laststate) {
      counter++;
      //nanosleep(1);		// overclocking might change this?
      if (counter == 1000)
        break;
    }
    laststate = bcm2835_gpio_lev(pin);
    if (counter == 1000) break;
    bits[bitidx++] = counter;
    
    if ((i>3) && (i%2 == 0)) {
      // shove each bit into the storage bytes
      data[j/8] <<= 1;
      if (counter > 200)
        data[j/8] |= 1;
      j++;
    }
  }

  printf("Hest 3\n");
  
  printf("Data (%d): 0x%x 0x%x 0x%x 0x%x 0x%x\n", j, data[0], data[1], data[2], data[3], data[4]);
  
  if ((j >= 39) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)))
  {
    // yay!
    //printf("Temp = %d *C, Hum = %d \%\n", data[2], data[0]);
    ReturnData.TemperatureC = data[2];
    ReturnData.RHPercent = data[0];
    ReturnData.DevPointC = DevPoint(data[2], data[0]);
    ReturnData.NewData = 1;
    printf("Jordbær\n");
    return &ReturnData;
  }
  
  printf("Svesker\n");
  return &ReturnData;
}
