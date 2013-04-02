


/* Access from ARM Running Linux */

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define MAXTIMINGS 100

#define DHT11_1_Pin  4
#define DHT11_2_Pin 17

#define DHT11_OK 0
#define DHT11_ERROR 1
#define DHT11_ERROR_TIMEOUT 2
#define DHT11_ERROR_CHECKSUM 3

typedef struct
{
  int TemperatureC;
  int RHPercent;
  float DevPointC;
  unsigned int NewData;
} SensorData_t;

/* Returns temp and RH in array[2] */
SensorData_t* GetData(int pin);

/* Calculate the devpoint from temperature and relative humidity */
double DewPoint(double celsius, double humidity);
