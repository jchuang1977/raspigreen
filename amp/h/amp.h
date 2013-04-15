


/* Access from ARM Running Linux */

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define MAXTIMINGS 100

#define AMP_R_CHANNEL 22
#define AMP_L_CHANNEL 23

void ControlPin(int pin, int state);
void InitPin(int pin);
