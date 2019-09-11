#ifndef _PM25_UART3_H_
#define _PM25_UART3_H_

#include "bsp.h"

void PM25_UART3_Init(void);
void ParsePM25Command(void);
void Open_Measurement(void);
void Close_Measurement(void);
#endif /* _LED_UART_H_ */
