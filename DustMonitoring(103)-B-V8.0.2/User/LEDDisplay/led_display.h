#ifndef  LED_DISPLAY_
#define  LED_DISPLAY_

#include "stm32f10x.h"

#define DISPLAY_DISABLE                   0
#define DISPLAY_LEVEL_WELCOME             1
#define DISPLAY_SPEED_DIRECT              2
#define DISPLAY_TEMP_HUMI                 3
#define DISPLAY_NOISE_TSP                 4
#define DISPLAY_PM25_PM10                 5

void DisplayNumberMode(void);

void UpdateDisplay(void);

#endif /* LED_DISPLAY_ */

